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
 * $brcm_Revision: Hydra_Software_Devel/36 $
 * $brcm_Date: 8/30/12 11:05a $
 *
 * Porting interface code for the data transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/src/core40nm/bxpt_playback.c $
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
#include "bxpt_xcbuf_priv.h"
#include "bxpt_spid.h"

#include "bchp_xpt_pb0.h"
#include "bchp_xpt_fe.h"

#include "bxpt.h"
#include "bchp_xpt_pb1.h"

#if BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#if BXPT_HAS_TSMUX
#include "bchp_xpt_pb_top.h"
#endif

#define PB_PARSER_REG_STEPSIZE  ( BCHP_XPT_PB1_CTRL1 - BCHP_XPT_PB0_CTRL1 )

#if( BDBG_DEBUG_BUILD == 1 )
BDBG_MODULE( xpt_playback );
#endif

#define BXPT_P_PLAYBACK_DEFAULT_USE_PCR_TIMEBASE        false
#define BXPT_P_PLAYBACK_DEFAULT_TIMESTAMP_MODE          BXPT_TimestampMode_e30_2U_Mod300    
#define BXPT_PB_MAX_SYNC_LENGTH                         ( 256 )
#define FLUSH_COUNTDOWN                                 ( 10 )      


static BERR_Code BXPT_Playback_P_CreateDesc( BXPT_Handle hXpt, BXPT_PvrDescriptor * const Desc, uint8_t *Buffer, 
    uint32_t BufferLength, bool IntEnable, bool ReSync, BXPT_PvrDescriptor * const NextDesc );

static void TsRangeErrorIsr ( void *hPb , int Param2);

static BERR_Code CalcAndSetBlockout( 
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    uint32_t BitRate            /* [in] Rate, in bits per second. */
    );

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
    uint32_t Reg;

    uint32_t BaseAddr = 0;
    BERR_Code ExitCode = BERR_SUCCESS;
    BXPT_Playback_Handle hPb = NULL;
    BINT_Id TsRangeErrorIntId;                                           
    BXPT_PidChannel_CC_Config DefaultCcConfig = { false, false, false, 0 };
    uint8_t *CachedDummyBuffer;

    BDBG_ASSERT( hXpt );
    BDBG_ASSERT( ChannelSettings );

    if( !BXPT_Playback_IsAvailable( hXpt, ChannelNo ) ) 
    {
        BDBG_ERR(( "Channel Number %lu is out of range or already openned!", ( unsigned long ) ChannelNo ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        return ExitCode;
    }

    /* 
    ** Use the address of the first register in the playback block as the 
    ** base address of the entire block. 
    */           
    BaseAddr = BCHP_XPT_PB0_CTRL1 + ( ChannelNo * PB_PARSER_REG_STEPSIZE );

    /* Create the playback handle. */
    hPb = &hXpt->PlaybackHandles[ ChannelNo ];
    hPb->hChip = hXpt->hChip;
    hPb->hRegister = hXpt->hRegister;
    hPb->BaseAddr = BaseAddr;
    hPb->ChannelNo = ChannelNo;
    hPb->LastDescriptor_Cached = 0;
    hPb->Running = false;
    hPb->vhXpt = hXpt;
    hPb->SyncMode = BXPT_PB_SYNC_MPEG_BLIND;
    hPb->BitRate = 27000000;
    hPb->IsParserInAllPass = false;
    hPb->CcConfigBeforeAllPass = DefaultCcConfig;
    hPb->ForceResync = 0;

#if BXPT_HAS_TSMUX
    hPb->PacingLoadMap = 0;
    hPb->PacingCount = 0;
    hPb->PacingLoadNeeded = false;
#endif

    /* If they've got a separate heap for playback, use it */
    if( hXpt->hPbHeap )
        hPb->hMemory = hXpt->hPbHeap;
    else
        hPb->hMemory = hXpt->hMemory;

    TsRangeErrorIntId = BXPT_Playback_GetIntId( hPb, BXPT_PbInt_eTsRangeErr );

    ExitCode = BINT_CreateCallback(
        &hPb->hPbIntCallback,
        hXpt->hInt,
        TsRangeErrorIntId,
        TsRangeErrorIsr,
        ( void * ) hPb,
        0 );
    if( BERR_SUCCESS != ExitCode )
    {
        ExitCode = BERR_TRACE( ExitCode );
        goto Done;
    }

    if ( ChannelSettings->ResetPacing) 
    {
        hPb->ResetPacingOnTsRangeError = true;
        ExitCode = BINT_EnableCallback( hPb->hPbIntCallback );
        if( BERR_SUCCESS != ExitCode )
        {
            ExitCode = BERR_TRACE( ExitCode );
            goto ErrorCleanUp;
        }
    } 
    else
    {
        hPb->ResetPacingOnTsRangeError = false;
        ExitCode = BINT_DisableCallback( hPb->hPbIntCallback );
        if( BERR_SUCCESS != ExitCode )
        {
            ExitCode = BERR_TRACE( ExitCode );
            goto ErrorCleanUp;
        }
    }

    /* Restore all the stuff they could have changed through the API. */
    Reg = (
        BCHP_FIELD_DATA( XPT_PB0_CTRL1, WAKE_MODE, 0 ) |
        BCHP_FIELD_DATA( XPT_PB0_CTRL1, RUN, 0 ) |
        BCHP_FIELD_DATA( XPT_PB0_CTRL1, WAKE, 0 ) 
    );

    /* Anand's playback hw fix didn't catch one corner case. Turn on the chicken bit. */
    #if 0
    Reg |= ( BCHP_FIELD_DATA( XPT_PB0_CTRL1, AGRESSIVE_DESC_READ, 1 ) );
    #endif

    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL1, Reg ); 

    /* Now load the defaults they passed in. */
    BXPT_Playback_SetChannelSettings( hPb, ChannelSettings );

    /* Create the dummy descriptor and buffer */    
    hPb->DummyDescriptor = BMEM_AllocAligned( hPb->hMemory, sizeof( BXPT_PvrDescriptor ), 4, 0 ); 
    if( !hPb->DummyDescriptor )
    {
        BDBG_ERR(( "DummyDescriptor alloc failed!" ));
        ExitCode = BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
        goto ErrorCleanUp;
    }
    hPb->DummyBuffer = BMEM_AllocAligned( hPb->hMemory, 2, 0, 0 );
    if( !hPb->DummyBuffer )
    {
        BDBG_ERR(( "DummyBuffer alloc failed!" ));
        BMEM_Free( hPb->hMemory, hPb->DummyDescriptor );
        ExitCode = BERR_TRACE( BERR_OUT_OF_DEVICE_MEMORY );
        goto ErrorCleanUp;
    }

    /* Flushing the cache is done in BXPT_Playback_CreateDesc() */
    BMEM_ConvertAddressToCached(hPb->hMemory, (void *) hPb->DummyBuffer, (void **) &CachedDummyBuffer);
    CachedDummyBuffer[ 0 ] = CachedDummyBuffer[ 1 ] = 0x55; 

    BXPT_Playback_CreateDesc( hXpt, hPb->DummyDescriptor, hPb->DummyBuffer, 2, false, false, NULL );
    BMEM_FlushCache(hPb->hMemory, CachedDummyBuffer, 2 );
    hPb->Opened = true;
    *PlaybackHandle = hPb;
    
    Done:
    return( ExitCode );

    ErrorCleanUp:
    (void) BINT_DisableCallback( hPb->hPbIntCallback );
    (void) BINT_DestroyCallback( hPb->hPbIntCallback );
    return( ExitCode );
}

void BXPT_Playback_CloseChannel(
    BXPT_Playback_Handle PlaybackHandle /* [in] Handle for the channel to close*/
    )
{
    /* Stop the channel. */
    uint32_t Reg = (
        BCHP_FIELD_DATA( XPT_PB0_CTRL1, WAKE_MODE, 0 ) |
        BCHP_FIELD_DATA( XPT_PB0_CTRL1, RUN, 0 ) |
        BCHP_FIELD_DATA( XPT_PB0_CTRL1, WAKE, 0 ) 
    );

    BINT_DestroyCallback( PlaybackHandle->hPbIntCallback );
    BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1, Reg ); 

    /* Clean up all previous Packetization state, if any. */
    BXPT_Playback_DisablePacketizers( PlaybackHandle );

    BMEM_Free( PlaybackHandle->hMemory, PlaybackHandle->DummyDescriptor );
    BMEM_Free( PlaybackHandle->hMemory, PlaybackHandle->DummyBuffer );
    
    PlaybackHandle->Opened = false;
}

void TsRangeErrorIsr( void *hPb, int Param2 )
{
    uint32_t Reg; 

    BDBG_ASSERT( hPb );

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 ); 

    /* 
    ** Init the Pacing bitfields to 0. The PB_PACING_START bit must transistion from 0 to 1
    ** in order to arm the pacing logic. 
    */
    Reg &= ~( BCHP_MASK( XPT_PB0_CTRL2, PACING_START ) );

    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL2, Reg );

    Reg |= ( BCHP_FIELD_DATA( XPT_PB0_CTRL2, PACING_START, 1 ) );  

    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL2, Reg );
    BSTD_UNUSED( Param2 );
    return;
}

BERR_Code BXPT_Playback_SetChannelSettings(
    BXPT_Playback_Handle hPb,       /* [in] Handle for the playback channel. */
    const BXPT_Playback_ChannelSettings *ChannelSettings /* [in] New settings to use */
    )
{
    uint32_t Reg, TimeBaseSel;

    unsigned int PacketLength = 188;    /* Use MPEG length as the default. */
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hPb );
    BDBG_ASSERT( ChannelSettings );

    hPb->SyncMode = ChannelSettings->SyncMode;

    if( ChannelSettings->PacketLength > BXPT_PB_MAX_SYNC_LENGTH )
    {
        BDBG_ERR(( "Packet length %d too long! Clamped to %d", ChannelSettings->PacketLength, BXPT_PB_MAX_SYNC_LENGTH )); 
        PacketLength = BXPT_PB_MAX_SYNC_LENGTH;
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }
    else
    {
        PacketLength = ChannelSettings->PacketLength;
    }

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL3 );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_CTRL3, SYNC_LENGTH ) 
        );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_CTRL3, SYNC_LENGTH, PacketLength )
        );
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL3, Reg ); 

    /* Need to recalculate the blockout value if the sync length has changed. */
    CalcAndSetBlockout( hPb, hPb->BitRate );

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_CTRL2, SYNC_EXT_MODE ) |
        BCHP_MASK( XPT_PB0_CTRL2, ENDIAN_CTRL ) |
        BCHP_MASK( XPT_PB0_CTRL2, TS_PARITY_CHECK_DIS ) | 
        BCHP_MASK( XPT_PB0_CTRL2, TIMESTAMP_EN ) |
        BCHP_MASK( XPT_PB0_CTRL2, PROGRAM_STREAM_MODE ) |
        BCHP_MASK( XPT_PB0_CTRL2, PROGRAM_STREAM_EN )
        | 
		BCHP_MASK( XPT_PB0_CTRL2, PACING_OFFSET_ADJ_DIS )
        );

    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_CTRL2, SYNC_EXT_MODE, ChannelSettings->SyncMode ) |
        BCHP_FIELD_DATA( XPT_PB0_CTRL2, TS_PARITY_CHECK_DIS, ChannelSettings->DisableTimestampParityCheck == true ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_PB0_CTRL2, ENDIAN_CTRL, ChannelSettings->SwapBytes == true ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_PB0_CTRL2, TIMESTAMP_EN, ChannelSettings->TimestampEn == true ? 1 : 0 ) |
		BCHP_FIELD_DATA( XPT_PB0_CTRL2, PACING_OFFSET_ADJ_DIS, ChannelSettings->PacingOffsetAdjustDisable == true ? 1 : 0 )
        );
        Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL2, PROGRAM_STREAM_MODE, ChannelSettings->PsMode );
        if( ChannelSettings->PackHdrConfig != BXPT_Playback_PackHdr_Drop)
        {
            Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL2, PROGRAM_STREAM_EN, 1 ); 
        }
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL2, Reg ); 

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL4 );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_CTRL4, OUTPUT_PIPE_SEL ) |
        BCHP_MASK( XPT_PB0_CTRL4, TIMEBASE_SEL ) |
        BCHP_MASK( XPT_PB0_CTRL4, TIMESTAMP_MODE )| 
        BCHP_MASK( XPT_PB0_CTRL4, PKTZ_PACK_HDR_INSERT_EN ) 
        );

    if( ChannelSettings->UsePcrTimeBase == false )
    {
        /* Use the free running 27 MHz clock. */
        TimeBaseSel = 0;
    }
    else
    {
        if( ChannelSettings->WhichPcrToUse > BXPT_NUM_PCRS ) 
        {
            /* Bad PCR module number. Complain, and default to free running 27 MHz clock. */
            BDBG_ERR(( "WhichPcrToUse %lu is out of range!", ( unsigned long ) ChannelSettings->WhichPcrToUse ));
            ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
            TimeBaseSel = 0;
        }
    
        TimeBaseSel = ChannelSettings->WhichPcrToUse + 1;
    }

    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_CTRL4, OUTPUT_PIPE_SEL, ChannelSettings->RaveOutputOnly == true ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_PB0_CTRL4, TIMEBASE_SEL, TimeBaseSel ) |
        BCHP_FIELD_DATA( XPT_PB0_CTRL4, TIMESTAMP_MODE, ChannelSettings->TimestampMode ) 
        );
    if (ChannelSettings->PackHdrConfig == BXPT_Playback_PackHdr_Adaptation_Insert)
    {
        Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_PACK_HDR_INSERT_EN, 1 ); 
    }

    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL4, Reg ); 

    /* PCR-based pacing */
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_CTRL2, PACING_TYPE ) |    
        BCHP_MASK( XPT_PB0_CTRL2, TIMESTAMP_MODE_32BIT ) 
        );
    if( ChannelSettings->PcrBasedPacing == true )
    {
        Reg |= (
            BCHP_FIELD_DATA( XPT_PB0_CTRL2, PACING_TYPE, 1 ) |
            BCHP_FIELD_DATA( XPT_PB0_CTRL2, TIMESTAMP_MODE_32BIT, 1 )
            );
    }

#if BXPT_HAS_TSMUX
    else if( ChannelSettings->PesBasedPacing == true )
    {
        Reg |= (
            BCHP_FIELD_DATA( XPT_PB0_CTRL2, PACING_TYPE, 2 )
            );
    }
#endif

    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL2, Reg ); 

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PACING_PCR_PID );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_PACING_PCR_PID, PACING_PCR_PID ) 
        );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_PACING_PCR_PID, PACING_PCR_PID, ChannelSettings->PcrPacingPid )
        );
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_PACING_PCR_PID, Reg ); 

    /* Don't overwrite TIMESTAMP_MODE_32BIT if PCR pacing is in use */
    if( ChannelSettings->PcrBasedPacing == false )
    {
        Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 );
        Reg &= ~( BCHP_MASK( XPT_PB0_CTRL2, TIMESTAMP_MODE_32BIT ) );
        Reg |= ( BCHP_FIELD_DATA( XPT_PB0_CTRL2, TIMESTAMP_MODE_32BIT, ChannelSettings->Use32BitTimestamps == true ? 1 : 0 ) );
        BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL2, Reg ); 
    }

    if ( ChannelSettings->ResetPacing) 
    {
        hPb->ResetPacingOnTsRangeError = true;
        ExitCode = BINT_EnableCallback( hPb->hPbIntCallback );
        if (ExitCode) 
            return BERR_TRACE(ExitCode);
    } 
    else
    {
        hPb->ResetPacingOnTsRangeError = false;
        BINT_DisableCallback( hPb->hPbIntCallback );
    }

#if BXPT_HAS_TSMUX
    /* Set LLD Type */
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL1 );
    Reg &= ~( BCHP_MASK( XPT_PB0_CTRL1, LLD_TYPE ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_PB0_CTRL1, LLD_TYPE, ChannelSettings->Use8WordDesc == true ? 1 : 0 ) );
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL1, Reg );
#endif

    return( ExitCode );
}

BERR_Code BXPT_Playback_GetChannelSettings(
    BXPT_Playback_Handle hPb,       /* [in] Handle for the playback channel. */
    BXPT_Playback_ChannelSettings *ChannelSettings /* [out] The current settings  */
    )
{
    uint32_t Reg;

    uint32_t Timebase = 0;
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hPb );
    BDBG_ASSERT( ChannelSettings );

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL3 );
    ChannelSettings->PacketLength = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL3, SYNC_LENGTH );

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 );
    ChannelSettings->SyncMode = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, SYNC_EXT_MODE );
    ChannelSettings->SwapBytes = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, ENDIAN_CTRL ) ? true : false;
    ChannelSettings->TimestampEn = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, TIMESTAMP_EN ) ? true : false;
    ChannelSettings->DisableTimestampParityCheck = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, TS_PARITY_CHECK_DIS ) ? true : false;
	ChannelSettings->ResetPacing = hPb->ResetPacingOnTsRangeError;
	ChannelSettings->PacingOffsetAdjustDisable = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, PACING_OFFSET_ADJ_DIS ) ? true : false;
    ChannelSettings->Use32BitTimestamps = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, TIMESTAMP_MODE_32BIT ) ? true : false;

    ChannelSettings->PsMode = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, PROGRAM_STREAM_MODE ); 
    if( !BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, PROGRAM_STREAM_EN ) )
        ChannelSettings->PackHdrConfig = BXPT_Playback_PackHdr_Drop;
    else
        ChannelSettings->PackHdrConfig = BXPT_Playback_PackHdr_Payload_Insert;

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL4 );
    ChannelSettings->TimestampMode = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL4, TIMESTAMP_MODE );
    ChannelSettings->RaveOutputOnly = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL4, OUTPUT_PIPE_SEL ) ? true : false;
    Timebase = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL4, TIMEBASE_SEL );

    /* 
    ** In the Timebase check below, note that the hardware maps timebases with DPCR0 given a value of 1, 
    ** DPCR1 is 2, etc. Thus, the check below is > as opposed to >= 
    */
    if( Timebase == 0 )
        ChannelSettings->UsePcrTimeBase = false; 
    else if( Timebase > BXPT_NUM_PCRS )   
    {
        BDBG_ERR(( "Invalid timebase %u configured in hardware.", ( unsigned long ) Timebase ));
        ChannelSettings->UsePcrTimeBase = false; 
    }
    else
    {
        ChannelSettings->UsePcrTimeBase = true; 
        ChannelSettings->WhichPcrToUse = Timebase - 1; 
    }

    if( ChannelSettings->PackHdrConfig != BXPT_Playback_PackHdr_Drop )
    {
        if( BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL4, PKTZ_PACK_HDR_INSERT_EN ) )
            ChannelSettings->PackHdrConfig = BXPT_Playback_PackHdr_Adaptation_Insert;
    }

    /* PCR-based pacing */
    ChannelSettings->PcrBasedPacing = false;
#if BXPT_HAS_TSMUX
    ChannelSettings->PesBasedPacing = false;
#endif
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 );
    switch( BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, PACING_TYPE ) )
    {
        case 1:
        ChannelSettings->PcrBasedPacing = true;
        break;

#if BXPT_HAS_TSMUX
        case 2: 
        ChannelSettings->PesBasedPacing = true;
        break;
#endif

        case 0:
        default:
        /* Do nothing. Timestamp mode is the hw default. */
        break;
    }

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PACING_PCR_PID );
    ChannelSettings->PcrPacingPid = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_PACING_PCR_PID, PACING_PCR_PID );

#if BXPT_HAS_TSMUX
    /* Get the LLD Type */
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL1 );
    ChannelSettings->Use8WordDesc = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL1, LLD_TYPE ) ? true : false;
#endif

    return( ExitCode );
}

#ifdef ENABLE_PLAYBACK_MUX
BERR_Code BXPT_Playback_SetChannelPacketSettings(
	BXPT_Playback_Handle hPb,								   /* [in] Handle for the playback channel. */
    const BXPT_Playback_ChannelPacketSettings *ChannelSettings /* [in] New settings to use */
	)
{
	uint32_t Reg;

	unsigned int PacketLength = 188;	/* Use MPEG length as the default. */
	BERR_Code ExitCode = BERR_SUCCESS;

	BDBG_ASSERT( hPb );

	hPb->SyncMode = ChannelSettings->SyncMode;

	if( ChannelSettings->PacketLength > BXPT_PB_MAX_SYNC_LENGTH )
	{
		BDBG_ERR(( "Packet length %d too long! Clamped to %d", ChannelSettings->PacketLength, BXPT_PB_MAX_SYNC_LENGTH )); 
		PacketLength = BXPT_PB_MAX_SYNC_LENGTH;
		ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
	}
	else
	{
		PacketLength = ChannelSettings->PacketLength;
	}

	/*set the packet length*/                             
	Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL3 );
	Reg &= ~(BCHP_MASK( XPT_PB0_CTRL3, SYNC_LENGTH ));
  	Reg |= (BCHP_FIELD_DATA( XPT_PB0_CTRL3, SYNC_LENGTH, PacketLength ));
	BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL3, Reg ); 

    /* Need to recalc the blockout if the packet size has changed */
    CalcAndSetBlockout( hPb, hPb->BitRate );

	/*set the packet sync*/
	Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 );
	Reg &= ~( 
	    BCHP_MASK( XPT_PB0_CTRL2, SYNC_EXT_MODE ) |
	    BCHP_MASK( XPT_PB0_CTRL2, TIMESTAMP_EN ) |
  	    BCHP_MASK( XPT_PB0_CTRL2, TS_PARITY_CHECK_DIS )
  	);
  
  	Reg |= (BCHP_FIELD_DATA( XPT_PB0_CTRL2, SYNC_EXT_MODE, ChannelSettings->SyncMode ));
	if (ChannelSettings->TimestampEn == true) {
  		Reg |= (BCHP_FIELD_DATA( XPT_PB0_CTRL2, TIMESTAMP_EN, 1 ));
	}
	if (ChannelSettings->DisableTimestampParityCheck == true) {
  		Reg |= (BCHP_FIELD_DATA( XPT_PB0_CTRL2, TS_PARITY_CHECK_DIS, 1 ));
	}

	BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL2, Reg ); 


	return( ExitCode );
}
#endif /*ENABLE_PLAYBACK_MUX*/
      
BERR_Code BXPT_Playback_GetChannelStatus(
    BXPT_Playback_Handle hPb,           /* [in] Handle for the playback channel. */
    BXPT_Playback_ChannelStatus *Status /* [out] Channel status. */
    )
{
    uint32_t Reg, CurrDescReg, CurrDescNotDone, IntEnReg;
    
    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hPb );
    BDBG_ASSERT( Status );

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL1 );
    Status->Busy = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL1, BUSY ) ? true : false;
    Status->Run = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL1, RUN ) ? true : false;

    CurrDescReg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CURR_DESC_ADDR );
    CurrDescNotDone = BCHP_GET_FIELD_DATA( CurrDescReg, XPT_PB0_CURR_DESC_ADDR, CURR_DESC_NOT_DONE );
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL1 );
    Status->Finished = ~CurrDescNotDone && BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL1, FINISHED ) && Status->Run; 
    
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_INTR );
    Status->OutOfSync = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_INTR, SE_OUT_OF_SYNC_INT ) ? true : false;

    /* We can clear the status bit here if the interrupt isn't enabled */
    IntEnReg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_INTR_EN );
    if( !BCHP_GET_FIELD_DATA( IntEnReg, XPT_PB0_INTR, SE_OUT_OF_SYNC_INT ))
    {
        Reg = ~( 
            BCHP_MASK( XPT_PB0_INTR, SE_OUT_OF_SYNC_INT ) 
        );
        BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_INTR, Reg );
    }

    return( ExitCode ); 
}
      
BERR_Code BXPT_Playback_GetCurrentBufferAddress(
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    uint32_t *Address                       /* [out] The address read from hardware. */
    )
{
    uint32_t Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CURR_BUFF_ADDR ); 
    *Address = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CURR_BUFF_ADDR, CURR_BUFF_ADDR );

    return( ExitCode );
}

BERR_Code BXPT_Playback_GetCurrentDescriptorAddress(
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor **LastDesc       /* [in] Address of the current descriptor. */
    )
{
    uint32_t Reg, CurrentDescAddr;
    void *UserDescAddr;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hPb );

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CURR_DESC_ADDR );
    CurrentDescAddr = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CURR_DESC_ADDR, CURR_DESC_ADDR );
    CurrentDescAddr <<= 4;  /* Convert to byte-address. */
    BMEM_ConvertOffsetToAddress( hPb->hMemory, CurrentDescAddr, ( void ** ) &UserDescAddr );
    *LastDesc = ( BXPT_PvrDescriptor * ) UserDescAddr;
                                                                               
    return( ExitCode );
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
    BMEM_Handle hHeap;
    BXPT_PvrDescriptor *CachedDescPtr;

    BDBG_ASSERT( Desc );

    if( hXpt->hPbHeap )
        hHeap = hXpt->hPbHeap;
    else
        hHeap = hXpt->hMemory;

    /*  Set the Last Descriptor bit. */
    BMEM_ConvertAddressToCached(hHeap, (void *) Desc, (void **) &CachedDescPtr);
    CachedDescPtr->NextDescAddr = TRANS_DESC_LAST_DESCR_IND;
    BMEM_FlushCache(hHeap, CachedDescPtr, sizeof (BXPT_PvrDescriptor) );
}

#if BXPT_HAS_TSMUX
static void LoadPacingCounter(
    BXPT_Playback_Handle hPb   /* [in] Handle for the playback channel */
    )
{
    if( hPb->PacingLoadNeeded ) 
    {
        uint32_t Reg = hPb->PacingLoadMap;   

        /* Done to intentionally clear LD_STC_MUX_DELAY_DIFF */
        Reg &= ~( BCHP_MASK( XPT_PB_TOP_PACING_COUNT_RELOAD_CTRL, LD_STC_MUX_DELAY_DIFF ) );

        BREG_Write32( hPb->hRegister, BCHP_XPT_PB_TOP_PACING_COUNT_WR_VALUE, hPb->PacingCount );
        BREG_Write32( hPb->hRegister, BCHP_XPT_PB_TOP_PACING_COUNT_RELOAD_CTRL, Reg );
        hPb->PacingLoadNeeded = false;
    }
}
#endif

BERR_Code BXPT_Playback_AddDescriptors( 
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor *LastDesc,   /* [in] Last descriptor in new chain */
    BXPT_PvrDescriptor *FirstDesc   /* [in] First descriptor in new chain */
    )
{
    uint32_t Reg;
    uint32_t DescPhysAddr;    
    BXPT_PvrDescriptor *CachedFirstDesc, *CachedDescPtr;

    BERR_Code ExitCode = BERR_SUCCESS;
    BXPT_PvrDescriptor *LastDescriptor_Cached = ( BXPT_PvrDescriptor * ) hPb->LastDescriptor_Cached;

    BDBG_ASSERT( FirstDesc );

    BMEM_ConvertAddressToCached(hPb->hMemory, (void *) FirstDesc, (void **) &CachedFirstDesc);

    /* When the sync extractor is in bypass mode, the first descriptor must have it's FORCE_RESYNC flag set. */
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 );
    if( !LastDescriptor_Cached && BXPT_PB_SYNC_BYPASS == BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, SYNC_EXT_MODE ) )
    {
        CachedFirstDesc->Flags |= TRANS_DESC_FORCE_RESYNC_FLAG;
    }

    BMEM_FlushCache(hPb->hMemory, CachedFirstDesc, sizeof (BXPT_PvrDescriptor) );

    if( hPb->ForceResync ) 
    {
        /* cycle through all descriptors and set force_resync flag */
        BXPT_PvrDescriptor *TempDesc;
        BXPT_PvrDescriptor *Desc = CachedFirstDesc;
        BXPT_PvrDescriptor *HeadOfChain = CachedFirstDesc;

        do 
        {
            Desc->Flags |= TRANS_DESC_FORCE_RESYNC_FLAG;
            BMEM_FlushCache(hPb->hMemory, Desc, sizeof (BXPT_PvrDescriptor) );

            if( Desc->NextDescAddr == TRANS_DESC_LAST_DESCR_IND ) 
                break;

            BMEM_ConvertOffsetToAddress( hPb->hMemory, Desc->NextDescAddr, ( void ** ) &TempDesc );
            BMEM_ConvertAddressToCached(hPb->hMemory, (void *) TempDesc, (void **) &Desc);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
            BMEM_FlushCache(hPb->hMemory, Desc, sizeof (BXPT_PvrDescriptor) );
#else
            BMEM_FlushCache(hPb->hMemory, TempDesc, sizeof (BXPT_PvrDescriptor) );
#endif
        } 
        while( Desc != HeadOfChain );
    }

#if 0
    /* Needs to be updated for cached memory. */
    /* Walk through the list list, dumping each descriptor's contents */
    {
        #define FLAG_STR_LEN 40
        char FlagsStr[ FLAG_STR_LEN ];

        BXPT_PvrDescriptor *Desc = FirstDesc;
        BXPT_PvrDescriptor *HeadOfChain = FirstDesc;

        do
        {
            #include  <stdio.h>

            /*
            FILE *Fp;
            void *Buffer;

            Fp = fopen( "videos/pb_dump.mpg", "wb" );
            BMEM_ConvertOffsetToAddress( hPb->hMemory, Desc->BufferStartAddr, ( void ** ) &Buffer );
            fwrite( Buffer, 1, sizeof (char), Fp );
            fclose( Fp );
            */

            BMEM_Heap_FlushCache( hPb->hMemory, (void *) Desc, sizeof( BXPT_PvrDescriptor) );

            BKNI_Snprintf( FlagsStr, FLAG_STR_LEN, "%s%s", 
                     Desc->Flags & TRANS_DESC_FORCE_RESYNC_FLAG ? "Force Resync,": "",
                     Desc->Flags & TRANS_DESC_INT_FLAG ? "Interrupt": ""
                     );
            BDBG_MSG(( "Desc 0x%08lX: BufferStartAddr 0x%08lX, BufferLength %u, Flags 0x%08lX (%s), NextDescAddr 0x%08lX",
                       Desc, Desc->BufferStartAddr, Desc->BufferLength, Desc->Flags, FlagsStr, Desc->NextDescAddr ));

            if( Desc->NextDescAddr == TRANS_DESC_LAST_DESCR_IND ) 
                break;
            BMEM_ConvertOffsetToAddress( hPb->hMemory, Desc->NextDescAddr, ( void ** ) &Desc );
        }
        while( Desc != HeadOfChain );
    }
#endif

#if BXPT_HAS_TSMUX
    LoadPacingCounter( hPb );
#endif

    if( LastDescriptor_Cached )
    {
        /* Channel has a linked-list loaded already. Append this descriptor to the end and use WAKE_MODE = 0 (resume from the last descriptor). */

        /* Set the last descriptor in the chain to point to the descriptor we're adding. */
        BMEM_ConvertAddressToOffset( hPb->hMemory, ( void * ) FirstDesc, &DescPhysAddr );
        LastDescriptor_Cached->NextDescAddr = ( uint32_t ) DescPhysAddr;
        BMEM_FlushCache(hPb->hMemory, LastDescriptor_Cached, sizeof (BXPT_PvrDescriptor) );

        /* SWDTV-8330: read register after uncache memory write operation to maintain consistency */
        Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL1 ); 
        Reg &= ~ ( BCHP_MASK( XPT_PB0_CTRL1, WAKE_MODE ) );
        Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL1, WAKE, 1 );
        BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL1, Reg );
    }
    else
    {
        /* Channel does not have a linked-list loaded. Point the FIRST_DESC_ADDR to this descriptor and set RUN */
        BMEM_ConvertAddressToOffset( hPb->hMemory, ( void * ) FirstDesc, &DescPhysAddr );

        Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_FIRST_DESC_ADDR );
        Reg &= ~( BCHP_MASK( XPT_PB0_FIRST_DESC_ADDR, FIRST_DESC_ADDR ) );

        /* 
        ** The descriptor address field in the hardware register is wants the address
        ** in 16-byte blocks. See the RDB HTML for details. So, we must shift the 
        ** address 4 bits to the right before writing it to the hardware. Note that
        ** the RDB macros will shift the value 4 bits to the left, since the address
        ** bitfield starts at bit 4. Confusing, but thats what the hardware and the
        ** RDB macros require to make this work.
        */
        DescPhysAddr >>= 4;
        Reg |= BCHP_FIELD_DATA( XPT_PB0_FIRST_DESC_ADDR, FIRST_DESC_ADDR, DescPhysAddr );  
        BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_FIRST_DESC_ADDR, Reg );

        Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL1 ); 
        Reg &= ~ ( 
                  BCHP_MASK( XPT_PB0_CTRL1, WAKE_MODE ) |
                  BCHP_MASK( XPT_PB0_CTRL1, WAKE ) |
                  BCHP_MASK( XPT_PB0_CTRL1, RUN ) 
        );
        Reg |= (
                BCHP_FIELD_DATA( XPT_PB0_CTRL1, WAKE_MODE, 1 ) |    
                BCHP_FIELD_DATA( XPT_PB0_CTRL1, WAKE, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL1, RUN, 1 )
        );  
        BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL1, Reg );
    }

    if ( LastDesc )
    {
        BMEM_ConvertAddressToCached(hPb->hMemory, (void *) LastDesc, (void **) &CachedDescPtr);
        hPb->LastDescriptor_Cached = ( uint32_t ) CachedDescPtr;
    }
    else
    {
        hPb->LastDescriptor_Cached = 0;
    }
    return ExitCode;
}

BERR_Code BXPT_Playback_StartChannel( 
    BXPT_Playback_Handle PlaybackHandle     /* [in] Handle for the playback channel */
    )
{
    uint32_t Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( PlaybackHandle );

    BDBG_MSG(( "Starting playback channel %d", ( unsigned long ) PlaybackHandle->ChannelNo )); 

    if( PlaybackHandle->Running == true )
    {
        BDBG_ERR(( "Playback channel %d cannot be started because it's already running!", 
            ( unsigned long ) PlaybackHandle->ChannelNo )); 
        ExitCode = BERR_TRACE( BXPT_ERR_CHANNEL_ALREADY_RUNNING );
    }

#ifdef BCHP_PWR_RESOURCE_XPT_PLAYBACK
    if( PlaybackHandle->Running == false ) 
    {
        BCHP_PWR_AcquireResource(PlaybackHandle->hChip, BCHP_PWR_RESOURCE_XPT_PLAYBACK);
    }
#endif

    /* Check if we have buffers already loaded for this channel */
    if( PlaybackHandle->LastDescriptor_Cached )
    {
        /* Since we already have some buffers loaded, we can start the pvr channel */
        Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1 ); 
        Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL1, RUN, 1 );  
        BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1, Reg );
    }   

    /* Need to enable the parser band. */
    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_PARSER_CTRL1 );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_ENABLE, 1 ) 
    );
    BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_PARSER_CTRL1, Reg ); 

    PlaybackHandle->Running = true;

    return( ExitCode );
}


BERR_Code BXPT_Playback_StopChannel( 
    BXPT_Playback_Handle PlaybackHandle     /* [in] Handle for the playback channel */
    )
{
    volatile uint32_t Reg;
    uint32_t PacingEn;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( PlaybackHandle );

    BDBG_MSG(( "Stopping playback channel %d", ( unsigned long ) PlaybackHandle->ChannelNo )); 

    if( PlaybackHandle->Running == false )
    {
        BDBG_ERR(( "Playback channel %d cannot be stopped because it's not running!", 
            ( unsigned long ) PlaybackHandle->ChannelNo )); 
        ExitCode = BERR_TRACE( BXPT_ERR_CHANNEL_ALREADY_STOPPED );
    }

    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2 ); 
    PacingEn = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, PACING_EN );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_CTRL2, PACING_EN ) 
        );
    BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2, Reg );

    /* Stop the channel hardware */
    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1 ); 
    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_CTRL1, DISCARD_DMA_DATA, 1 ) 
    );  
    BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1, Reg );
    {
        int Wait;

        /* Disable pausing from the XC buffer. Allows data to drain */
        BXPT_XcBuf_P_EnablePlaybackPausing( (BXPT_Handle) PlaybackHandle->vhXpt, PlaybackHandle->ChannelNo, false );   
        for( Wait = 0; Wait < 100; Wait++ ) 
        {
            BKNI_Delay( 1 );  /* Busy wait for 1 uS */
        }

        /* Re-enable pausing from the XC buffer. Prevents overflows in RAVE. */
        BXPT_XcBuf_P_EnablePlaybackPausing( (BXPT_Handle) PlaybackHandle->vhXpt, PlaybackHandle->ChannelNo, true );   
    }

    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1 ); 
    Reg &= ~( BCHP_MASK( XPT_PB0_CTRL1, RUN ) );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_CTRL1, RUN, 0 )
    );  
    BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1, Reg );

    /* Clear the first desc addr (for cleaner debugging) */
    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_FIRST_DESC_ADDR );
    Reg &= ~( BCHP_MASK( XPT_PB0_FIRST_DESC_ADDR, FIRST_DESC_ADDR ) );
    BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_FIRST_DESC_ADDR, Reg );

    /* Clear the host pause. This shouldn't be restored, though. */
    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2 );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_CTRL2, MICRO_PAUSE ) 
        );
    BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2, Reg );

    PlaybackHandle->LastDescriptor_Cached = 0;     

#if 1
    /* Keep this around, just in case. */
    {
        uint32_t Busy;
        int WaitCount;
        uint32_t FirstDescAddr, SyncExtMode;
        uint32_t DescPhysAddr;    
        uint32_t DescNotDone;

        /* Wait for the BUSY bit to clear. */
        WaitCount = 100;          /* 100 uS max wait before we declare failure */
        do
        {
            Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1 );
            Busy = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL1, BUSY ) ? true : false;
    
            if( Busy )
            {
                WaitCount--;
                if( !WaitCount )
                {
                    BDBG_ERR(( "Playback channel %d timed-out waiting for BUSY bit to clear", 
                        ( unsigned long ) PlaybackHandle->ChannelNo )); 
                    ExitCode = BERR_TRACE( BERR_TIMEOUT );
                    goto Done;
                }
     
                BKNI_Delay( 1 );  /* Busy wait for 1 uS */
            }
        }
        while( Busy );

        Done:
	BKNI_Delay( 10 );   /* Wait 10 uS; */
        Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_FIRST_DESC_ADDR );
        FirstDescAddr = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_FIRST_DESC_ADDR, FIRST_DESC_ADDR );
        Reg &= ~( BCHP_MASK( XPT_PB0_FIRST_DESC_ADDR, FIRST_DESC_ADDR ) );
        BMEM_ConvertAddressToOffset( PlaybackHandle->hMemory, ( void * ) PlaybackHandle->DummyDescriptor, &DescPhysAddr );
        DescPhysAddr >>= 4;
        Reg |= BCHP_FIELD_DATA( XPT_PB0_FIRST_DESC_ADDR, FIRST_DESC_ADDR, DescPhysAddr );  
        BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_FIRST_DESC_ADDR, Reg );

        Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2 );
        SyncExtMode = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, SYNC_EXT_MODE );
        Reg &= ~BCHP_MASK( XPT_PB0_CTRL2, SYNC_EXT_MODE );
        Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL2, SYNC_EXT_MODE, 3 );
        BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2, Reg );
  
        Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1 );
        Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL1, RUN, 1 );
        BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1, Reg );

        WaitCount = 100;          /* 100 uS max wait before we declare failure */
        do
        {
            Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CURR_DESC_ADDR );
            DescNotDone = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CURR_DESC_ADDR, CURR_DESC_NOT_DONE );
    
            if( DescNotDone )
            {
                WaitCount--;
                if( !WaitCount )
                {
                    BDBG_ERR(( "Playback channel %d timed-out waiting for CURR_DESC_NOT_DONE bit to clear", 
                        ( unsigned long ) PlaybackHandle->ChannelNo )); 
                    ExitCode = BERR_TRACE( BERR_TIMEOUT );
                    goto Done2;
                }
     
                BKNI_Delay( 1 );  /* Busy wait for 1 uS */
            }

        }
        while( DescNotDone ); 

        Done2:       
        Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1 );
        Reg &= ~( BCHP_MASK( XPT_PB0_CTRL1, RUN ) );
        BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1, Reg );

        WaitCount = 100;          /* 100 uS max wait before we declare failure */
        do
        {
            Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1 );
            Busy = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL1, BUSY ) ? true : false;
    
            if( Busy )
            {
                WaitCount--;
                if( !WaitCount )
                {
                    BDBG_ERR(( "Playback channel %d timed-out waiting for BUSY bit to clear, 2nd loop", 
                        ( unsigned long ) PlaybackHandle->ChannelNo )); 
                    ExitCode = BERR_TRACE( BERR_TIMEOUT );
                    goto Done3;
                }
     
                BKNI_Delay( 1 );  /* Busy wait for 1 uS */
            }
        }
        while( Busy );
	
        Done3:
        Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_FIRST_DESC_ADDR );
        Reg &= ~( BCHP_MASK( XPT_PB0_FIRST_DESC_ADDR, FIRST_DESC_ADDR ) );
        Reg |= BCHP_FIELD_DATA( XPT_PB0_FIRST_DESC_ADDR, FIRST_DESC_ADDR, FirstDescAddr );  
        BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_FIRST_DESC_ADDR, Reg );

        Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2 );
        Reg &= ~BCHP_MASK( XPT_PB0_CTRL2, SYNC_EXT_MODE );
        Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL2, SYNC_EXT_MODE, SyncExtMode );
        BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2, Reg );
    }
#endif

    /* Need to disable the parser band. */
    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_PARSER_CTRL1 );
    Reg &= ~(
        BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_ENABLE ) 
    );
    BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_PARSER_CTRL1, Reg ); 

    /* Stop dropping data in the DMA buffers. */
    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1 ); 
    Reg &= ~( BCHP_MASK( XPT_PB0_CTRL1, DISCARD_DMA_DATA ) );
    BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1, Reg );

    /* Restore the pacing enable. */
    if( PacingEn ) 
    {
        Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2 ); 
        Reg |= (
            BCHP_FIELD_DATA( XPT_PB0_CTRL2, PACING_EN, 1 ) 
            );
        BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2, Reg );
    }

#ifdef BCHP_PWR_RESOURCE_XPT_PLAYBACK
    if (PlaybackHandle->Running) {
        BCHP_PWR_ReleaseResource(PlaybackHandle->hChip, BCHP_PWR_RESOURCE_XPT_PLAYBACK);
    }
#endif

    PlaybackHandle->Running = false;

    return( ExitCode );
}


BERR_Code BXPT_Playback_Pause( 
    BXPT_Playback_Handle PlaybackHandle     /* [in] Handle for the playback channel */
    )
{
    uint32_t Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( PlaybackHandle );

    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2 ); 
    Reg &= ~( BCHP_MASK( XPT_PB0_CTRL2, MICRO_PAUSE ) );
    Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL2, MICRO_PAUSE, 1 );  
    BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2, Reg );

    return( ExitCode );
}


BERR_Code BXPT_Playback_Resume( 
    BXPT_Playback_Handle PlaybackHandle     /* [in] Handle for the playback channel */
    )
{
    uint32_t Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( PlaybackHandle );

    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2 ); 
    if( BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL2, PACING_EN ) ) 
    {
        TsRangeErrorIsr( PlaybackHandle, 0 );
    }

    /* Reread the register, since TsRangeErrorIsr() modifies it. */
    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2 ); 
    Reg &= ~( BCHP_MASK( XPT_PB0_CTRL2, MICRO_PAUSE ) );
    Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL2, MICRO_PAUSE, 0 );  
    BXPT_Playback_P_WriteReg( PlaybackHandle, BCHP_XPT_PB0_CTRL2, Reg );

    return( ExitCode );
}                                                              

BERR_Code BXPT_Playback_SetBitRate( 
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    uint32_t BitRate            /* [in] Rate, in bits per second. */
    )
{
    BDBG_ASSERT( hPb );

    if( BitRate < BXPT_MIN_PLAYBACK_RATE ) 
    {
        BDBG_ERR(( "Minimum playback rate is %u bps. Rate will be clamped to that value", BXPT_MIN_PLAYBACK_RATE ));
        BitRate = BXPT_MIN_PLAYBACK_RATE;
    }
    else if ( BitRate > BXPT_MAX_PLAYBACK_RATE ) 
    {
        BDBG_ERR(( "Maximum playback rate is %u bps. Rate will be clamped to that value", BXPT_MAX_PLAYBACK_RATE ));
        BitRate = BXPT_MAX_PLAYBACK_RATE;
    }

    hPb->BitRate = BitRate;
    return CalcAndSetBlockout( hPb, BitRate );
}

BERR_Code CalcAndSetBlockout( 
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    uint32_t BitRate            /* [in] Rate, in bits per second. */
    )
{
    uint32_t Reg, SyncLen, BoCount;

    BERR_Code ExitCode = BERR_SUCCESS;

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL3 );
    SyncLen = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL3, SYNC_LENGTH );

#if BXPT_SW7425_1323_WORKAROUND
    {
        BXPT_Handle lhXpt = (BXPT_Handle) hPb->vhXpt;

        if( lhXpt->DoWorkaround )
            BitRate = BitRate <= 30000000 ? BitRate : 30000000; 
    }
#endif

    if(SyncLen == 0xB8)
        SyncLen = 0xBC;

    /* Do the math in units of 10kbps to avoid overflowing the 24-bit blockout bitfield in the register. */
    BoCount = (10800 * SyncLen * 8) / ( BitRate / 10000 );

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_BLOCKOUT ); 
    Reg &= ~( 
         BCHP_MASK( XPT_PB0_BLOCKOUT, BO_COUNT ) |
         BCHP_MASK( XPT_PB0_BLOCKOUT, BO_SPARE_BW_EN )
         );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_BLOCKOUT, BO_COUNT, BoCount ) 
        );  
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_BLOCKOUT, Reg );

    return( ExitCode );
}


BERR_Code BXPT_Playback_CheckHeadDescriptor( 
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor *Desc,       /* [in] Descriptor to check. */
    bool *InUse,                    /* [out] Is descriptor in use? */
    uint32_t *BufferSize            /* [out] Size of the buffer (in bytes). */
    )
{
    uint32_t Reg, ChanBusy, CurrentDescNotDone, CurrentDescAddr, CandidateDescPhysAddr;
    uint32_t DescFinish;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( PlaybackHandle );

    /* 
    ** Check if the current descriptor being processed by the
    ** playback hardware is the first on our hardware list
    ** (which means this descriptor is still being used)
    */
    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CURR_DESC_ADDR );
    CurrentDescNotDone = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CURR_DESC_ADDR, CURR_DESC_NOT_DONE );
    DescFinish = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CURR_DESC_ADDR, FINISHED_SHADOW ) ;
    DescFinish = (~CurrentDescNotDone && DescFinish) ;
    CurrentDescAddr = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CURR_DESC_ADDR, CURR_DESC_ADDR );
    CurrentDescAddr <<= 4;  /* Convert to byte-addressing */

    Reg = BXPT_Playback_P_ReadReg( PlaybackHandle, BCHP_XPT_PB0_CTRL1 );
    ChanBusy = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL1, BUSY );
    
    BMEM_ConvertAddressToOffset( PlaybackHandle->hMemory, ( void * ) Desc, &CandidateDescPhysAddr );

    if( CurrentDescAddr == CandidateDescPhysAddr )
    {
/*        if( ChanBusy && CurrentDescNotDone ) */
	if( !DescFinish )
        {
            /* The candidate descriptor is being used by hardware. */
            *InUse = true;
        }
        else
        {
            *InUse = false;
        }
    }
    else
    {
        /* 
        ** The candidate descriptor isn't being processed. If this is the head descriptor
        ** we can conclude that the hardware is finished with the descriptor. 
        */
        *InUse = false;

#if 0
/* Keep this around, just in case */
        if (Desc->NextDescAddr == 1)
        {
           /* If there's no next descriptor and the current descriptor isn't the head descriptor,
            * the head descriptor hasn't been read by the HW yet.  It's still InUse.
            */
           *InUse = true;
        }
        else
        {
            /* Check if our head descriptor is after the current one */
            BXPT_PvrDescriptor *  CurrentHwDesc;
            BMEM_ConvertOffsetToAddress( PlaybackHandle->hMemory, CurrentDescAddr, ( void * ) &CurrentHwDesc );
            
            /* If the head descriptor is the next descriptor, its still InUse */
            if (CurrentHwDesc->NextDescAddr == CandidateDescPhysAddr){
                *InUse = true;
            } 
        }
#endif
    }

    if( *InUse == false )
    {
        BXPT_PvrDescriptor *CachedDescPtr;

        BMEM_ConvertAddressToCached(PlaybackHandle->hMemory, (void *) Desc, (void **) &CachedDescPtr);
        BMEM_FlushCache(PlaybackHandle->hMemory, CachedDescPtr, sizeof (BXPT_PvrDescriptor) );
        *BufferSize = CachedDescPtr->BufferLength;
    }
    else
    {
        *BufferSize = 0;
    }

    return( ExitCode );
}

BERR_Code BXPT_Playback_GetTimestampUserBits(
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    unsigned int *Bits                          /* [out] The user bits read from hardware. */
    )
{
    uint32_t Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hPb );

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL4 );
    *Bits = ( int ) ( BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL4, TIMESTAMP_USER_BITS ) & 0xFF );

    return( ExitCode );
}

BERR_Code BXPT_Playback_ConfigPacing(
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    BXPT_PacingControl Mode                 /* [in] New mode for pacing. */
    )
{
    uint32_t Reg; 

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hPb );

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 ); 

    /* 
    ** Init the Pacing bitfields to 0. The PB_PACING_START bit must transistion from 0 to 1
    ** later on in order to arm the pacing logic. 
    */
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_CTRL2, PACING_START ) |
        BCHP_MASK( XPT_PB0_CTRL2, PACING_EN ) |
        BCHP_MASK( XPT_PB0_CTRL2, PACING_AUTOSTART_EN ) 
        );

    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL2, Reg );

    if( Mode == BXPT_PacingControl_eStart )
    {
        Reg |= (
            BCHP_FIELD_DATA( XPT_PB0_CTRL2, PACING_START, 1 ) |
            BCHP_FIELD_DATA( XPT_PB0_CTRL2, PACING_EN, 1 ) |

            /*
            ** Autostart will re-arm the pacing logic automatically if a 
            ** force-resync condition occurrs.
            */ 
            BCHP_FIELD_DATA( XPT_PB0_CTRL2, PACING_AUTOSTART_EN, 1 )
        );  
    }

    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL2, Reg );

    return( ExitCode );
}

BERR_Code BXPT_Playback_SetPacingErrorBound(
    BXPT_Playback_Handle hPb,       /* [in] Handle for the playback channel */
    unsigned long MaxTsError        /* [in] Maximum timestamp error. */
    )
{
    uint32_t Reg;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hPb );

    /* Newer chips */
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_TS_ERR_BOUND_EARLY );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_TS_ERR_BOUND_EARLY, TS_ERR_BOUND ) 
        );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_TS_ERR_BOUND_EARLY, TS_ERR_BOUND, MaxTsError )
    );  
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_TS_ERR_BOUND_EARLY, Reg ); 

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_TS_ERR_BOUND_LATE );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_TS_ERR_BOUND_LATE, TS_ERR_BOUND ) 
        );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_TS_ERR_BOUND_LATE, TS_ERR_BOUND, MaxTsError )
    );  
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_TS_ERR_BOUND_LATE, Reg ); 

    return( ExitCode );
}

void BXPT_Playback_GetPacketizerDefaults(
    const BXPT_Playback_PacketizeConfig *Cfg   /* [in] Config to initialize */
    )
{
    BDBG_ASSERT( Cfg );
    BKNI_Memset( (void *) Cfg, 0, sizeof( BXPT_Playback_PacketizeConfig ) );
}

BERR_Code BXPT_Playback_PacketizeStream(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    unsigned Context,                           /* [in] Which context to map the packets to. */
    const BXPT_Playback_PacketizeConfig *Cfg,   /* [in] Configuration for this context */
    bool Enable                                 /* [in] Start or stop packetization. */
    )
{
    /* In _CloseChannel(), disable all the packetizers */
    unsigned Index;
    uint32_t Reg, RegAddr;
    int32_t i;
    BXPT_Spid_eChannelFilter SidFilter;
    BXPT_Handle hXpt;
    BXPT_PidChannel_CC_Config ccConfig;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hPb );
    BDBG_ASSERT( Cfg );

    hXpt = (BXPT_Handle)hPb->vhXpt;

    if( Enable == true )
    {
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
       /*NONE*/ 
#else
        RegAddr = BCHP_XPT_PB0_PKTZ_CONTEXT0 + ( 4 * Context );

        Reg = BXPT_Playback_P_ReadReg( hPb, RegAddr );
        Reg &= ~( 
            BCHP_MASK( XPT_PB0_PKTZ_CONTEXT0, PID ) |
            BCHP_MASK( XPT_PB0_PKTZ_CONTEXT0, CH_NUM ) |
            BCHP_MASK( XPT_PB0_PKTZ_CONTEXT0, PES_STREAM_ID ) 
        );
        BXPT_Playback_P_WriteReg( hPb, RegAddr, Reg ); 
#endif
        Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 );
        Reg &= ~( 
                 BCHP_MASK( XPT_PB0_CTRL2, SYNC_EXT_MODE ) 
        );
        if (Cfg->PacketizerMode == BXPT_Playback_PacketizerMode_Es) 
        {
            hPb->ForceResync = true; /* for ES playback, the FORCE_RESYNC flag must be set for all descriptors. see SW7425-1672 */
            Reg &= ~( 
                 BCHP_MASK( XPT_PB0_CTRL2, PROGRAM_STREAM_EN )
            );
            Reg |= (
                BCHP_FIELD_DATA( XPT_PB0_CTRL2, SYNC_EXT_MODE, BXPT_PB_SYNC_BYPASS ) 
            );
        }
        else
        {
            hPb->ForceResync = false;
            Reg |= (
                BCHP_FIELD_DATA( XPT_PB0_CTRL2, SYNC_EXT_MODE, BXPT_PB_SYNC_PES )
            );
        }
        BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL2, Reg ); 

        Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PARSER_CTRL1 );
        Reg &= ~( 
            BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_PACKET_TYPE ) |
            BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_PKT_LENGTH ) |
            BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD ) 
            );
        Reg |= (
            BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_PACKET_TYPE, 0x00 ) |
            BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_PKT_LENGTH, 0xBC ) |
            BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD, 1 )
            );
        BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_PARSER_CTRL1, Reg );

        Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL4 );
        Reg &= ~( 
            BCHP_MASK( XPT_PB0_CTRL4, PKTZ_PUSI_SET_DIS ) |
            BCHP_MASK( XPT_PB0_CTRL4, PKTZ_CONTEXT_EN ) |
            BCHP_MASK( XPT_PB0_CTRL4, PACKETIZE_EN )|
            BCHP_MASK( XPT_PB0_CTRL4, PKTZ_PT_EN ) 
            );

        switch(Cfg->PacketizerMode)
        {
        case BXPT_Playback_PacketizerMode_Es:
            SidFilter.Mode = BXPT_Spid_eChannelFilterMode_StreamIdRange;
            SidFilter.FilterConfig.StreamIdRange.Hi = 0xFF;
            SidFilter.FilterConfig.StreamIdRange.Lo = 0x00;
            BXPT_Spid_P_ConfigureChannelFilter(hXpt,Cfg->ChannelNum,SidFilter);
            Reg |= ( 
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_PUSI_SET_DIS, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_SUB_ID_EN, 0 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_STREAM_ID_EXT_EN, 0 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_PT_EN, 1 )
            );
            break;

        case BXPT_Playback_PacketizerMode_Pes_MapAll: 
            /* Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_CONTEXT_EN, 1 );*/
            SidFilter.Mode = BXPT_Spid_eChannelFilterMode_StreamIdRange;
            SidFilter.FilterConfig.StreamIdRange.Hi = 0xff;
            SidFilter.FilterConfig.StreamIdRange.Lo = 0x00;
            BXPT_Spid_P_ConfigureChannelFilter(hXpt,Cfg->ChannelNum,SidFilter);
            Reg |= ( 
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_SUB_ID_EN, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_STREAM_ID_EXT_EN, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_PT_EN, 1 )
            );
            break;

        case BXPT_Playback_PacketizerMode_Pes_Sid:       
            SidFilter.Mode = BXPT_Spid_eChannelFilterMode_StreamId;
            SidFilter.FilterConfig.StreamId = Cfg->FilterConfig.StreamId;
            BXPT_Spid_P_ConfigureChannelFilter(hXpt,Cfg->ChannelNum,SidFilter);
            Reg |= ( 
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_SUB_ID_EN, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_STREAM_ID_EXT_EN, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_PT_EN, 1 )
            );
            break;

        case BXPT_Playback_PacketizerMode_Pes_SidRange:  
            SidFilter.Mode = BXPT_Spid_eChannelFilterMode_StreamIdRange;
            SidFilter.FilterConfig.StreamIdRange.Hi = Cfg->FilterConfig.StreamIdRange.Hi;
            SidFilter.FilterConfig.StreamIdRange.Lo = Cfg->FilterConfig.StreamIdRange.Lo;
            BXPT_Spid_P_ConfigureChannelFilter(hXpt,Cfg->ChannelNum,SidFilter);
            Reg |= ( 
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_SUB_ID_EN, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_STREAM_ID_EXT_EN, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_PT_EN, 1 )
            );
            break;

        case BXPT_Playback_PacketizerMode_Pes_SidExtension:  
            SidFilter.Mode = BXPT_Spid_eChannelFilterMode_StreamIdExtension;
            SidFilter.FilterConfig.StreamIdAndExtension.Id = Cfg->FilterConfig.StreamIdAndExtension.Id;
            SidFilter.FilterConfig.StreamIdAndExtension.Extension = Cfg->FilterConfig.StreamIdAndExtension.Extension;
            BXPT_Spid_P_ConfigureChannelFilter(hXpt,Cfg->ChannelNum,SidFilter);
            Reg |= ( 
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_SUB_ID_EN, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_STREAM_ID_EXT_EN, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_PT_EN, 1 )
            );
            break;

        case BXPT_Playback_PacketizerMode_Pes_SidSubSid: 
            SidFilter.Mode = BXPT_Spid_eChannelFilterMode_SubStreamId;
            SidFilter.FilterConfig.StreamIdAndSubStreamId.Id = Cfg->FilterConfig.StreamIdAndSubStreamId.Id;
            SidFilter.FilterConfig.StreamIdAndSubStreamId.SubStreamId = Cfg->FilterConfig.StreamIdAndSubStreamId.SubStreamId;
            BXPT_Spid_P_ConfigureChannelFilter(hXpt,Cfg->ChannelNum,SidFilter);
            Reg |= ( 
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_SUB_ID_EN, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_STREAM_ID_EXT_EN, 1 ) |
                BCHP_FIELD_DATA( XPT_PB0_CTRL4, PKTZ_PT_EN, 1 )
            );
            break;
       }
        Reg |= BCHP_FIELD_DATA( XPT_PB0_CTRL4, PACKETIZE_EN, 1 );
        BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL4, Reg ); 

        /* Disable CC checking, but enable CC generation in the full parser. */
        BXPT_GetPidChannel_CC_Config( hXpt, Cfg->ChannelNum, &ccConfig );
        ccConfig.Primary_CC_CheckEnable = false;
        ccConfig.Generate_CC_Enable = true;
        BXPT_SetPidChannel_CC_Config( hXpt, Cfg->ChannelNum, &ccConfig );
    }
    else
    {
        hPb->ForceResync = false;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
        /*NONE*/
#else
        RegAddr = BCHP_XPT_PB0_PKTZ_CONTEXT0 + ( 4 * Context );
        Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PKTZ_CONTEXT0 );
        Reg &= ~( 
            BCHP_MASK( XPT_PB0_PKTZ_CONTEXT0, PID ) |
            BCHP_MASK( XPT_PB0_PKTZ_CONTEXT0, CH_NUM ) |
            BCHP_MASK( XPT_PB0_PKTZ_CONTEXT0, PES_STREAM_ID ) 
            );
        BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_PKTZ_CONTEXT0, Reg ); 
#endif

        SidFilter.Mode = BXPT_Spid_eChannelFilterMode_Disable;
        BXPT_Spid_P_ConfigureChannelFilter(hXpt,Cfg->ChannelNum,SidFilter);

        BXPT_GetPidChannel_CC_Config( hXpt, Cfg->ChannelNum, &ccConfig );
        ccConfig.Primary_CC_CheckEnable = true;
        ccConfig.Generate_CC_Enable = false;
        BXPT_SetPidChannel_CC_Config( hXpt, Cfg->ChannelNum, &ccConfig );
        
        /* 
        ** SCheck the packetizer in each SPID/PID channel. If all the channels that are mapped to 
        ** this playback are Invalid, disable packetizing at the PB control reg. 
        */
       for( Index = 0; Index < BXPT_NUM_PID_CHANNELS; Index++ )
        {
        	unsigned int Band, Pid;
            bool IsPlayback;         

            BXPT_GetPidChannelConfig( hXpt, Index, &Pid, &Band, &IsPlayback );

            RegAddr = BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( 4 * Index );
            Reg = BREG_Read32( hXpt->hRegister, RegAddr );

            i = BCHP_GET_FIELD_DATA( Reg, XPT_FE_SPID_TABLE_i, SPID_MODE );

            /* if this pid channnel is used for spid functions or disabled */
            if((i >= BXPT_Spid_eChannelMode_Disable && i <= BXPT_Spid_eChannelMode_Remap)
            && Band == hPb->ChannelNo && IsPlayback )
                continue;
            else
                break;
        }

        if( Index == BXPT_NUM_PID_CHANNELS )
        {
            /* All contexts where invalid, so disable packetizer in the PB control reg. */
            Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL4 );
            Reg &= ~( 
                BCHP_MASK( XPT_PB0_CTRL4, PKTZ_CONTEXT_EN ) |
                BCHP_MASK( XPT_PB0_CTRL4, PACKETIZE_EN ) |
                BCHP_MASK( XPT_PB0_CTRL4, PKTZ_PT_EN )
                );
            BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL4, Reg ); 

            Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PARSER_CTRL1 );
            Reg &= ~( 
                BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_PACKET_TYPE ) |
                BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_PKT_LENGTH ) |
                BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD ) 
                );
            Reg |= (
                BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_PKT_LENGTH, 0xBC ) 
                );
            BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_PARSER_CTRL1, Reg ); 

            Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 );
            Reg &= ~( 
                BCHP_MASK( XPT_PB0_CTRL2, SYNC_EXT_MODE )
            );
            Reg |= (
                BCHP_FIELD_DATA( XPT_PB0_CTRL2, SYNC_EXT_MODE, BXPT_PB_SYNC_MPEG_BLIND )
            );
            BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL2, Reg ); 
        }
    }

    return( ExitCode );
}

 
BERR_Code BXPT_Playback_DisablePacketizers(
    BXPT_Playback_Handle hPb                    /* [in] Handle for the playback channel */
    )
{ 
    uint32_t Reg, RegAddr;
    unsigned Index;

    BERR_Code ExitCode = BERR_SUCCESS;

    {
        int32_t i;
        BXPT_Handle hXpt = (BXPT_Handle)hPb->vhXpt;

        for( Index = 0; Index < BXPT_NUM_PID_CHANNELS; Index++ )
        {
            /* if the pid channel input is this parser band cleanup the SPID table */
            RegAddr = BCHP_XPT_FE_PID_TABLE_i_ARRAY_BASE + ( 4 * Index ); 
            Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    
            if(!BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i,PLAYBACK_FE_SEL))
                continue;
            /* pb parser is the input */
            if(BCHP_GET_FIELD_DATA( Reg, XPT_FE_PID_TABLE_i,PLAYBACK_BAND_PARSER_PID_CHANNEL_INPUT_SELECT)!= hPb->ChannelNo)
                continue;
    
            RegAddr = BCHP_XPT_FE_SPID_TABLE_i_ARRAY_BASE + ( 4 * Index );
            Reg = BREG_Read32( hXpt->hRegister, RegAddr );
    
            i = BCHP_GET_FIELD_DATA( Reg, XPT_FE_SPID_TABLE_i, SPID_MODE );
    
            /* if this pid channnel is used for spid functions or disabled */
            if(i >= BXPT_Spid_eChannelMode_Disable && i <= BXPT_Spid_eChannelMode_Remap)
                    continue;
            else
            {
                Reg &= ~( 
                BCHP_MASK( XPT_FE_SPID_TABLE_i, SPID_MODE ) |
                BCHP_MASK( XPT_FE_SPID_TABLE_i, STREAM_ID_RANGE_STREAM_ID_HI ) |
                BCHP_MASK( XPT_FE_SPID_TABLE_i, STREAM_ID_RANGE_STREAM_ID_LO )
                );
    
                BREG_Write32( hXpt->hRegister, RegAddr, Reg );
            }
        }
    }

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
    /*NONE*/
#else
    RegAddr = BCHP_XPT_PB0_PKTZ_CONTEXT0 + ( 4 * Index );
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PKTZ_CONTEXT0 );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_PKTZ_CONTEXT0, PID ) |
        BCHP_MASK( XPT_PB0_PKTZ_CONTEXT0, CH_NUM ) |
        BCHP_MASK( XPT_PB0_PKTZ_CONTEXT0, PES_STREAM_ID ) 
        );
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_PKTZ_CONTEXT0, Reg ); 
#endif

    /* Disable packetizer in the PB control reg. */
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL4 );                         
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_CTRL4, PKTZ_CONTEXT_EN ) |
        BCHP_MASK( XPT_PB0_CTRL4, PACKETIZE_EN ) |
        BCHP_MASK( XPT_PB0_CTRL4, PKTZ_PT_EN )|
        BCHP_MASK( XPT_PB0_CTRL4, PKTZ_SUB_ID_EN )|
        BCHP_MASK( XPT_PB0_CTRL4, PKTZ_STREAM_ID_EXT_EN )
        );
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL4, Reg ); 

    /* These parser settings are always loaded when BXPT_Playback_PacketizeStream() is called. */
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PARSER_CTRL1 );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_PACKET_TYPE ) |
        BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_PKT_LENGTH ) |
        BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD ) 
        );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_PKT_LENGTH, 0xBC ) 
        );
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_PARSER_CTRL1, Reg );

    return ExitCode;
}

BERR_Code BXPT_Playback_GetParserConfig(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    BXPT_Playback_ParserConfig *ParserConfig    /* [out] The current settings */
    )
{
    uint32_t Reg;
    BXPT_Handle hXpt;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hPb );
    BDBG_ASSERT( ParserConfig );

    hXpt = ( BXPT_Handle ) hPb->vhXpt;

    /* The parser config registers are at consecutive addresses. */
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PARSER_CTRL1 );
    
    ParserConfig->ErrorInputIgnore = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_PARSER_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE );
    ParserConfig->AllPass = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_PARSER_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD );
    ParserConfig->AcceptNulls = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_PARSER_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD );
  
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL4 );
    ParserConfig->ForceRestamping = BCHP_GET_FIELD_DATA( Reg, XPT_PB0_CTRL4, PARSER_FORCE_RESTAMP );

#if BCHP_XPT_FULL_PID_PARSER_PBP_ACCEPT_ADAPT_00_PB0_ACCEPT_ADP_00_MASK != 0x00000001 || BXPT_NUM_PLAYBACKS > 32
    #error "PI NEEDS UPDATING"
#else
	Reg = BREG_Read32( hPb->hRegister, BCHP_XPT_FULL_PID_PARSER_PBP_ACCEPT_ADAPT_00 );
	ParserConfig->AcceptAdapt00 = (Reg >> hPb->ChannelNo) & 0x01 ? true : false;
#endif

    return( ExitCode );
}

BERR_Code BXPT_Playback_SetParserConfig(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    const BXPT_Playback_ParserConfig *ParserConfig  /* [in] The new settings */
    )
{
    uint32_t Reg;
    BXPT_Handle hXpt;

    BERR_Code ExitCode = BERR_SUCCESS;

    BDBG_ASSERT( hPb );
    BDBG_ASSERT( ParserConfig );

    hXpt = ( BXPT_Handle ) hPb->vhXpt;

    /* The parser config registers are at consecutive addresses. */
    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PARSER_CTRL1 );
    
    /* Clear all the bits we are about to change. */
    Reg &= ~(
        BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE ) |
        BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD ) |
        BCHP_MASK( XPT_PB0_PARSER_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD ) 
    );

    /* Now set the new values. */
    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_ERROR_INPUT_TEI_IGNORE, ParserConfig->ErrorInputIgnore ) |
        BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_ALL_PASS_CTRL_PRE_MPOD, ParserConfig->AllPass == true ? 1 : 0 ) |
        BCHP_FIELD_DATA( XPT_PB0_PARSER_CTRL1, PARSER_ACCEPT_NULL_PKT_PRE_MPOD, ParserConfig->AcceptNulls ) 
    );

    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_PARSER_CTRL1, Reg ); 

    if( ParserConfig->AllPass && false == hPb->IsParserInAllPass )
    {
        BXPT_PidChannel_CC_Config AllCcDisabled = { false, false, false, 0 };

        /* Remember the config before entering all-pass. */
        BXPT_GetPidChannel_CC_Config( hXpt, BXPT_GET_PLAYBACK_ALLPASS_CHNL(hPb->ChannelNo) , &hPb->CcConfigBeforeAllPass );

        /* Now disable the CC checks */
        BXPT_SetPidChannel_CC_Config( hXpt, BXPT_GET_PLAYBACK_ALLPASS_CHNL(hPb->ChannelNo), &AllCcDisabled );
        hPb->IsParserInAllPass = true;
    }
    else if( false == ParserConfig->AllPass ) 
    {
        /* Restore the config we had before entering all-pass. */
        BXPT_SetPidChannel_CC_Config( hXpt, BXPT_GET_PLAYBACK_ALLPASS_CHNL(hPb->ChannelNo), &hPb->CcConfigBeforeAllPass );
        hPb->IsParserInAllPass = false;
    }

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL4 );
    Reg &= ~( BCHP_MASK( XPT_PB0_CTRL4, PARSER_FORCE_RESTAMP ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_PB0_CTRL4, PARSER_FORCE_RESTAMP, ParserConfig->ForceRestamping ) );
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL4, Reg ); 

#if BCHP_XPT_FULL_PID_PARSER_PBP_ACCEPT_ADAPT_00_PB0_ACCEPT_ADP_00_MASK != 0x00000001 || BXPT_NUM_PLAYBACKS > 32
    #error "PI NEEDS UPDATING"
#else
		Reg = BREG_Read32( hXpt->hRegister, BCHP_XPT_FULL_PID_PARSER_PBP_ACCEPT_ADAPT_00 );
		Reg &= ~(0x01 << hPb->ChannelNo);
        if( ParserConfig->AcceptAdapt00 ) 
            Reg |= (0x01 << hPb->ChannelNo);
		BREG_Write32( hXpt->hRegister, BCHP_XPT_FULL_PID_PARSER_PBP_ACCEPT_ADAPT_00, Reg );
#endif

    return( ExitCode );
}

void BXPT_Playback_P_WriteReg(
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    uint32_t Pb0RegAddr,
    uint32_t RegVal
    )
{
    /* 
    ** The address is the offset of the register from the beginning of the 
    ** block, plus the base address of the block ( which changes from 
    ** channel to channel ). 
    */
    uint32_t RegAddr = Pb0RegAddr - BCHP_XPT_PB0_CTRL1 + PlaybackHandle->BaseAddr;

    BREG_Write32( PlaybackHandle->hRegister, RegAddr, RegVal );
}       


uint32_t BXPT_Playback_P_ReadReg(
    BXPT_Playback_Handle PlaybackHandle,    /* [in] Handle for the playback channel */
    uint32_t Pb0RegAddr
    )
{
    /* 
    ** The address is the offset of the register from the beginning of the 
    ** block, plus the base address of the block ( which changes from 
    ** channel to channel ). 
    */
    uint32_t RegAddr = Pb0RegAddr - BCHP_XPT_PB0_CTRL1 + PlaybackHandle->BaseAddr;

    return( BREG_Read32( PlaybackHandle->hRegister, RegAddr ));
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
    uint32_t BufferPhysicalAddr;
    uint32_t ThisDescPhysicalAddr;
    BXPT_PvrDescriptor *CachedDescPtr;

    BERR_Code ExitCode = BERR_SUCCESS;
    BMEM_Handle hHeap = hXpt->hPbHeap ? hXpt->hPbHeap : hXpt->hMemory;

    BDBG_ASSERT( hXpt );
    BDBG_ASSERT( Desc );
    BDBG_ASSERT( Buffer );

    BMEM_ConvertAddressToOffset( hHeap, ( void * ) Buffer, &BufferPhysicalAddr );

    /* Verify that the descriptor we're creating sits on a 16-byte boundary. */
    BMEM_ConvertAddressToOffset( hHeap, ( void * ) Desc, &ThisDescPhysicalAddr );
    if( ThisDescPhysicalAddr % 16 )
    {
        BDBG_ERR(( "Desc is not 16-byte aligned!" ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
    }

    BMEM_ConvertAddressToCached(hHeap, (void *) Desc, (void **) &CachedDescPtr);

    /* Load the descriptor's buffer address, length, and flags. */
    CachedDescPtr->BufferStartAddr = BufferPhysicalAddr;
    CachedDescPtr->BufferLength = BufferLength;

    /* Clear everything, then set the ones we want below. */
    CachedDescPtr->Flags = 0;                                                                       

    if( IntEnable )
        CachedDescPtr->Flags |= TRANS_DESC_INT_FLAG;

    if( ReSync )
        CachedDescPtr->Flags |= TRANS_DESC_FORCE_RESYNC_FLAG;

    /* Load the pointer to the next descriptor in the chain, if there is one. */
    if( NextDesc != 0 )
    {
        /* There is a another descriptor in the chain after this one. */
        uint32_t NextDescPhysAddr;

        BMEM_ConvertAddressToOffset( hHeap, ( void * ) NextDesc, &NextDescPhysAddr );
        if( NextDescPhysAddr % 16 )
        {
            BDBG_ERR(( "NextDescDesc is not 32-bit aligned!" ));
            ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        }

        /* Next descriptor address must be 16-byte aligned. */
        NextDescPhysAddr &= ~( 0xF );
        CachedDescPtr->NextDescAddr = NextDescPhysAddr;
    }
    else                  
    {
        /* There is NOT another descriptor. Set the Last Descriptor bit. */
        CachedDescPtr->NextDescAddr = TRANS_DESC_LAST_DESCR_IND;
    }

    BMEM_FlushCache(hHeap, CachedDescPtr, sizeof (BXPT_PvrDescriptor) );
    return( ExitCode );
}

#ifdef ENABLE_PLAYBACK_MUX
void BXPT_Playback_SetDescBuf( 
	BXPT_Handle hXpt, 	   					/* [in] Handle for this transport */
	BXPT_PvrDescriptor * const Desc,		/* [in] Descriptor to initialize */  
	uint8_t *Buffer, 						/* [in] Data buffer. */
	uint32_t BufferLength 					/* [in] Size of buffer (in bytes). */
	)
{
    BXPT_PvrDescriptor *CachedDescPtr;
    BMEM_Handle hHeap = hXpt->hPbHeap ? hXpt->hPbHeap : hXpt->hMemory;

    BMEM_ConvertAddressToCached(hHeap, (void *) Desc, (void **) &CachedDescPtr);
	BMEM_ConvertAddressToOffset(hHeap, ( void * ) Buffer, &(CachedDescPtr->BufferStartAddr));
	CachedDescPtr->BufferLength = BufferLength;
    BMEM_FlushCache(hHeap, CachedDescPtr, sizeof (BXPT_PvrDescriptor) );
}
#endif /*ENABLE_PLAYBACK_MUX*/

BINT_Id BXPT_Playback_GetIntId(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    BXPT_Playback_Int PbInt
    )
{
    uint32_t RegAddr; 

    BDBG_ASSERT( hPb );
    
    RegAddr = BCHP_XPT_PB0_INTR - BCHP_XPT_PB0_CTRL1 + hPb->BaseAddr;
    return BCHP_INT_ID_CREATE( RegAddr, PbInt );
}

#if BXPT_HAS_TSMUX

void BXPT_Playback_GetMuxingInfo(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    BXPT_Playback_MuxingInfo *Info
    )
{
    BDBG_ASSERT( hPb );
    BDBG_ASSERT( Info );

    Info->uiPacingCounter = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PACING_COUNTER );
}

BERR_Code BXPT_Playback_P_SetPacingSpeed( 
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    unsigned Speed
    )
{
    uint32_t Reg;

    unsigned PacingSpeedBitField = 0;
    BERR_Code ExitCode = BERR_SUCCESS;
                                                               
    BDBG_ASSERT( hPb );

    switch( Speed )
    {
        case 1: PacingSpeedBitField = 0; break;
        case 2: PacingSpeedBitField = 1; break;
        case 4: PacingSpeedBitField = 2; break;
        case 8: PacingSpeedBitField = 3; break;
        case 16: PacingSpeedBitField = 4; break;
        case 32: PacingSpeedBitField = 5; break;
        case 64: PacingSpeedBitField = 6; break;
        case 128: PacingSpeedBitField = 7; break;

        default: 
        BDBG_ERR(( "Invalid pacing speed %u", Speed ));
        ExitCode = BERR_TRACE( BERR_INVALID_PARAMETER );
        goto Done;
        break;
    }

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_CTRL2 );
    Reg &= ~( BCHP_MASK( XPT_PB0_CTRL2, PACING_SPEED ) );
    Reg |= ( BCHP_FIELD_DATA( XPT_PB0_CTRL2, PACING_SPEED, PacingSpeedBitField ) );
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_CTRL2, Reg );

    Done:
    return ExitCode;
}

void BXPT_Playback_P_SetPacingCount(
    BXPT_Playback_Handle hPb,                   /* [in] Handle for the playback channel */
    unsigned PacingLoadMap,
    unsigned PacingCount
    )
{
    hPb->PacingLoadMap = PacingLoadMap;
    hPb->PacingCount = PacingCount;
    hPb->PacingLoadNeeded = true;
}

#endif

bool BXPT_Playback_IsAvailable(
    BXPT_Handle hXpt,                           /* [in] Handle for this transport */
    unsigned int ChannelNo                      /* [in] Which channel to check. */
    )
{
    if( ChannelNo >= BXPT_NUM_PLAYBACKS )
    {
        return false;
    }
    else if( hXpt->PlaybackHandles[ ChannelNo ].Opened ) 
    {
        return false;
    }
#if (BCHP_CHIP == 7425) || (BCHP_CHIP == 7346)
    else if( 13 == ChannelNo || 14 == ChannelNo ) 
    {
        /* SW7425-3701. */
        return false;
    }
#endif

    return true;
}


#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
void BXPT_Playback_InitDescriptorFlags(
    BXPT_PvrDescriptorFlags *flags
    )
{
    BDBG_ASSERT( flags );
    BKNI_Memset((void *)flags, 0, sizeof( BXPT_PvrDescriptorFlags ));
}

#if BXPT_HAS_TSMUX
void BXPT_Playback_SetDescriptorFlags(
    BXPT_Playback_Handle hPb,   /* [in] Handle for the playback channel */
    BXPT_PvrDescriptor *Desc, 
    const BXPT_PvrDescriptorFlags *flags
    )
{
    BXPT_PvrDescriptor *CachedDesc = NULL;

    BDBG_ASSERT( Desc );
    BDBG_ASSERT( flags );

    if( BMEM_Heap_ConvertAddressToCached( hPb->hMemory, (void *) Desc, (void ** ) &CachedDesc ) != BERR_SUCCESS )
    {
        BDBG_ERR(( "Conversion of uncached Desc 0x%08lX to cached failed", (unsigned long) Desc ));
        goto Done;
    }
    BMEM_FlushCache( hPb->hMemory, CachedDesc, sizeof (*CachedDesc) );

    CachedDesc->Reserved0 = 0;
    CachedDesc->MuxingFlags = flags->muxFlags.bRandomAccessIndication ? 1 << 2 : 0;	 
    if( flags->muxFlags.bNextPacketPacingTimestampValid )
    {
        CachedDesc->MuxingFlags |= 1 << 1;	 
        CachedDesc->NextPacketPacingTimestamp = flags->muxFlags.uiNextPacketPacingTimestamp;
    }
    if( flags->muxFlags.bPacket2PacketTimestampDeltaValid )
    {
        CachedDesc->MuxingFlags |= 1;	 
        CachedDesc->Pkt2PktPacingTimestampDelta = flags->muxFlags.uiPacket2PacketTimestampDelta;
    }
    BMEM_FlushCache( hPb->hMemory, CachedDesc, sizeof (*CachedDesc) );

    Done:
    return;
}
#endif

unsigned BXPT_Playback_P_GetBandId( 
    BXPT_Playback_Handle hPb 
    )
{
    return (unsigned) BCHP_GET_FIELD_DATA( BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PLAYBACK_PARSER_BAND_ID ), 
        XPT_PB0_PLAYBACK_PARSER_BAND_ID, PB_PARSER_BAND_ID );
}

unsigned int BXPT_PB_P_GetPbBandId( 
    BXPT_Handle hXpt,
    unsigned int Band 
    )
{
    uint32_t RegAddr = BCHP_XPT_PB0_PLAYBACK_PARSER_BAND_ID + Band * (BCHP_XPT_PB1_CTRL1 - BCHP_XPT_PB0_CTRL1);
    return (unsigned) BCHP_GET_FIELD_DATA( BREG_Read32( hXpt->hRegister, RegAddr ), 
        XPT_PB0_PLAYBACK_PARSER_BAND_ID, PB_PARSER_BAND_ID );
}

void BXPT_Playback_P_SetBandId( 
    BXPT_Playback_Handle hPb, 
    unsigned NewBandId 
    )
{
    uint32_t Reg;

    Reg = BXPT_Playback_P_ReadReg( hPb, BCHP_XPT_PB0_PLAYBACK_PARSER_BAND_ID );
    Reg &= ~( 
        BCHP_MASK( XPT_PB0_PLAYBACK_PARSER_BAND_ID, PB_PARSER_SEL ) |
        BCHP_MASK( XPT_PB0_PLAYBACK_PARSER_BAND_ID, PB_PARSER_BAND_ID ) 
        );
    Reg |= (
        BCHP_FIELD_DATA( XPT_PB0_PLAYBACK_PARSER_BAND_ID, PB_PARSER_SEL, 1 ) |
        BCHP_FIELD_DATA( XPT_PB0_PLAYBACK_PARSER_BAND_ID, PB_PARSER_BAND_ID, NewBandId )
    );  
    BXPT_Playback_P_WriteReg( hPb, BCHP_XPT_PB0_PLAYBACK_PARSER_BAND_ID, Reg );
}

#if 0
void BXPT_Playback_PvrDescriptorAdvDefault(
    BXPT_PvrDescriptorAdv *desc
    )
{
    BDBG_ASSERT( desc );
    BKNI_Memset((void *)desc, 0, sizeof( BXPT_PvrDescriptorAdv ));
}
#endif
#endif


