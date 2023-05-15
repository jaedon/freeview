/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxvd.c $
 * $brcm_Revision: Hydra_Software_Devel/539 $
 * $brcm_Date: 10/10/12 10:56a $
 *
 * Module Description:
 *   See code
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xvd/src/bxvd.c $
 * 
 * Hydra_Software_Devel/539   10/10/12 10:56a rayshap
 * SW7425-4058: Remove BERR_TRACE from GetNextPTS
 * 
 * Hydra_Software_Devel/538   9/25/12 3:15p davidp
 * SW7425-3903: Add bUserDataBTPModeEnable to BXVD_DecodeSettings to
 * enable userdata processing in Broadcom Trick Play mode.
 * 
 * Hydra_Software_Devel/537   9/21/12 4:04p davidp
 * SW7346-963: Access FW picture queues using cached memory space, fix
 * still picture release issue.
 * 
 * Hydra_Software_Devel/536   8/20/12 9:12a btosi
 * SW7405-4736: added wrappers for queue macros to aid with debug
 * 
 * Hydra_Software_Devel/535   8/2/12 2:59p btosi
 * SW7405-4736: change the XDM uiInstanceId format to
 * uDecoderInstance.uiVDCRectangleNum
 * 
 * Hydra_Software_Devel/534   8/2/12 10:31a btosi
 * SW7425-1064: support for flushing a held picture on the enhanced
 * channel at StartDecode
 * 
 * Hydra_Software_Devel/533   7/31/12 1:29p btosi
 * SW7425-1064: don't call BXVD_Status_... for the enhanced channel
 * 
 * Hydra_Software_Devel/532   7/27/12 11:29a btosi
 * SW7425-1064: tweaks for XMO
 * 
 * Hydra_Software_Devel/531   7/25/12 9:47a btosi
 * SW7425-3558: added BXDM_PictureProvider_GetCurrentPicturePtr_isr
 * 
 * Hydra_Software_Devel/530   7/19/12 4:14p davidp
 * SW7425-3316: Add BXVD_GetHardwareCapabilities API to determine video
 * decoder capabilities at runtime.
 * 
 * Hydra_Software_Devel/529   7/6/12 12:04p btosi
 * SW7425-3358: added support for FNRT
 * 
 * Hydra_Software_Devel/528   7/3/12 10:20a btosi
 * SW7425-1064: commented out code for now to get around converity issue
 * 
 * Hydra_Software_Devel/527   6/25/12 3:26p btosi
 * SW7425-1064: merged XMO changes from branch
 * 
 * Hydra_Software_Devel/526   6/25/12 2:40p btosi
 * SW7425-1064: merged XMO changes from branch
 * 
 * Hydra_Software_Devel/SW7425-1064/6   6/20/12 10:07a btosi
 * SW7425-1064: merging latest mainline changes
 * 
 * Hydra_Software_Devel/525   6/18/12 5:33p davidp
 * SW7435-164: Remove unneeded include file.
 * 
 * Hydra_Software_Devel/524   6/14/12 4:02p btosi
 * SW7425-3177: changed when the BAVC_VideoCompressionStd enum is mapped
 * to a BXVD_P_PPB_Protocol value
 * 
 * Hydra_Software_Devel/523   6/13/12 11:15a btosi
 * SW7425-3177: added support frame rates e10,e15,e20.  Reintroduced
 * BXVD_P_PPB_Protocol since the AVD and bavc.h protocol enums have
 * diverged.
 * 
 * Hydra_Software_Devel/522   6/5/12 5:46p davidp
 * SW7425-3172: Add test to cassert if crc element in public PPB struct is
 * not at same offset as private PPB crc element.
 * 
 * Hydra_Software_Devel/521   6/1/12 2:20p btosi
 * SW7231-788: divide the Delivery Queue depth by 2 for protocols that
 * deliver pairs of pictures
 * 
 * Hydra_Software_Devel/520   5/30/12 5:18p davidp
 * SW7425-3028: Add VP8 decode still picture support.
 * 
 * Hydra_Software_Devel/519   5/7/12 6:23p davidp
 * SW7405-5655: If FW Dram logging previously started, restart during
 * watchdog processing.
 * 
 * Hydra_Software_Devel/518   4/19/12 5:21p davidp
 * SW7346-761: Create channel context free list to keep channel context
 * for reuse.
 * 
 * Hydra_Software_Devel/517   4/5/12 5:56p davidp
 * SW7344-306: Printout AVD HW revision in BXVD_Open.
 * 
 * Hydra_Software_Devel/516   3/14/12 3:22p davidp
 * SW7435-8: Add more detailed error handling output.
 * 
 * Hydra_Software_Devel/515   2/27/12 5:14p davidp
 * SW7425-1680: Add bEarlyPictureDeliveryMode to BXVD_DecodeSettings, set
 * appropriate bit in FW channel start command.
 * 
 * Hydra_Software_Devel/514   2/22/12 3:46p btosi
 * SW7405-4736: OR the decoder instance into the XDM instance ID
 * 
 * Hydra_Software_Devel/513   2/16/12 6:14p davidp
 * SW7425-2419: Destroy interrupts before chip is reset.
 * 
 * Hydra_Software_Devel/512   2/10/12 1:17p davidp
 * SW7425-2284: Save and use init channel return code.
 * 
 * Hydra_Software_Devel/511   2/10/12 9:46a davidp
 * SW7425-2284: Fix compiler warning.
 * 
 * Hydra_Software_Devel/510   2/9/12 7:56p davidp
 * SW7425-2284: Cleanup numerous calls to Enter/LeaveCriticalSection.
 * 
 * Hydra_Software_Devel/509   2/2/12 3:02p davidp
 * SW7425-2024: Validate decode protocol specified in BXVD_StartDecode is
 * supported on platform.
 * 
 * Hydra_Software_Devel/508   1/20/12 3:46p davidp
 * SW7208-175: Add support for 1920 portrait decode.
 * 
 * Hydra_Software_Devel/507   12/15/11 5:32p davidp
 * SW7405-3984: Disable picture provider when channel change mode is set
 * to eMute.
 * 
 * Hydra_Software_Devel/506   12/15/11 9:41a btosi
 * SW7405-3984: don't try to start an external PictureProvider
 * 
 * Hydra_Software_Devel/505   12/14/11 1:55p btosi
 * SW7405-3984: added
 * BXDM_DisplayInterruptHandler_(Set/Get)PictureProviderMode_isr
 * 
 * Hydra_Software_Devel/504   11/10/11 2:37p davidp
 * SW7425-1599: Add bSVCProgressiveOnly flag to BXVD_ChannelSettings.
 * 
 * Hydra_Software_Devel/503   10/20/11 3:00p davidp
 * SW7425-1564: Support for SVC interlaced decode.
 * 
 * Hydra_Software_Devel/502   10/3/11 2:43p btosi
 * SW7425-1264: support for a SW STC internal to XDM
 * 
 * Hydra_Software_Devel/501   9/30/11 3:36p rayshap
 * SW7340-279: Fix coverity false positve
 * 
 * Hydra_Software_Devel/500   9/27/11 6:40p davidp
 * SW7340-279: Allocate Still Channel context in BXVD_Open.
 * 
 * Hydra_Software_Devel/499   8/4/11 6:09p davidp
 * SW7231-127: Halt ARCS then reset decoder in BXVD_Standby().
 * 
 * Hydra_Software_Devel/498   7/28/11 5:50p davidp
 * SW7231-127: Power off the decoder in BXVD_Close.
 * 
 * Hydra_Software_Devel/497   7/27/11 2:02p btosi
 * SWDTV-7795: added new protocols to the debug messages
 * 
 * Hydra_Software_Devel/496   7/26/11 1:57p davidp
 * SW7405-5424: Add bIgnoreNumReorderFramesEqZero to BXVD_DecodeSettings
 * to workaround issue with certain encoded AVC streams.
 * 
 * Hydra_Software_Devel/495   7/20/11 3:27p davidp
 * SW7231-127: Add BXVD_Standby and BXVD_Resume support.
 * 
 * Hydra_Software_Devel/494   6/30/11 4:06p davidp
 * SW7425-826: Clear vsync interrupt when vsync interrupt mask is
 * registered with XVD.
 * 
 * Hydra_Software_Devel/493   6/28/11 2:28p davidp
 * SW7425-542: Fix deadcode coverity STC_MAX = 2 issue.
 * 
 * Hydra_Software_Devel/492   6/27/11 4:13p davidp
 * SW7425-542: Merge devel branch to mainline.
 *
 * Hydra_Software_Devel/SW7425-542/4   6/9/11 1:19p btosi
 * SW7425-542: added in support for the Host Memory Interface
 * 
 * Hydra_Software_Devel/SW7425-542/3   6/8/11 7:07p davidp
 * SW7425-542: Add Display Info macros.
 * 
 * Hydra_Software_Devel/SW7425-542/2   6/7/11 5:42p davidp
 * SW7425-542: Add support for 7 STCs.
 * 
 * Hydra_Software_Devel/SW7425-542/1   5/31/11 4:08p davidp
 * SW7425-542: Add support for Host Interface Memory (ARC Scratch memory)
 * FW API changes.
 * 
 * Hydra_Software_Devel/491   6/16/11 4:28p davidp
 * SW7425-615: Move GISB ERR init code to platform chip-enable routine.
 * 
 * Hydra_Software_Devel/490   6/16/11 11:21a btosi
 * SW7425-615: added retry mechanism for register reads that fail
 * 
 * Hydra_Software_Devel/489   6/14/11 9:06a btosi
 * SWDTV-7262: in BXVD_GetChannelStatus_isr, include the number of
 * pictures on the Delivery Queue in uiPictureDeliveryCount
 * 
 * Hydra_Software_Devel/488   6/8/11 1:03p btosi
 * SW7425-542: moved the delivery/release queue management macros to the
 * platform files
 * 
 * Hydra_Software_Devel/487   5/25/11 4:34p davidp
 * SWDTV-7270: BXVD_DecodeStillPictureReset was not determining still
 * channel handle correctly.
 * 
 * Hydra_Software_Devel/486   3/25/11 3:56p davidp
 * SW7405-5101: Add checks to verify AVD code space, CDB/ITB and memory
 * used is in memory region below 768mb for ARC300 based cores.
 * 
 * Hydra_Software_Devel/485   3/21/11 4:48p davidp
 * SWBLURAY-23744: Add SW Coef AVC Decode channel mode to
 * BXVD_DecodeSettings and FW ChannelStart command.
 * 
 * Hydra_Software_Devel/484   2/25/11 10:20a davidp
 * SW7422-185: Add support for 3D SVC decode in BXVD_StartDecode.
 * 
 * Hydra_Software_Devel/483   2/23/11 4:06p davidp
 * SW7422-183: Restore extended XPT context in flush and
 * SetSkipPictureModeDecode routines.
 * 
 * Hydra_Software_Devel/482   2/23/11 2:55p pblanco
 * SW7422-22: Change logical and to bitwise and in
 * BXVD_InstallDeviceInterruptCallback in base layer ARC present test.
 * 
 * Hydra_Software_Devel/481   2/15/11 5:32p davidp
 * SW7422-22: Add support for AVD Boot callback and AVD Reset Callback.
 * 
 * Hydra_Software_Devel/480   1/11/11 8:08p davidp
 * SW7400-2992: Add BXVD_GetChannelMemoryConfig, used to determine channel
 * memory heap sizes.
 * 
 * Hydra_Software_Devel/479   1/3/11 11:17a btosi
 * SW7422-72: don't set the 3D mode in BXVD_StopDecode
 * 
 * Hydra_Software_Devel/478   12/7/10 7:09p davidp
 * SW7408-191: Enable dynamic power management on 7408 platform.
 * 
 * Hydra_Software_Devel/477   11/24/10 1:22p btosi
 * SW7422-72: added BXVD_LEGACY_MVC_SUPPORT and bSetNextPointer for MVC on
 * older DVD chips
 * 
 * Hydra_Software_Devel/476   11/23/10 6:12p davidp
 * SW7422-22: Fix SVC support.
 * 
 * Hydra_Software_Devel/475   11/17/10 9:37a btosi
 * SW7422-72: added new BXVD and XDM 3D API's
 * 
 * Hydra_Software_Devel/474   10/27/10 5:34p davidp
 * SW7422-22: Add VICH IL and BL interrupt support, add external boot
 * callback support.
 * 
 * Hydra_Software_Devel/473   10/21/10 6:21p davidp
 * SWBLURAY-23012: Add NULL handle test in BXVD_GetMonitorRefreshRate_isr.
 * 
 * Hydra_Software_Devel/472   10/20/10 2:14p davidp
 * SW7425-16: Remove FGT support.
 * 
 * Hydra_Software_Devel/471   10/15/10 6:35p davidp
 * SW7425-16: Add support for base layer ARC, new memory config,
 * additional FW api memory arguments.
 * 
 * Hydra_Software_Devel/470   10/5/10 10:24a davidp
 * SW7405-4923: Resolve latest coverity run issues for non-debug builds.
 * 
 * Hydra_Software_Devel/469   10/1/10 2:52p btosi
 * SWDEPRECATED-1003: added support for bTreatAsSingleElement
 * 
 * Hydra_Software_Devel/468   9/27/10 12:31p davidp
 * SW3548-3033: Add bAVCAspectRatioOverrideMode to BXVD_DecodeSettings.
 * 
 * Hydra_Software_Devel/467   9/23/10 3:09p btosi
 * SW7405-4736: add support for a XDM instance ID to help debug multi-
 * channel issues
 * 
 * Hydra_Software_Devel/466   9/15/10 1:56p btosi
 * SWDEPRECATED-1003: conditionally reset the frame rate override in
 * BXVD_StopDecode
 * 
 * Hydra_Software_Devel/465   9/15/10 12:06p btosi
 * SWDEPRECATED-1003: added BXVD_SetFrameRateOverride
 * 
 * Hydra_Software_Devel/464   9/14/10 11:56a btosi
 * SWDEPRECATED-1003: added BXDM_PictureProvider_SetTrickMode_isr to
 * primarily disable the FIC logic when receiving video with sparse
 * pictures
 * 
 * Hydra_Software_Devel/463   9/9/10 5:27p davidp
 * SW7420-972: Only include bchp_pwr.h on platforms that support bchp_pwr.
 * 
 * Hydra_Software_Devel/462   9/9/10 2:59p davidp
 * SW7420-972: Use BCHP_PWR routines for dynamic power management in XVD.
 * 
 * Hydra_Software_Devel/461   8/20/10 1:47p davidp
 * SWBLURAY-22113: Change name to bExternalPictureProviderMode.
 * 
 * Hydra_Software_Devel/460   8/19/10 5:57p davidp
 * SWBLURAY-22113: Add BXVD_DecodeSettings field,
 * bExternalPictureProviderInUse flag to support DVD app.
 * 
 * Hydra_Software_Devel/459   8/9/10 2:16p btosi
 * SW7405-4733: reset bPauseActive in BXVD_StopDecode if NOT preserving
 * state
 * 
 * Hydra_Software_Devel/458   8/6/10 5:47p btosi
 * SW7340-199: reset the "DM" state cached in the channel structure.
 * 
 * Hydra_Software_Devel/457   8/5/10 8:34a delkert
 * SW7405-4703: Added BXVD_Set/GetHorizontalOverscanMode and
 * BXVD_Set/GetHorizontalOverscanMode APIs to allow bypass of horizontal
 * overscan calcs.
 * 
 * Hydra_Software_Devel/456   8/3/10 2:03p btosi
 * SW7400-2870: added BXVD_[Set/Get]PlaybackRate(_isr)
 * 
 * Hydra_Software_Devel/455   7/30/10 2:10p pblanco
 * SW7400-2857: Backed out memory reduction changes.
 * 
 * Hydra_Software_Devel/453   7/14/10 3:07p davidp
 * SW7405-4429: Remove channel BXVD_Interrupt_ePictureDataReady1 interrupt
 * callback.
 * 
 * Hydra_Software_Devel/452   7/13/10 4:25p davidp
 * SW7405-4628: Remove seq header callback support, add stereo seq error
 * callback support.
 * 
 * Hydra_Software_Devel/451   5/24/10 3:53p btosi
 * SW7405-4378: Return the unaltered source width and height in the
 * BXVD_PictureParameterInfo structure.
 * 
 * Hydra_Software_Devel/450   5/18/10 4:13p davidp
 * SW7405-4358: Allow zero delay output mode for MPEG2 streams as well as
 * AVC.
 * 
 * Hydra_Software_Devel/449   5/11/10 1:37p btosi
 * SW7405-4117: added usage note
 * 
 * Hydra_Software_Devel/448   4/29/10 11:20a btosi
 * SW7405-4117: added support for ...DisplayFieldMode_eAuto
 * 
 * Hydra_Software_Devel/447   4/27/10 4:09p davidp
 * SW7400-2747: Add bPFrameSkipDisable field to decode settings to disable
 * P frame skip mode.
 * 
 * Hydra_Software_Devel/446   3/25/10 4:39p rayshap
 * SW7401-4383: Fix compile issue with avc.h
 * 
 * Hydra_Software_Devel/445   3/24/10 2:59p davidp
 * SW7405-3137: Save and restore extended CDB read register address for
 * decode restart cases.
 * 
 * Hydra_Software_Devel/444   3/3/10 5:25p davidp
 * SW7400-2704: Add uiSEIMessagesFlags to BXVD_DecodeSettings.
 * 
 * Hydra_Software_Devel/443   3/1/10 4:58p nilesh
 * SW7405-2993: Fix coverity warnings
 * 
 * Hydra_Software_Devel/442   2/25/10 4:24p nilesh
 * SW7405-2993: XDM Merge
 * 
 * Hydra_Software_Devel/441   2/25/10 1:52p btosi
 * SW3556-1058: added bIgnoreDPBOutputDelaySyntax to the decode settings
 * 
 * Hydra_Software_Devel/440   2/17/10 2:50p davidp
 * SW7468-74: Add support for IP licensing, save chip product revision
 * register address in xvd handle.
 * 
 * Hydra_Software_Devel/439   1/8/10 11:50a btosi
 * SW7405-3137: added support for extension data callback
 *
 * Hydra_Software_Devel/438   1/6/10 2:28p btosi
 * SW7405-3137: merged branch into mainline
 * 
 * Hydra_Software_Devel/437   12/16/09 4:14p btosi
 * SW7601-180: added BXVD_MPEGPulldownOverride routines/logic to
 * conditionally override TB->TBT and BT->BTB
 * 
 * Hydra_Software_Devel/436   12/14/09 11:27a nilesh
 * SW7405-3358: Added Jitter Tolerance Improvement (JTI) logic to prevent
 * cadence issues caused when deltaStcPts is near 0 and STC snapshot is
 * jittery
 * 
 * Hydra_Software_Devel/435   12/11/09 1:01p davidp
 * SW7405-3572: Clear Channelhandle pointer in ChannelHandle array before
 * sending FW ChannelClose command to prevent DM from accessing data
 * after channel is closed.
 * 
 * Hydra_Software_Devel/434   12/8/09 3:26p btosi
 * SW7405-3257: removed BTMR_StopTimer() from BXVD_Close()
 * 
 * Hydra_Software_Devel/433   11/30/09 4:38p btosi
 * SW7405-3245: added BXVD_DBG_* macros.  Map to either BDBG_INSTANCE_* or
 * BDBG_*
 * at compile.
 * 
 * Hydra_Software_Devel/432   11/24/09 10:22a btosi
 * SW7405-3257: modified code to use a shared timer,
 * BTMR_Type_eSharedFreeRun
 * 
 * Hydra_Software_Devel/431   11/23/09 5:17p davidp
 * SW7335-616: Add bIonlyFieldOutputMode to BXVD_DecodeSettings.
 * 
 * Hydra_Software_Devel/430   11/19/09 5:45p davidp
 * SW7601-173: Add bAVCErrorConcealmentMode to decode_settings structure,
 * pass to FW if enabled.
 * 
 * Hydra_Software_Devel/429   11/5/09 4:10p davidp
 * SW7405-3053: Add support to use IFrame as RAP indicator for AVC in
 * BXVD_StartDecode settings.
 * 
 * Hydra_Software_Devel/428   11/4/09 3:21p davidp
 * SW7405-3323: All FW commands called now return status back to
 * application.
 * 
 * Hydra_Software_Devel/427   10/13/09 11:21a btosi
 * SW7405-3005: added "uiDecoderInputOverflow" i.e. reporting of CDB
 * overflow
 * 
 * Hydra_Software_Devel/426   10/9/09 10:35a nilesh
 * SW7405-3085: Added a way to disable forced progressive scanout of
 * 240i/288i content
 *
 * Hydra_Software_Devel/425   10/5/09 12:26p davidp
 * SW7405-2963: Re-enable FW debug logging in BXVD_ProcessWatchdog.
 *
 * Hydra_Software_Devel/424   10/2/09 4:06p davidp
 * SW7405-3049: Change default timestamp mode to decoder order.
 *
 * Hydra_Software_Devel/423   9/30/09 3:14p nilesh
 * SW7405-3085: Target source polarity synchronization now only occurs if
 * there are multiple active decodes targeted for the same display
 * interrupt.
 *
 * Hydra_Software_Devel/422   9/29/09 4:05p davidp
 * SW7405-3049: Add support for AVC in AVI container files. Add new
 * eTimestampMode to DecodeSettings.
 *
 * Hydra_Software_Devel/421   9/24/09 10:43a nilesh
 * SW3548-2418: PTS Jitter Correction
 *
 * Hydra_Software_Devel/420   9/1/09 4:47p davidp
 * SW7405-2963: Add support to printout AVD FW log in PDR_isr.
 *
 * Hydra_Software_Devel/419   8/28/09 3:07p btosi
 * SW7405-2940: added BXVD_GetErrorHandlingMode
 *
 * Hydra_Software_Devel/418   8/27/09 1:40p btosi
 * SW7405-2940: added BXVD_SetErrorHandlingMode
 *
 * Hydra_Software_Devel/417   8/18/09 2:53p nilesh
 * PR57807: Fix coverity errors
 *
 * Hydra_Software_Devel/416   8/3/09 5:16p nilesh
 * PR57256: In bavc.h, 7.943 frame rate should be 7.493.
 *
 * Hydra_Software_Devel/415   7/23/09 3:39p nilesh
 * PR55446: Updated 480p/576p/1080p interlaced scanout override to be
 * deterministic.  Added BXVD_DecodeSettings.eProgressiveOverrideMode to
 * control override.
 *
 * Hydra_Software_Devel/414   7/20/09 12:36p pblanco
 * PR56929: Added code to destroy VICH callbacks in BXVD_Close.
 *
 * Hydra_Software_Devel/413   7/6/09 3:35p davidp
 * PR56374: Turn on clocks in BXVD_Close to access AVD FW and AVD
 * registers.
 *
 * Hydra_Software_Devel/412   6/30/09 4:20p davidp
 * PR56374: Power down AVD related clocks in BXVD_Close.
 *
 * Hydra_Software_Devel/411   6/10/09 4:17p nilesh
 * PR55297: Enable FIC when effective frame rate is greater than display
 * rate
 *
 * Hydra_Software_Devel/PR55297/1   6/9/09 1:59p nilesh
 * PR55297: Default ePictureDropMode is now eFrame to prevent jitter
 * during STC fast forward and other drop scenarios
 *
 * Hydra_Software_Devel/410   6/4/09 4:07p nilesh
 * PR52424: BXVD_GetPPBParameterQueueInfo() now generates info on-the-fly
 *
 * Hydra_Software_Devel/409   6/1/09 6:35p davidp
 * PR55198: Add dynamic power management support to most STB platforms.
 *
 * Hydra_Software_Devel/408   5/27/09 4:38p davidp
 * PR50178: Keep AVD clocks enabled when closing still compatibility
 * channel.
 *
 * Hydra_Software_Devel/407   5/26/09 3:33p nilesh
 * PR52424: Added BXVD_Interrupt_eTSMResult callback and
 * BXVD_GetPPBParameterQueueInfo() function
 *
 * Hydra_Software_Devel/406   5/14/09 4:55p nilesh
 * PR54254: Added advanced clip support
 *
 * Hydra_Software_Devel/405   4/30/09 4:52p nilesh
 * PR54677: Added BXVD_GetPPBParameterInfo().  Added overscan flags to
 * BXVD_PPBParameterInfo struct.
 *
 * Hydra_Software_Devel/404   4/3/09 2:20p btosi
 * PR52959: removed BXVD_GetVsyncPlaybackRateDefault
 *
 * Hydra_Software_Devel/403   4/3/09 1:41p btosi
 * PR53039: in "BXVD_SetSkipPictureModeDecode()" moved "bPreserveState"
 * after BXVD_StartDecode()"
 *
 * Hydra_Software_Devel/402   3/31/09 5:29p davidp
 * PR52344: Add bAVC51Enable flag to channel settings to use more memory
 * for AVC 5.1 compliant streams.
 *
 * Hydra_Software_Devel/401   3/27/09 3:01p btosi
 * PR52959: added support for programmable playback rate in vsync mode
 *
 * Hydra_Software_Devel/400   3/26/09 6:13p nilesh
 * PR53580: Cleaned up DM structs
 *
 * Hydra_Software_Devel/399   3/24/09 4:52p nilesh
 * PR52905: Added ISR versions of most get/set functions.  Cleaned up
 * formatting.  Removed critical sections for some get functions.
 *
 * Hydra_Software_Devel/398   3/12/09 10:58a btosi
 * PR52950: added "BXVD_SetDisplayFieldMode_isr()"
 *
 * Hydra_Software_Devel/397   3/9/09 5:40p davidp
 * PR52977: Added bBluRayDecode boolean to BXVD_DecodeSettings
 *
 * Hydra_Software_Devel/396   3/4/09 4:06p nilesh
 * PR52516: Fixed KERNELMODE build warning
 *
 * Hydra_Software_Devel/395   3/4/09 3:43p nilesh
 * PR52516: Fixed KERNELMODE build error
 *
 * Hydra_Software_Devel/392   2/24/09 10:47a nilesh
 * PR50462: Fixed compiler warning
 *
 * Hydra_Software_Devel/391   2/23/09 3:09p nilesh
 * PR50462: Added ISR version of BXVD_SetClipTime()
 *
 * Hydra_Software_Devel/390   2/23/09 2:40p btosi
 * PR52205: added BXVD_Interrupt_eDecodeError callback
 *
 * Hydra_Software_Devel/389   2/17/09 3:56p btosi
 * PR50623: removed handle check from BXVD_[Set/Get]SwPcrOffset()
 *
 * Hydra_Software_Devel/388   2/12/09 1:58p nilesh
 * PR52053: Added BXVD_GetDisplayFieldMode_isr() call
 *
 * Hydra_Software_Devel/387   2/10/09 12:46p davidp
 * PR40316: BXVD_StopDecode call moved in BXVD_DecodeStillPictureReset.
 *
 * Hydra_Software_Devel/386   2/9/09 6:19p nilesh
 * PR45052: Set the default instance level to the global level
 *
 * Hydra_Software_Devel/385   2/5/09 5:44p nilesh
 * PR45052: Fixed coverity warning
 *
 * Hydra_Software_Devel/384   2/4/09 5:20p rayshap
 * PR51835: BXVD_GetGopTimeCode should make it obvious when no timecode is
 * present
 *
 * Hydra_Software_Devel/383   1/29/09 5:37p davidp
 * PR50471: Move critical section start to reduce race condition in
 * BXVD_StartDecode.
 *
 * Hydra_Software_Devel/382   1/29/09 5:19p davidp
 * PR50471: Disable/re-enable still picture ready interrupt around call to
 * BXVD_StartDecode to prevent race condition.
 *
 * Hydra_Software_Devel/381   1/23/09 2:53p nilesh
 * PR51470: Fixed compilation warning when DEBUG=b
 *
 * Hydra_Software_Devel/380   1/23/09 10:14a nilesh
 * PR51470: Fixed compilation errors when DEBUG=n
 *
 * Hydra_Software_Devel/379   1/21/09 2:01p nilesh
 * PR45052: Converted BDBG_xxx to BDBG_INSTANCE_xxx calls to support
 * multiple channels
 *
 * Hydra_Software_Devel/378   1/16/09 1:16p btosi
 * PR50623: removed BXVD_[Get/Set]STCOffset
 *
 * Hydra_Software_Devel/377   1/14/09 3:04p btosi
 * PR50623: added BXVD_[Set/Get]SwPcrOffset() and
 * BXVD_[Set/Get]HwPcrOffsetEnable()
 *
 * Hydra_Software_Devel/376   1/13/09 3:58p nilesh
 * PR47456: Added Frame Rate Detection (FRD) logic
 *
 * Hydra_Software_Devel/375   1/7/09 5:05p nilesh
 * PR44579: Added support for BAVC_FrameRateCode_e14_985 and
 * BAVC_FrameRateCode_e7_943
 *
 * Hydra_Software_Devel/374   12/19/08 12:04p btosi
 * PR50241: added support for "OpenChannel" initialization, cleaned up DM
 * API
 *
 * Hydra_Software_Devel/373   12/17/08 2:33p btosi
 * PR50241: removed the extraneous DM function declarations
 *
 * Hydra_Software_Devel/372   12/9/08 5:15p davidp
 * PR50033: Add bZeroDelayOutputMode to decodeSettings to be passed to FW
 * in ChannelStart command.
 *
 * Hydra_Software_Devel/371   12/2/08 1:14p nilesh
 * PR48693: Removed depenency on BTMR to succeed in BXVD_Open()
 *
 * Hydra_Software_Devel/370   11/25/08 1:59p nilesh
 * PR48726: Added API to tell DM whether to drop fields or frames
 *
 * Hydra_Software_Devel/369   11/25/08 12:51p davidp
 * PR49533: Preserve state in BXVD_ProcessWatchdog.
 *
 * Hydra_Software_Devel/368   11/21/08 4:04p btosi
 * PR49533: add bPreserveCounters to BXVD_SetSkipPictureModeDecode
 *
 * Hydra_Software_Devel/367   11/21/08 11:49a btosi
 * PR48843: added in support for uiPicturesReceivedCount
 *
 * Hydra_Software_Devel/366   11/20/08 5:12p davidp
 * PR49533: Add bPreserveCounters boolean to channel structure to not
 * decoder counters for certain startDecode situations.
 *
 * Hydra_Software_Devel/365   11/13/08 12:41p nilesh
 * PR47456: XVD default frame rate is now eUnknown.  If the coded frame
 * rate and app specified frame rate are both unknown, then XVD defaults
 * to using 30 for it's internal calculations.  If the coded frame rate
 * is unknown, then DM now sends VDC the app specified frame rate (which
 * could be unknown).
 *
 * Hydra_Software_Devel/364   11/5/08 3:25p nilesh
 * PR48693: Added BTMR support
 *
 * Hydra_Software_Devel/363   10/31/08 3:29p davidp
 * PR46543: Disable/re-enable hibernate mode install/uninstall device
 * callbacks.
 *
 * Hydra_Software_Devel/362   10/30/08 6:15p davidp
 * PR48544: In BXVD_GetChannelStatus, set AVDStatusBlock to 0 if channel
 * has not been started.
 *
 * Hydra_Software_Devel/361   10/28/08 12:28p davidp
 * PR47278: Add bExcessDirModeEnable BXVD_OpenChannel settings flag to
 * allocate more direct memory for FW to decode certain non conpliant AVC
 * streams.
 *
 * Hydra_Software_Devel/360   10/23/08 5:37p davidp
 * PR46566: Remove duplicate code in
 * BXVD_SetInterpolationModeForStill/MovingContext, call isr flavor.
 *
 * Hydra_Software_Devel/359   10/22/08 3:58p nilesh
 * PR47998: Improved clip support in VSYNC mode
 *
 * Hydra_Software_Devel/358   10/17/08 1:26p nilesh
 * PR47758: Added BXVD_GetDisplayOffset_isr()
 *
 * Hydra_Software_Devel/357   10/9/08 11:01a nilesh
 * PR47494: Merged BXVD_Interrupt_ePPBParameters callback support
 *
 * Hydra_Software_Devel/356   10/8/08 12:59p davidp
 * PR47274: Move Reset740x call in BXVD_Close.
 *
 * Hydra_Software_Devel/355   10/6/08 12:02p btosi
 * PR46567: copy IFrame counts and video info in BXVD_GetChannelStatus_isr
 *
 * Hydra_Software_Devel/354   10/3/08 5:29p davidp
 * PR46543: Add power management support for 3548/3556.
 *
 * Hydra_Software_Devel/353   9/27/08 1:51a nilesh
 * PR37222: Next PTS Valid flag is now set to false before display manager
 * is initialized to ensure compatibility between DMv1 and DMv2
 *
 * Hydra_Software_Devel/352   9/27/08 1:27a nilesh
 * PR37222: Fixed GetNextPTS() to call _isr() version instead of itself.
 *
 * Hydra_Software_Devel/351   9/19/08 2:03p davidp
 * PR46900: Move BNKI_LeaveCriticalSection to after
 * ReleasePPBOnCloseChannel call.
 *
 * Hydra_Software_Devel/350   9/18/08 2:02p davidp
 * PR46650: Fix MemCfgMode enum field names.
 *
 * Hydra_Software_Devel/349   9/17/08 9:20a btosi
 * PR39962: updated PTSInfo structure
 *
 * Hydra_Software_Devel/348   9/9/08 3:14p davidp
 * PR46566: Add ISR version of
 * BXVD_SetInterpolationModeForStill/MovingContent.
 *
 * Hydra_Software_Devel/347   9/8/08 6:42p davidp
 * PR46650: Add BXVD_P_GET_MEMORY_CONFIG macro and BXVD_P_ValidateHeaps to
 * sanity check heaps passed to BXVD_Open.
 *
 * Hydra_Software_Devel/346   8/28/08 4:53p davidp
 * PR44341: Add coverity overide comments for false positive coverity
 * defects.
 *
 * Hydra_Software_Devel/345   8/18/08 6:13p davidp
 * PR45801: Clear StillPictureToRelease flag when [re]opening channel in
 * compatibility mode.
 *
 * Hydra_Software_Devel/344   8/6/08 12:00p rayshap
 * PR45453: Addd isr version of set display offset
 *
 * Hydra_Software_Devel/343   8/5/08 2:25p nilesh
 * PR29915: Improve BDBG_MSG output
 *
 * Hydra_Software_Devel/342   8/5/08 2:22p davidp
 * PR45080: Add AVD status block support to BXVD_GetChannelStatus.
 *
 * Hydra_Software_Devel/341   8/1/08 4:56p davidp
 * PR43272: Merge PR43272 branch to mainline, new FW memory configuration
 * API.
 *
 * Hydra_Software_Devel/PR43272/1   7/17/08 12:35p davidp
 * PR43272: Add support for additional FW memory configuration parameters,
 * Direct mode and IL Work list memory.
 *
 * Hydra_Software_Devel/340   7/29/08 4:02p nilesh
 * PR37222: Removed stale comment
 *
 * Hydra_Software_Devel/339   7/29/08 4:01p nilesh
 * PR37222: BXVD_GetNextPTS() returns an error if the next picture is not
 * available on the delivery queue
 *
 * Hydra_Software_Devel/338   7/29/08 3:00p nilesh
 * PR37222: Added BXVD_GetNextPTS() support
 *
 * Hydra_Software_Devel/337   7/29/08 1:32p btosi
 * PR41123: added support for picture error handling
 *
 * Hydra_Software_Devel/336   7/24/08 4:37p nilesh
 * PR44761: Added 1080p pulldown scan mode support
 *
 * Hydra_Software_Devel/335   7/24/08 3:07p btosi
 * PR38467: init uiPreRollRate in StartDecode
 *
 * Hydra_Software_Devel/334   7/24/08 11:21a btosi
 * PR38467: finished up pre-roll logic
 *
 * Hydra_Software_Devel/333   7/16/08 4:38p nilesh
 * PR29915: Fixed still picture compatibility mode CDB alignment check
 *
 * Hydra_Software_Devel/332   7/10/08 4:44p rayshap
 * PR44715:Add message to start decode is using legacy decode setting w/o
 * signature
 *
 * Hydra_Software_Devel/331   7/8/08 1:49p btosi
 * PR38467: added support for PPBReceived callback
 *
 * Hydra_Software_Devel/330   7/1/08 4:44p nilesh
 * PR43585: Update to allow bxvd_dispmgr_dvd.c to compile
 *
 * Hydra_Software_Devel/329   7/1/08 1:36p nilesh
 * PR43585: Merge DVD Development branch to mainline
 *
 * Hydra_Software_Devel/328   7/1/08 12:44p pblanco
 * PR44363: Moved NULL check before handle type check.
 *
 * Hydra_Software_Devel/PR43585/1   6/23/08 8:34p nilesh
 * PR43585: Added BXVD_SetTSMWaitForValidSTC()
 *
 * Hydra_Software_Devel/327   6/19/08 6:20p davidp
 * PR43222: Add support for Mpeg4Part2/DivX still picture decode.
 *
 * Hydra_Software_Devel/326   6/12/08 4:47p nilesh
 * PR43585: Updated multi-decode XVD to compile with 7440
 *
 * Hydra_Software_Devel/325   5/28/08 1:49p davidp
 * PR39004: Add eDefaultFrameRate to BXVD_StartDecode settings.
 *
 * Hydra_Software_Devel/324   5/22/08 4:52p davidp
 * PR41411: Add verification that the AVD can access GISB.
 *
 * Hydra_Software_Devel/323   5/22/08 3:17p pblanco
 * PR42910: Added handle type checking for externally visable APIs that
 * take a BXVD_Handle or BXVD_ChannelHandle."
 *
 * Hydra_Software_Devel/322   5/21/08 2:51p pblanco
 * PR42910: Add handle type to handle initialization.
 *
 * Hydra_Software_Devel/321   5/20/08 4:27p davidp
 * PR42177: In BXVD_Open, output FW revision using DBG_WRN.
 *
 * Hydra_Software_Devel/320   5/15/08 6:14p rayshap
 * PR42737: Coverity Defect ID:7831
 *
 * Hydra_Software_Devel/319   5/15/08 12:49p pblanco
 * PR42702: Fixed BXVD_GetRevision function.
 *
 * Hydra_Software_Devel/318   5/14/08 12:29p davidp
 * PR21390: Add support for Aegis video instruction checker interrupt and
 * callback.
 *
 * Hydra_Software_Devel/317   4/30/08 5:39p davidp
 * PR41983: Add HITS mode to decode settings to be passed to FW in
 * StartDecode command.
 *
 * Hydra_Software_Devel/316   4/24/08 1:42p nilesh
 * PR32273: Rename bIgnorePcrDiscontinuity to
 * bVsyncModeOnPcrDiscontinuity. Set default to true.
 *
 * Hydra_Software_Devel/315   4/11/08 4:51p davidp
 * PR40234: Merge decoder debug logging from branch.
 *
 * Hydra_Software_Devel/PR40234/1   4/10/08 3:34p davidp
 * PR40234: Add decoder debug log memory size to BXVD_Open BXVD_Settings,
 * set default size to 0.
 *
 * Hydra_Software_Devel/314   4/3/08 2:17p nilesh
 * PR41276: Fixed possible out of bounds condition for StripeWidth and
 * StripeMultiple
 *
 * Hydra_Software_Devel/313   4/1/08 2:44p davidp
 * PR27168: Clear stillChannel flag in Xvd device handle if channel being
 * closed is the Still Picture Decode channel.
 *
 * Hydra_Software_Devel/312   3/25/08 5:14p davidp
 * PR40316: Merge PR40316 changes to mainline.
 *
 * Hydra_Software_Devel/PR40316/2   3/25/08 12:37p davidp
 * PR40316: Add BXVD_DecodeStillPictureReset to reset processing still
 * picture decode after a timeout has occurred.
 *
 * Hydra_Software_Devel/PR40316/1   3/18/08 6:12p davidp
 * PR40316: FW now quietly stops after each still picture is decoded.
 * Remove explicit stop, release PPB in DecodeStillPicture routine.
 *
 * Hydra_Software_Devel/311   3/12/08 2:29p davidp
 * PR40491: XPT pContextMap in DecodeSettings for BXVD_StartDecode should
 * never be null. Test upfront and return INVALID_PARAMETER if NULL.
 *
 * Hydra_Software_Devel/310   2/26/08 1:19p nilesh
 * PR31682: Added BXVD_DisplayFieldType_eSingleField enum
 *
 * Hydra_Software_Devel/309   2/8/08 3:14p davidp
 * PR38626: Add UseFieldAsFrame flag to BXVD_DeviceVdcInterruptSettings.
 *
 * Hydra_Software_Devel/308   1/31/08 12:40p nilesh
 * PR39174: Updated memory tables to match AVD Mosaic API doc v2.17.
 * Restructured memory size calculation code.
 *
 * Hydra_Software_Devel/307   1/24/08 11:39a nilesh
 * PR38570: Merge CABAC worklist API changes to mainline
 *
 * Hydra_Software_Devel/PR38570/1   1/21/08 5:11p nilesh
 * PR38570: Added support for cabac worklist buffer and size parameter to
 * AVD ChannelOpen
 *
 * Hydra_Software_Devel/306   1/18/08 5:58p davidp
 * PR38735: Put uiChainedISR item back in BXVD_DeviceVdcInterruptSettings
 * to support old and new for a while.
 *
 * Hydra_Software_Devel/305   1/17/08 6:06p davidp
 * PR38735: Create and use BXVD_DeviceVdcIntrSettingsFlags_None.
 *
 * Hydra_Software_Devel/304   1/15/08 5:21p davidp
 * PR38735: Add BXVD_LinkDecoders/UnlinkDecoders and associated data
 * structures.
 *
 * Hydra_Software_Devel/303   11/16/07 10:54a btosi
 * PR37081: fixed problem in BXVD_GetChannelStatus_isr() reported by
 * Coverity
 *
 * Hydra_Software_Devel/302   11/12/07 5:26p nilesh
 * PR36741: Fixed incorrect BXVD_Status data caused by race condition when
 * stopping decode
 *
 * Hydra_Software_Devel/301   11/8/07 10:48a btosi
 * PR32273: added support bIgnorePcrDiscontinuity
 *
 * Hydra_Software_Devel/300   11/6/07 2:58p btosi
 * PR30310: when DQT is enabled, set BXVD_P_AVD_NO_CONSTRAINTS when
 * calling BXVD_P_HostCmdSendDecChannelStart
 *
 * Hydra_Software_Devel/299   11/6/07 2:37p nilesh
 * PR36741: Fixed segmentation fault in BXVD_Close() when hXvdStatus is
 * not initialized
 *
 * Hydra_Software_Devel/298   11/6/07 2:19p davidp
 * PR27168: Remove use of SETUP_HEAP macro and associated routines per
 * code review
 *
 * Hydra_Software_Devel/297   11/6/07 12:36p nilesh
 * PR36741: Added XVD status reporting via BDBG console messages
 *
 * Hydra_Software_Devel/296   11/6/07 10:50a nilesh
 * PR36735: XVD : Need to add support for Adaptive System Time Management
 * (ASTM)
 *
 * Hydra_Software_Devel/295   10/31/07 11:07a btosi
 * PR36545: added support for uiCabacBinDepth in GetChannelStatus()
 *
 * Hydra_Software_Devel/294   10/26/07 2:05p nilesh
 * PR27168: Fixed compiler warning when using DEBUG=n
 *
 * Hydra_Software_Devel/293   10/26/07 1:27p nilesh
 * PR36480: Added bFGTEnable setting in BXVD_Settings to enable/disable
 * FGT SEI message parsing for a particular decoder
 *
 * Hydra_Software_Devel/292   10/23/07 5:48p nilesh
 * PR34118: Added BXVD_USE_CUSTOM_CONTEXT build flag to optionally link
 * firmware image into binary
 *
 * Hydra_Software_Devel/291   10/23/07 2:57p rayshap
 * PR36371: add decode error count to debug output on stop decode
 *
 * Hydra_Software_Devel/290   10/16/07 11:09a pblanco
 * PR35985: Backed out Coverity change. Report based on older code
 *
 * Hydra_Software_Devel/289   10/11/07 10:46a pblanco
 * PR35985: Fixed missing deallocation in error path found by Coverity
 *
 * Hydra_Software_Devel/288   9/28/07 4:12p nilesh
 * PR27168: Improved BDBG_MSG output for BXVD_Open(), BXVD_OpenChannel(),
 * and BXVD_StartDecode()
 *
 * Hydra_Software_Devel/287   9/21/07 10:34a btosi
 * PR34980: return uiVsyncCount in BXVD_GetChannelStatus
 *
 * Hydra_Software_Devel/286   9/7/07 6:22p davidp
 * PR34678: In BXVD_StartDecode, move resetting skip mode after
 * eDecoderState is set to eActive.
 *
 * Hydra_Software_Devel/285   9/6/07 5:35p nilesh
 * PR29915: Added BERR_TRACE wrapper around all return codes
 *
 * Hydra_Software_Devel/284   9/6/07 3:22p nilesh
 * PR34430: Channel specific cabac and picture heaps now fallback to
 * channel system heap instead of XVD's sub-heaps
 *
 * Hydra_Software_Devel/283   9/4/07 10:10a nilesh
 * PR29915: Fix error handling in BXVD_OpenChannel() when any error
 * occurs.  BXVD_CloseChannel() is now called to free any allocated
 * memory before returning an error.
 *
 * Hydra_Software_Devel/282   8/31/07 5:20p nilesh
 * PR29915: Fixed error handling in BXVD_OpenChannel() when allocating
 * memory
 *
 * Hydra_Software_Devel/281   8/31/07 3:54p nilesh
 * PR29915: Fixed memory leaks in normal and error cases of
 * BXVD_Open()/BXVD_Close()
 *
 * Hydra_Software_Devel/280   8/31/07 11:41a nilesh
 * PR29915: Fix FGT support for DVD platforms
 *
 * Hydra_Software_Devel/279   8/30/07 12:20p nilesh
 * PR34430: Channel specific heaps are now used for address conversion
 *
 * Hydra_Software_Devel/278   8/29/07 3:54p rayshap
 * PR33893: Add support for conditional b_Trickmode_Transitions in DM"
 *
 * Hydra_Software_Devel/277   8/29/07 1:08p nilesh
 * PR34430: Added support for channel specific memory heaps
 *
 * Hydra_Software_Devel/276   8/22/07 4:45p nilesh
 * PR34240: Fixed setting default uiVDCRectangleNumber if decode settings
 * struct has an invalid signature
 *
 * Hydra_Software_Devel/275   8/22/07 3:48p nilesh
 * PR29915: The FW memory config table for compatibility mode is now
 * generated on-the-fly at runtime
 *
 * Hydra_Software_Devel/274   8/22/07 11:49a nilesh
 * PR34240: Support moving eDisplayInterrupt and uiVDCRectangleNumber
 * settings from BXVD_ChannelSettings to BXVD_DecodeSettings
 *
 * Hydra_Software_Devel/273   8/21/07 3:04p davidp
 * PR34052: Remove nolonger used register addr pointers from BXVD_Handle,
 * use platform_info.stReg structure to reference registers
 *
 * Hydra_Software_Devel/272   8/16/07 11:52a nilesh
 * PR29915: GetChannelDefaultSettings() now fills in the proper decode
 * protocol and resolution based on the decode mode specified at
 * BXVD_Open()
 *
 * Hydra_Software_Devel/271   8/13/07 4:06p nilesh
 * PR29915: Multi-decode merge to mainline
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/54   8/10/07 2:49p nilesh
 * PR29915: Merge from mainline
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/53   8/10/07 1:02p davidp
 * PR29915: Move VDCRectangleNum from BXVD_StartDecode settings to
 * BXVD_OpenChannel settings.
 *
 * Hydra_Software_Devel/270   8/1/07 6:07p davidp
 * PR32564: Copy BXVD_Open decodeModes to Alloc'd memory, can not rely on
 * the arg passed will be valid after call returns.
 *
 * Hydra_Software_Devel/269   7/30/07 6:35p davidp
 * PR33261: Only send speed command to FW when decoder is already started.
 *
 * Hydra_Software_Devel/268   7/27/07 12:33p nilesh
 * PR 4727: TDL_0002 Title 30 Subpicture on - step doesn't always advance
 * time
 *
 * Hydra_Software_Devel/267   7/27/07 10:31a nilesh
 * PR30249: Removed duplicate eSTC values which prevented dynamic change
 * of STC source
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/52   8/8/07 1:24p nilesh
 * PR29915: Removed redudant code to set VDC rectangle number if it hasn't
 * been set, yet.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/51   8/8/07 1:17p nilesh
 * PR29915: Changed BXVD_GetDecodeDefaultSettings() to take an XVD channel
 * handle instead of an XVD device handle
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/50   8/7/07 5:13p nilesh
 * PR29915: Fixed problem with some decode settings being incorrectly
 * reset during a watchdog reset or flush decode
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/49   8/3/07 2:28p nilesh
 * PR29915: Fixed GetBufferConfig() not setting UsePictureCounter in
 * returned BAVC_CdbItbConfig struct
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/48   8/2/07 3:57p nilesh
 * PR29915: We now allocate and copy video compression standard list
 * passed into BXVD_OpenChannel() instead of copying the pointer directly
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/47   7/26/07 4:13p nilesh
 * PR29915: Prevent opening of the same channel twice before a
 * corresponding close
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/46   7/26/07 4:00p nilesh
 * PR29915: Add MPEG2 w/o BTP support
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/45   7/25/07 1:35p nilesh
 * PR29915: Cleaned up bxvd_priv.h and bxvd_vdec_info.h constants to match
 * XVD coding style
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/44   7/25/07 1:16p nilesh
 * PR29915: Removed warning
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/43   7/25/07 11:39a nilesh
 * PR29915: Merge from mainline
 *
 * Hydra_Software_Devel/266   7/24/07 9:56a btosi
 * PR32432: added BXVD_GetChannelStatus_isr()" variant
 *
 * Hydra_Software_Devel/265   7/19/07 1:33p btosi
 * PR32432: enhanced "BXVD_GetChannelStatus()" to return the
 * new"stCurrentDMInfo" data
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/42   7/24/07 3:31p davidp
 * PR29915: Move sending Config Cmd to after decoder is booted and before
 * channels are open in ProcessWatchdog.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/41   7/24/07 2:21p nilesh
 * PR29915: Added ePictureType parameter validation in
 * BXVD_DecodeStillPicture()
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/40   7/24/07 12:55p nilesh
 * PR29915: Added BXVD_DeviceInterrupt_eDisplayInterrupt support from
 * mainline
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/39   7/23/07 6:13p davidp
 * PR29915: Send FW config command to re-register VDC RUL masks in
 * BXVD_ProcessWatchdog.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/38   7/19/07 5:14p nilesh
 * PR29915: Update BXVD_GetBufferConfig() and BXVD_P_ParseDecodeMode() to
 * handle BXVD_DecodeMode_eCustom
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/37   7/19/07 2:43p nilesh
 * PR29915: Cleaned up variable naming style
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/36   7/18/07 6:21p nilesh
 * PR29915: Fixed typo in ePictureType validation code when in still
 * picture compatibility
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/35   7/18/07 6:12p nilesh
 * PR29915: Limit ePictureType to MPEG SD or MPEG HD in DecodeStillPicture
 * when in compatibilty mode
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/34   7/13/07 5:15p nilesh
 * PR29915: Updated memory tables based on v2.9 of Mosaic API doc
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/33   7/10/07 6:42p nilesh
 * PR29915: Added 7405 support to mosaic branch
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/32   7/10/07 5:18p nilesh
 * PR29915: Fixed bookkeeping related to still picture channel allocation
 * when using new API
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/31   7/10/07 10:41a nilesh
 * PR29915: Merged from mainline 07/10/2007
 *
 * Hydra_Software_Devel/264   6/28/07 3:24p nilesh
 * PR32611: Fixed heaps used for offset->virtual address conversion for
 * data structures returned by firmware
 *
 * Hydra_Software_Devel/263   6/22/07 11:32a btosi
 * PR30310: reset "bReversePlayback" in "BXVD_OpenChannel()"
 *
 * Hydra_Software_Devel/262   6/22/07 9:31a btosi
 * PR30130: reset "bReversePlayback" on "BXVD_StopDecode()"
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/30   7/3/07 3:46p nilesh
 * PR29915: bxvd_priv.h cleanup
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/29   7/3/07 12:30p nilesh
 * PR29915: Enabled FGT by default on all DVD platforms
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/28   7/3/07 12:08p nilesh
 * PR29915: Added FGT support
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/27   6/29/07 3:48p davidp
 * PR29915:
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/26   6/28/07 1:36p davidp
 * PR29915: Move remaining data structure definitions for
 * bxvd_devcmds_priv.h to bxvd_vdec_api.h
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/25   6/27/07 4:57p davidp
 * PR29915: Removed unused data types and data structures per code review.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/24   6/25/07 6:31p davidp
 * PR29915: Finish adding support for 2nd display device for each decoder
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/23   6/19/07 2:33p nilesh
 * PR29915: Changed VideoCmprCount -> uiVideoCmprCount to match notation
 * of other fields in BXVD_ChannelSettings
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/22   6/19/07 2:28p nilesh
 * PR29915: Fixed eSTC debug message in BXVD_StartDecode
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/21   6/19/07 1:11p nilesh
 * PR29915: Fixed bug in decode mode list copy
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/20   6/19/07 12:55p nilesh
 * PR29915: BXVD_Settings decode mode list is now copied locally
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/19   6/19/07 12:12p nilesh
 * PR29915: Merge from mainline.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/15   6/14/07 12:57p davidp
 * PR29915: Add support for 2nd display device for each decoder for
 * platforms that support this configuration.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/14   6/12/07 5:11p davidp
 * PR29915: Use bitwise OR not logical to create RUL mask.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/13   6/12/07 4:38p davidp
 * PR29915: Send FW config command in BXVD_RegisterVdcInterrupt.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/12   6/12/07 3:53p nilesh
 * PR29915: Temporatily forced DM to always use eSTC_eZero until AVD FW
 * sets STC1 properly
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/11   6/7/07 2:32p nilesh
 * PR29915: Added legacy app compatibility support for dual STC
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/10   6/6/07 12:37p nilesh
 * PR29915: Fixed FW timeout when closing still picture compatibility
 * channel
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/9   6/1/07 6:17p davidp
 * PR29915: Add multi-decode support to watchdog processing
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/8   6/1/07 2:20p nilesh
 * PR29915: Still picture release is now delayed and occurs on
 * BXVD_StopDecode()
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/7   5/29/07 5:30p nilesh
 * PR29915: Updated still picture compatibility mode to match discussed
 * design
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/6   5/29/07 4:53p davidp
 * PR29915: Change int32_t fields in channel handle to uint32_t
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/5   5/23/07 2:02p nilesh
 * PR29915: Added still picture compatibility mode
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/4   5/22/07 5:01p nilesh
 * PR29915: Added still picture support according to FW mosaic API
 * document v2.3 (TODO: Add compatibility mode)
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/3   5/18/07 4:38p davidp
 * PR29915: Create XVD sub-heaps after loading FW, add support for FW
 * config command.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/2   5/9/07 6:44p davidp
 * PR29915: Add channel number to FW close command.
 *
 * Hydra_Software_Devel/xvd_PR29915_Rel_Mosaic_FW_API/1   5/8/07 3:32p davidp
 * PR29915: Add initial API changes required for released FW API that
 * supports multi-decode.
 *
 * Hydra_Software_Devel/261   6/14/07 2:52p rayshap
 * PR32093: BAVC_VideoCompressionStd_eVC1SimpleMain results in no video
 *
 * Hydra_Software_Devel/260   6/13/07 4:06p nilesh
 * PR30249: Added dual STC support
 *
 * Hydra_Software_Devel/259   6/12/07 1:27p rayshap
 * PR31988: Set 3:2 pulldown on stop decode
 *
 * Hydra_Software_Devel/258   6/12/07 1:10p rayshap
 * PR31988: Add support for enable/disable of 3-2 pulldown - remove reset
 * at stop decode
 *
 * Hydra_Software_Devel/257   6/12/07 12:37p rayshap
 * PR31988: Add support for enable/disable of 3-2 pulldown
 *
 * Hydra_Software_Devel/256   5/29/07 10:52a nilesh
 * PR30249: Added support for retrieving STC1 snapshot
 *
 * Hydra_Software_Devel/255   5/23/07 12:42p nilesh
 * PR29874: Consolidate TSM threshold code
 *
 * Hydra_Software_Devel/254   5/15/07 1:23p pblanco
 * PR28215: Added support for variable stripe width and multiple.
 *
 * Hydra_Software_Devel/253   5/15/07 10:53a nilesh
 * PR30249: Display interrupt polarity is now returned by GetSTCValue()
 *
 * Hydra_Software_Devel/252   5/14/07 5:16p nilesh
 * PR30249: Changed BXVD_GetSTCValue() to return both the STC value and
 * polarity
 *
 * Hydra_Software_Devel/251   5/14/07 11:30a nilesh
 * PR30249: Merged UOD 2.x changes to mainline
 *
 * Hydra_Software_Devel/PR30249/8   5/9/07 3:17p nilesh
 * PR30249: Add BXVD_DisplayMgrMode setting to BXVD_Settings
 *
 * Hydra_Software_Devel/PR30249/7   5/7/07 3:02p vijeth
 * PR 30249: Fix for Warning messages
 *
 * Hydra_Software_Devel/PR30249/6   5/7/07 2:22p vijeth
 * PR 30249 : Fix for warning message
 *
 * Hydra_Software_Devel/PR30249/5   5/4/07 4:20p nilesh
 * PR30249: Updated UOD2 status
 *
 * Hydra_Software_Devel/PR30249/4   5/3/07 3:53p nilesh
 * PR30249: Moved app specified ui32STCOffset variable from
 * BXVD_P_DisplayManager_STCInfo to BXVD_P_TSMCommands
 *
 * Hydra_Software_Devel/PR30249/3   5/3/07 10:28a nilesh
 * PR30249: Implemented BXVD_SetFirstPictureDefaultPTS_isr() function.
 * Added critical section around original SetClipTime call.
 *
 * Hydra_Software_Devel/PR30249/2   5/1/07 4:05p nilesh
 * PR30249: Added BXVD_EnableInterrupt() support for BXVD_Interupt_eMarker
 *
 * Hydra_Software_Devel/PR30249/1   5/1/07 3:44p nilesh
 * PR30249: Changes to support UOD 2.x development
 *
 * Hydra_Software_Devel/250   4/24/07 2:51p rayshap
 * PR20086: VC1 threshold increased to 10 seconds, no longer used as the
 * default.
 * MPEG will be the default
 *
 * Hydra_Software_Devel/249   4/12/07 9:50a pblanco
 * PR29649: Removed redundant sanity check from non-isr version of
 * BXVD_EnableInterrupt.
 *
 * Hydra_Software_Devel/248   4/11/07 2:34p pblanco
 * PR29649: Added _isr version of BXVD_EnableInterrupt.
 *
 * Hydra_Software_Devel/247   4/11/07 11:02a pblanco
 * PR29649: Added BXVD_EnableInterrupt API.
 *
 * Hydra_Software_Devel/246   4/3/07 4:16p pblanco
 * PR27168: Removed unnecessary spin and entire structure copy from
 * BXVD_GetPTS_isr. Only the relevant members are assigned.
 *
 * Hydra_Software_Devel/245   4/2/07 4:03p pblanco
 * PR29183: Preserve discard threshold value through a BXVD_FlushDecode
 * call.
 *
 * Hydra_Software_Devel/244   3/29/07 3:38p rayshap
 * PR 28227: Add first picture preview support.  Needed to change logic to
 * verify valid mode was supplied.
 *
 * Hydra_Software_Devel/243   3/27/07 1:51p pblanco
 * PR27168: Removed bxvd_mem.h since we no longer reference this API.
 *
 * Hydra_Software_Devel/242   3/27/07 1:35p pblanco
 * PR27168: Converted BXVD_Mem_Read call to BKNI_Memcpy call in
 * BXVD_GetPTS so that we can deprecate the bxvd_mem module.
 *
 * Hydra_Software_Devel/241   3/26/07 12:26p pblanco
 * PR27168: Converted more hardcoded constants to defined constants.
 *
 * Hydra_Software_Devel/240   3/23/07 1:27p pblanco
 * PR27168: Began conversion of hard coded value to appropriate #defined
 * constants.
 *
 * Hydra_Software_Devel/239   3/22/07 11:51a pblanco
 * PR28629: Converted GOP timecode masks and shifts from hard coded
 * constants to #defined constants.
 *
 * Hydra_Software_Devel/238   3/20/07 12:08p pblanco
 * PR28629: Added BXVD_GetGopTimeCode API and support code.
 *
 * Hydra_Software_Devel/237   3/16/07 1:58p gaurava
 * PR 27168: Clear pending interrupts after booting AVD since they are
 * edge-triggered
 * and Picture-data ready interrupts will stop going to AVD if one existed
 * before
 * AVD reset.
 *
 * Hydra_Software_Devel/236   3/15/07 7:20p davidp
 * PR27168: Clear any pending VEC interrupts before enabling interrupt.
 *
 * Hydra_Software_Devel/235   3/15/07 10:37a pblanco
 * PR27168: Fixed compiler warning caused by previous checkin.
 *
 * Hydra_Software_Devel/234   3/15/07 10:31a pblanco
 * PR27168: Removed deprecated FW command event and associated code.
 *
 * Hydra_Software_Devel/233   3/12/07 4:10p davidp
 * PR27947: Set hXvdCh->stChannelState.stTSMCmd.ui32PTSOffset to zero in
 * BXVD_StopDecode when preserve flag is cleared.
 *
 * Hydra_Software_Devel/232   3/2/07 1:35p nilesh
 * PR26188: Merged 7400B0 bring-up branch to mainline.  Cleaned up heap
 * naming and usage.
 *
 * Hydra_Software_Devel/PR26188/1   2/27/07 4:05p nilesh
 * PR26188: 7400B0 Bring-Up
 *
 * Hydra_Software_Devel/231   2/13/07 5:42p davidp
 * PR27836: Reset DisplayFieldMode in BXVD_StopDecode when preserve flag
 * is false.
 *
 * Hydra_Software_Devel/230   2/7/07 1:23p nilesh
 * PR27373: Added user-settable default frame rate if unknown from stream
 *
 * Hydra_Software_Devel/229   2/6/07 11:06a nilesh
 * PR27575: Fix 7440 Watchdog and make watchdog more robust on all
 * platforms.
 *
 * Hydra_Software_Devel/PR27575/1   2/2/07 2:10p nilesh
 * PR27575: Fix 7440 Watchdog
 *
 * Hydra_Software_Devel/227   1/30/07 8:21a pblanco
 * PR27168: Backed out change to setting structure that removed deprecated
 * members.
 *
 * Hydra_Software_Devel/226   1/29/07 1:54p pblanco
 * PR27168: More review driven code cleanup.
 *
 * Hydra_Software_Devel/225   1/10/07 5:58p davidp
 * PR26837: If in sparse mode when bxvd_SetSkipPictureModeDecode is
 * called, stop/start decode.
 *
 * Hydra_Software_Devel/224   1/9/07 3:09p pblanco
 * PR26648: Moved watchdog initialization to *after* decoder startup.
 *
 * Hydra_Software_Devel/223   12/22/06 2:39p pblanco
 * PR26572: These changes were provided by David E. Needed critsecs on
 * device and interrupt callback install and uninstall routines.
 *
 * Hydra_Software_Devel/222   12/18/06 3:37p davidp
 * PR26648: Add call to BXVD_P_SETUP_WATCHDOG in BXVD_ProcessWatchdog to
 * prevent Watchdog from firing during FW loading.
 *
 * Hydra_Software_Devel/221   12/13/06 7:16p davidp
 * PR25443: Remove deprecated API routines and associated data structures.
 *
 * Hydra_Software_Devel/220   12/12/06 6:04p davidp
 * PR24297: Initialize XVD_to_VDC_Info.eYCbCrType in BXVD_ChannelOpen.
 *
 * Hydra_Software_Devel/219   12/12/06 2:43p davidp
 * PR25443: Merge Simplified FW API branch into mainline
 *
 * Hydra_Software_Devel/218   12/6/06 11:11a pblanco
 * PR26199: Altered change based on Gaurav's input. There is now a
 * separate definefor VC1 streams (4 secs.) and MPEG is back to 2 secs.
 *
 * Hydra_Software_Devel/217   11/21/06 1:47p davidp
 * PR25724: Add DM Picture Delivery Count to BXVD_GetChannelStatus.
 *
 * Hydra_Software_Devel/216   11/17/06 4:50p nilesh
 * PR25868: Merged authenticated image support to mainline
 *
 * Hydra_Software_Devel/PR25868/3   11/17/06 4:32p nilesh
 * PR25868: Added warning messages for incorrect usage of the
 * BXVD_USE_CUSTOM_IMAGE
 *
 * Hydra_Software_Devel/PR25868/2   11/17/06 3:47p nilesh
 * PR25868: Added NULL handle error checking in watchdog routines
 *
 * Hydra_Software_Devel/PR25868/1   11/16/06 3:37p nilesh
 * PR25868: Added authenticated image support
 *
 * Hydra_Software_Devel/215   11/14/06 12:01p nilesh
 * PR25365: Merged BIMG support to mainline
 *
 * Hydra_Software_Devel/214   11/13/06 6:22p davidp
 * PR25698: Restore CDB_Read register address in BXVD_FlushDecode
 *
 * Hydra_Software_Devel/212   11/9/06 4:42p davidp
 * PR25698: Explicitly save XPT Rave CDB_Read register address being used
 * for current decode to be restored during watchdog processing.
 *
 * Hydra_Software_Devel/211   10/12/06 3:01p davidp
 * PR24448: Prevent DM from being called during BXVD_ProcessWathchdog
 *
 * Hydra_Software_Devel/210   10/10/06 1:13p nilesh
 * PR23237: Added DVD TSM default values in OpenChannel
 *
 * Hydra_Software_Devel/209   10/9/06 6:28p nilesh
 * PR23113: Added DVD-specific API calls: SetClipTime() and
 * [Set/Get]PtsErrorReport[Upper/Lower]Threshold()
 *
 * Hydra_Software_Devel/208   10/9/06 6:23p nilesh
 * PR23113: Simplied [Install/UnInstall]InterruptCallback code
 *
 * Hydra_Software_Devel/207   9/27/06 10:47a gaurava
 * PR 24559: Removed hack for printing number of pictures delivered late
 * by video decoder to DM.
 *
 * Hydra_Software_Devel/206   9/22/06 2:21p davidp
 * PR22867: Update FW memory usage table, add secure memory heap support.
 *
 * Hydra_Software_Devel/205   9/14/06 7:44a pblanco
 * PR22398: Added code to set/unset bFreeze in
 * BXVD_Enable/DisableVideoFreeze.
 *
 * Hydra_Software_Devel/204   9/13/06 6:39p davidp
 * PR22817: Update CDB/ITB memory usage table, also FW decode mode memory
 * usage table.
 *
 * Hydra_Software_Devel/203   9/12/06 4:26p davidp
 * PR24126: BXVD_GetPictureTag api added.
 *
 * Hydra_Software_Devel/202   9/8/06 5:01p nilesh
 * PR23113: Fix BXVD_Open() initialization sequence.  Chip is now reset
 * BEFORE setting up interrupts
 *
 * Hydra_Software_Devel/201   9/5/06 3:04p davidp
 * PR22867: Merge secure heap (Cabac) support
 *
 * Hydra_Software_Devel/xvd_cabac_heap/1   8/30/06 5:10p davidp
 * 22867: SSeparate FW memory so Cabac bin memory can me moved to secure
 * heap.
 *
 * Hydra_Software_Devel/199   8/10/06 4:11p davidp
 * PR23356: In BXVD_StartDecode, if decode already in progress, stop the
 * decode. In BXVD_ProcessWatchdog, don't restart decode if decoder was
 * not active.
 *
 * Hydra_Software_Devel/198   8/9/06 5:20p davidp
 * PR23343: In BXVD_GetGopTimeCode, poll vdec status block to read stable
 * data.
 *
 * Hydra_Software_Devel/197   8/9/06 3:06p davidp
 * PR20394: PCR TimeBase decoder setting now assigned a default value in
 * BXVD_OpenChannel.
 *
 * Hydra_Software_Devel/196   8/8/06 3:05p davidp
 * PR23296: Output delay count minus 1, count is 1 not 0 based.
 *
 * Hydra_Software_Devel/195   8/8/06 2:43p davidp
 * PR23343: Use pointer to GOP structure not structure instance in address
 * conversion.
 *
 * Hydra_Software_Devel/194   8/8/06 12:50p davidp
 * PR23296: Add BDBG_MSG to BXVD_StopDecode to display delayed picture
 * count.
 *
 * Hydra_Software_Devel/193   8/7/06 4:04p davidp
 * PR22633: Initialize eCurrentSkipMode to BXVD_SkipMode_eDecode_IPB when
 * decode starts.
 *
 * Hydra_Software_Devel/192   8/3/06 6:27p davidp
 * PR22633: Only send SetSkipPictureMode command to firmware if the
 * skipmode has changed from previous call.
 *
 * Hydra_Software_Devel/191   8/3/06 3:35p pblanco
 * PR22673: Added FW revision debug message to BXVD_Open.
 *
 * Hydra_Software_Devel/190   7/31/06 7:50p davidp
 * PR22673: Add picture buffer address macros for chip specific use.
 *
 * Hydra_Software_Devel/189   7/26/06 4:50p davidp
 * PR22673: Cleanup more platform specific code, add hHWMemHeap element in
 * Xvd handle.
 *
 * Hydra_Software_Devel/188   7/25/06 9:03a pblanco
 * PR22548: Added parameter check to BXVD_SetChannelChangeMode.
 *
 * Hydra_Software_Devel/187   7/24/06 10:58a davidp
 * PR22673: Added SetupFWBuffer platform routines.
 *
 * Hydra_Software_Devel/186   7/20/06 6:19p davidp
 * PR22673: Add init register pointers and mask platform routines.
 *
 * Hydra_Software_Devel/185   7/18/06 12:09a nilesh
 * PR22673: Code restructure for 97400, 97401 B0, and 97118
 *
 * Hydra_Software_Devel/PR22673/1   7/17/06 2:33p nilesh
 * PR22673: Restructure on 97401
 *
 * Hydra_Software_Devel/184   7/17/06 1:55p pblanco
 * PR22673: Removed 7401a0 conditionalized code and restructured some of
 * the remaining conditional code.
 *
 * Hydra_Software_Devel/183   7/14/06 1:22p pblanco
 * PR22638: Changed Get/SetPtsStcDiffThreshold to match 7411 with a long
 * threshold argument.
 *
 * Hydra_Software_Devel/182   7/13/06 5:22p davidp
 * PR22673: Add BXVD_P_SetWatchdog_7401B0 routine, remove some 7401A0
 * support.
 *
 * Hydra_Software_Devel/181   7/13/06 12:46p pblanco
 * PR22638: Added BXVD_Get/SetPtsStcDiffThreshold functions.
 *
 * Hydra_Software_Devel/180   7/12/06 4:48p davidp
 * PR20017: Cleanup BXVD_Open, add platform specific setup routines
 *
 * Hydra_Software_Devel/179   7/11/06 5:06p davidp
 * PR20017: Add FW inner loop Watchdog support, also add platform specific
 * SetupInterrupts routines
 *
 * Hydra_Software_Devel/178   7/10/06 9:57a pblanco
 * PR22548: Minor fixes for bugs found during QA testing. BXVD_Close now
 * NULLs closed handle and parameter validation was added to
 * BXVD_SetVideoDisplayMode and to BXVD_SetDisplayFieldMode.
 *
 * Hydra_Software_Devel/177   7/6/06 9:40a pblanco
 * PR21943: Fixed conditional compilation for 7401 B0, broken when
 * conditionalizing for 7118.
 *
 * Hydra_Software_Devel/176   7/5/06 3:19p pblanco
 * PR21943: Conditionalization for 7118
 *
 * Hydra_Software_Devel/175   6/30/06 11:20a pblanco
 * PR22398: Added function skeleton for BXVD_GetVideoFreezeState/
 *
 * Hydra_Software_Devel/174   6/29/06 7:21p davidp
 * PR20017: Enable 7401 B0 watchdog support
 *
 * Hydra_Software_Devel/173   6/29/06 2:20p pblanco
 * PR22398: Added function skeletons for video freeze/unfreeze.
 *
 * Hydra_Software_Devel/172   6/22/06 4:49p davidp
 * PR20017: Disable watchdog on 7401 B0 until FW supports new watchdog.
 *
 * Hydra_Software_Devel/171   6/21/06 6:13p davidp
 * PR22259: Add Watchdog support and fix a couple problems related to L2
 * MBX interrupt processing
 *
 * Hydra_Software_Devel/170   6/15/06 4:06p davidp
 * PR20017: Remove references to mmap'd FW heap
 *
 * Hydra_Software_Devel/169   6/14/06 5:18p davidp
 * PR20017: Add 7401 B0 support for MBX, SeqHdr and DecodeStillPicture FW
 * generated interrupts.
 *
 * Hydra_Software_Devel/168   6/13/06 10:17a pblanco
 * PR20017: Code cleanup for 7401b0.
 *
 * Hydra_Software_Devel/167   6/12/06 6:09p davidp
 * PR20017: Allocate memory for FW code out of framework heap.
 *
 * Hydra_Software_Devel/166   6/8/06 4:31p davidp
 * PR20017: Add support for FW/MIPS cmd mailbox interrupt.
 *
 * Hydra_Software_Devel/165   6/8/06 9:17a pblanco
 * PR20017: Changed chip version test conditionals from A0 and B0 to
 * BCHP_VER_A0 and BCHP_VER_B0 to conform with the rest of magnum.
 *
 * Hydra_Software_Devel/164   6/6/06 2:33p davidp
 * PR20017: Add vsync interrupt support for 7401 B0 L2 interrupt
 *
 * Hydra_Software_Devel/163   5/30/06 10:36a pblanco
 * PR20017: Make sure register pointers are set correctly for 7401 B0.
 *
 * Hydra_Software_Devel/162   5/24/06 4:52p davidp
 * PR21712: 7400 decoder specific 7400 watchdog changes lost, changes
 * merged back in
 *
 * Hydra_Software_Devel/161   5/23/06 3:18p pblanco
 * PR20017: Conditionalize DestroyCallback in BXVD_Close.
 *
 * Hydra_Software_Devel/160   5/23/06 3:16p pblanco
 * PR20017: More interrupt conditionalization.
 *
 * Hydra_Software_Devel/160   5/23/06 3:14p pblanco
 * PR20017: More interrupt conditionalization by chip version.
 *
 * Hydra_Software_Devel/160   5/23/06 2:48p pblanco
 * PR20017: Conditionalized interrupt enable by chip version.
 *
 * Hydra_Software_Devel/159   5/22/06 5:58p davidp
 * PR21712: Use 7400 decoder specific watchdog timers.
 *
 * Hydra_Software_Devel/158   5/18/06 10:30a pblanco
 * PR21610: BXVD_GetBufferConfig now returns an error code if one of its
 * pointer arguments is NULL.
 *
 * Hydra_Software_Devel/157   5/16/06 10:19a pblanco
 * PR20649: Cleanup for B0. Conditionals must be replaced or removed in
 * later development.
 *
 * Hydra_Software_Devel/156   5/11/06 6:08p davidp
 * PR21532: Created new BXVD_[Un]InstallDeviceInterruptCallback to handle
 * device instance interrupts for DecodeStillPicture and Watchdog.
 *
 * Hydra_Software_Devel/155   5/10/06 4:49p davidp
 * PR20566: Change BXVD_SetSTCInvalidFlag to BXVD_SetSTCInvalid_isr
 *
 * Hydra_Software_Devel/154   5/9/06 11:43a pblanco
 * PR21472: Added Set/Get API for monitor refresh rate.
 *
 * Hydra_Software_Devel/153   5/5/06 2:09p davidp
 * PR21156: Add BXVD_P_ReleasePPB_isr and related _isr routines for DM to
 * use at ISR time.
 *
 * Hydra_Software_Devel/152   5/4/06 3:45p pblanco
 * PR21416: Make sure DM is in "both fields" mode when decode starts or
 * restarts.
 *
 * Hydra_Software_Devel/151   5/3/06 2:37p pblanco
 * PR19877: Cleaned up BXVD_CloseChannel code so that the appropriate
 * return code is returned by the function. This is in support of
 * Sylvain's testing.
 *
 * Hydra_Software_Devel/150   5/3/06 9:56a pblanco
 * PR19877: BXVD_Close no longer implicitly closes channels.
 *
 * Hydra_Software_Devel/149   4/28/06 2:46p davidp
 * PR20537: Decoder FW was not decoding the last slice.
 *
 * Hydra_Software_Devel/148   4/28/06 11:06a pblanco
 * PR19877: Added code to prevent BXVD_StartDecode from restarting an
 * active decode         and added DM initialization to the critical
 * section.
 *
 * Hydra_Software_Devel/147   4/27/06 4:13p pblanco
 * PR19877: Added guard code to BXVD_StopDecode to make sure we don't
 * attempt to stop an inactive channel.
 *
 * Hydra_Software_Devel/146   4/26/06 11:07a pblanco
 * PR20649: Temporary definition for CPU status interrupt shift. Will be
 * removed when definition exists in B0 rdb.
 *
 * Hydra_Software_Devel/145   4/25/06 4:07p pblanco
 * PR20649: Added callback code for VIC interrupt.
 *
 * Hydra_Software_Devel/144   4/25/06 3:45p pblanco
 * PR20649: Removed includes for bchp_xpt_pcroffset*.h which are included
 * by bxvd_intr.c now.
 *
 * Hydra_Software_Devel/143   4/24/06 2:11p davidp
 * PR18043: Add support for Sequence Header interrupt processing
 *
 * Hydra_Software_Devel/142   4/20/06 1:32p pblanco
 * PR19284: Removed references to BXVD_Interrupt_eNewPtsStcOffset.
 *
 * Hydra_Software_Devel/141   4/17/06 6:27p davidp
 * PR20916: Save and restore channelChangeMode during disableForFlush and
 * Flush processing
 *
 * Hydra_Software_Devel/140   4/15/06 8:12a davidp
 * PR19600: Fix compiler conditional type-o
 *
 * Hydra_Software_Devel/139   4/14/06 2:17p davidp
 * PR19600: Move 7400 AVD FW load address to 62MB
 *
 * Hydra_Software_Devel/138   4/12/06 12:49p davidp
 * PR19600: Move 7401 FW load address to 48mb, also decrease size to 1mb
 *
 * Hydra_Software_Devel/137   4/6/06 3:51p pblanco
 * PR19877: Cleaned up code based on removal of some unused channel
 * context structure members.
 *
 * Hydra_Software_Devel/136   4/6/06 12:59p pblanco
 * PR20625: Use the channel state mute flag in BXVD_EnableMute instead of
 * the channel context structure version.
 *
 * Hydra_Software_Devel/135   4/5/06 3:01p pblanco
 * PR20274: Made sure that BUD mode state is restored after a FW channel
 * open call.
 *
 * Hydra_Software_Devel/134   4/5/06 2:06p pblanco
 * PR15434: Added default setting for RAVE endianess to BXVD_Settings
 * initialization and passed this value through to FW channel open host
 * command.
 *
 * Hydra_Software_Devel/133   4/4/06 3:28p davidp
 * PR20353: Add capture still picture debug support for 7400
 *
 * Hydra_Software_Devel/132   4/4/06 10:39a pblanco
 * PR19877: Removed C++ style comment.
 *
 * Hydra_Software_Devel/131   4/4/06 8:36a pblanco
 * PR20274: Added in David's change to preserve host sparse mode. I will
 * talk to Gaurav to find out why this is necessary.
 *
 * Hydra_Software_Devel/130   4/3/06 3:42p pblanco
 * PR19887: Make sure parity flag is correctly set based on CRC enable. It
 * was being interpreted "backwards" before.
 *
 * Hydra_Software_Devel/129   4/3/06 2:21p pblanco
 * PR20274: Reset sparse mode flag on BXVD_FlushDecode call.
 *
 * Hydra_Software_Devel/128   4/3/06 10:06a pblanco
 * PR20566: Added BXVD_SetSTCInvalidFlag API.
 *
 * Hydra_Software_Devel/127   3/30/06 1:37p pblanco
 * PR19284: Added default initialization for vsync diff threshold (10) and
 * added the interrupt callback enum BXVD_Interrupt_eNewPtsStcOffset and
 * supporting code.
 *
 * Hydra_Software_Devel/126   3/29/06 2:26p pblanco
 * PR19099: Select discard threshold based on compression type in
 * BXVD_StartDecode.
 *
 * Hydra_Software_Devel/125   3/29/06 11:08a pblanco
 * PR20202: BXVD_OpenChannel WAS NOT propagating channel default values.
 * It is now.
 *
 * Hydra_Software_Devel/124   3/28/06 6:14p davidp
 * PR20353: Add decoder private memory base address to luma/chroma buffer
 * address being returned to app.
 *
 * Hydra_Software_Devel/123   3/28/06 9:46a pblanco
 * PR19877: Completed code for BXVD_GetGopTimeCode and removed unused or
 * deprecated code.
 *
 * Hydra_Software_Devel/122   3/28/06 8:40a pblanco
 * PR19877: Wired new BXVD_Interrupt_ePtsStcOffset into install/uninstall
 * interrupt code.
 *
 * Hydra_Software_Devel/121   3/27/06 6:33p davidp
 * PR20353: Add Decode Still Picture support for 7400
 *
 * Hydra_Software_Devel/120   3/24/06 1:34p pblanco
 * PR20274: Interim checkin prior to FW implementation.
 *
 * Hydra_Software_Devel/119   3/23/06 8:57a pblanco
 * PR19877: Made necessary modifications to code after moving
 * bPreserveState from public to private data area.
 *
 * Hydra_Software_Devel/118   3/22/06 5:16p davidp
 * PR20347: Add L2 interrupt support
 *
 * Hydra_Software_Devel/117   3/21/06 5:19p pblanco
 * PR20246: Have BXVD_StartDecode check preserve context flag before
 * attempting to reset PVR state.
 *
 * Hydra_Software_Devel/116   3/21/06 1:34p pblanco
 * PR20202: Added default initialization of ulRemovalDelay (value: 0) in
 * channel settings structure.
 *
 * Hydra_Software_Devel/115   3/16/06 10:56a pblanco
 * PR20241: Phase 2.0 release code cleanup.
 *
 * Hydra_Software_Devel/114   3/15/06 4:07p pblanco
 * PR20197: BXVD_SetDisplayFieldMode was setting the wrong variable so the
 * mode change was never seen by DM. This has been corrected and the
 * bogus context structure member has been removed.
 *
 * Hydra_Software_Devel/113   3/14/06 1:53p davidp
 * PR20164: Fix still buffer picture buffer allocation, align to 4K.:
 *
 * Hydra_Software_Devel/112   3/9/06 4:15p davidp
 * PR18862: Added BXVD)DisableForFlush and modified functionality of
 * BXVD_FlushDecode, now a stop/start decode:
 *
 * Hydra_Software_Devel/111   3/8/06 11:19a pblanco
 * PR20077: Removed ISR code which is now in bxvd_intr.c
 *
 * Hydra_Software_Devel/110   3/6/06 1:19p pblanco
 * PR19785: Changed set/get skip mode APIs and code to reflect new
 * enumerations for skip modes.
 *
 * Hydra_Software_Devel/109   2/28/06 4:36p davidp
 * PR18906: Pass proper field polarity/frame value to VDC for
 * PictureDataRdy interrupt regardless if decode is in progress.:
 *
 * Hydra_Software_Devel/108   2/28/06 10:31a pblanco
 * PR19877: Additional structure and code cleanup.
 *
 * Hydra_Software_Devel/107   2/27/06 3:32p pblanco
 * PR19877: Removed redundant structure members and their references in
 * code.
 *
 * Hydra_Software_Devel/106   2/27/06 12:51p pblanco
 * PR19123: Temporarily disable TSM checking for 7400.
 *
 * Hydra_Software_Devel/105   2/22/06 11:31a pblanco
 * PR19785: Implemented BXVD_GetSkipPictureModeConfig.
 *
 * Hydra_Software_Devel/104   2/22/06 9:14a pblanco
 * PR19795: Reset PVR settings to default values in BXVD_StartDecode.
 *
 * Hydra_Software_Devel/103   2/21/06 1:46p pblanco
 * PR19785: Implemented BXVD_SetSkipPictureMode API. This also needs FW
 * support.
 *
 * Hydra_Software_Devel/102   2/17/06 3:17p davidp
 * PR16792: Modify FW API for Still Picture Decode, feature now working:
 *
 * Hydra_Software_Devel/101   2/14/06 1:12p pblanco
 * PR19123: Removed static data from interrupt routines. The equivalent
 * variables are now in the per-instance XVD context structure.
 *
 * Hydra_Software_Devel/100   2/13/06 5:30p hongtaoz
 * PR19082: bring up dual decode for 7400;
 *
 * Hydra_Software_Devel/99   2/10/06 3:05p hongtaoz
 * PR19082: bring up the 2nd xvd instance;
 *
 * Hydra_Software_Devel/98   2/9/06 10:08a hongtaoz
 * PR19082: bring up xvd_vdc_test for 7400;
 *
 * Hydra_Software_Devel/97   2/2/06 4:03p pblanco
 * PR19123: More code cleanup. Changed all instances of uAvcInstance to
 * uDecoderInstance and removed last remaining references to the old
 * decoder context array.
 *
 * Hydra_Software_Devel/96   2/2/06 2:34p pblanco
 * PR19123: Code and data structure clean up. Benefits both 7401 and 7400.
 *
 * Hydra_Software_Devel/95   2/1/06 3:53p pblanco
 * PR19123: Changed "AVC" instance array in XVD context structure to a
 * single instance of type BXVD_DecoderContext. Tested against 7401 with
 * no regressions and against 7400 with no regressions.
 *
 * Hydra_Software_Devel/94   1/30/06 2:40p pblanco
 * PR19123: Instance initialization of some context values were out of
 * order in BXVD_Open.
 *
 * Hydra_Software_Devel/93   1/26/06 4:02p davidp
 * PR19123: Remove printf debug messages, clean up compiler warnings:
 *
 * Hydra_Software_Devel/92   1/25/06 11:49a pblanco
 * PR19123: Create second event for AVD instance 1 mailbox.
 *
 * Hydra_Software_Devel/91   1/24/06 2:51p pblanco
 * PR19123: 7400 firmware bring up debugging changes.
 *
 * Hydra_Software_Devel/90   1/20/06 10:00a pblanco
 * PR19123: Added decoder instance to defaults and made sure correct
 * instance is selected for 7400 implementation.
 *
 * Hydra_Software_Devel/89   1/18/06 2:22p pblanco
 * PR19123: Fix error introduced in previous checkin for 7400.
 *
 * Hydra_Software_Devel/88   1/18/06 1:38p pblanco
 * PR19123: Modifications for 7400 port to allow bxvd.c to build in the
 * xvd_test compilation harness.
 *
 * Hydra_Software_Devel/87   1/17/06 7:03p davidp
 * PR16792:  Change FW data memory allocation alignment:
 *
 * Hydra_Software_Devel/86   1/16/06 4:38p pblanco
 * PR19099: Added initialization of default value for picture discard
 * threshold in BXVD_OpenChannel.
 *
 * Hydra_Software_Devel/85   1/16/06 1:27p davidp
 * PR16792: BXVD_Open now uses two heap pointers, one for FW code (2MB)
 * the other for FW picture buffers.
 *
 * Hydra_Software_Devel/84   1/13/06 10:00a pblanco
 * PR16052: Added default initialization of PVR and other variables to
 * open channel.
 *
 * Hydra_Software_Devel/83   1/11/06 10:08a pblanco
 * PR18908: Added code to set video state to Decode_Started in
 * BXVD_StartDecode.
 *
 * Hydra_Software_Devel/82   1/3/06 11:16a pblanco
 * PR16052: Removed C++ style commend from BXVD_DecodeStillPicture.
 *
 * Hydra_Software_Devel/81   12/23/05 6:50p davidp
 * PR16878: Reset Decoder in Watchdog ISR, add BXVD_Open setting for
 * watchdog timer value, pass WD info to FW:
 *
 * Hydra_Software_Devel/80   12/23/05 10:01a pblanco
 * PR18797: Fixed uninitialized variable issues.
 *
 * Hydra_Software_Devel/79   12/22/05 1:55p davidp
 * PR16052: Update CDB/ITB config table:
 *
 * Hydra_Software_Devel/78   12/22/05 1:28p pblanco
 * PR16052: Removed dead code in BXVD_StartDecode.
 *
 * Hydra_Software_Devel/77   12/21/05 5:59p davidp
 * PR16052: Add Still picture support:
 *
 * Hydra_Software_Devel/76   12/20/05 8:22a pblanco
 * PR18318: XVD now passes the live/playback flag to FW. Now DM has to
 * pick it out of the BXVD_DecodeSettings structure.
 *
 * Hydra_Software_Devel/75   12/15/05 6:06p davidp
 * PR16052: Support for hold last picture on channel change:
 *
 * Hydra_Software_Devel/74   12/13/05 2:57p pblanco
 * PR16052: Removed code after David E's review.
 *
 * Hydra_Software_Devel/73   12/12/05 8:19p davidp
 * PR16052: Add PTS related callbacks:
 *
 * Hydra_Software_Devel/72   12/12/05 3:09p pblanco
 * PR16052: More still picture internals (sanity checkin)
 *
 * Hydra_Software_Devel/71   12/9/05 1:31p pblanco
 * PR16052: Added still picture API skeleton.
 *
 * Hydra_Software_Devel/69   12/7/05 10:36a pblanco
 * PR16052: Implemented BXVD_GetDisplayThresholds and
 * BXVD_SetDiscardThreshold functions.
 *
 * Hydra_Software_Devel/68   12/2/05 4:06p pblanco
 * PR16052: Implemented new API functions. Some need further development.
 *
 * Hydra_Software_Devel/67   12/2/05 9:57a pblanco
 * PR16052: Moved BXVD_ReleasePPB to private API.
 *
 * Hydra_Software_Devel/66   11/22/05 6:29p davidp
 * PR17568: Add CrcMode setting to BXVD_StartDecode routine:
 *
 * Hydra_Software_Devel/65   11/8/05 1:42p davidp
 * PR17935: Update decoder register definitions to match latest RDB:
 *
 * Hydra_Software_Devel/64   11/4/05 3:53p davidp
 * PR16052: Add call to BXVD_P_ReleasePPBonCloseChannel to StopDecode:
 *
 * Hydra_Software_Devel/63   11/2/05 8:19a pblanco
 * PR16052: Removed some local register definitions and replaced them with
 * the appropriate rdb definitions. Also did some cleanup by removing
 * dead definitions and dead code.
 *
 * Hydra_Software_Devel/62   10/28/05 3:16p davidp
 * PR17678: BXVD_GetPTS, return RunningPTS instead of currentPTS:
 *
 * Hydra_Software_Devel/61   10/25/05 7:57p davidp
 * PR16052: Fix compiler warnings and remove <CR> from report message
 * strings:
 *
 * Hydra_Software_Devel/60   10/21/05 5:38p davidp
 * PR17706: Set release PPB argument for VDEC channel close command based
 * on channel change mode.:
 *
 * Hydra_Software_Devel/59   10/20/05 3:41p syang
 * PR 12762: add setting of hSurface to NULL before BAVC_MFD_Picture is
 * passed to VDC
 *
 * Hydra_Software_Devel/58   10/17/05 7:43p davidp
 * PR17380: Change CDB/ITB config values to recommended values supplied by
 * FW team:
 *
 * Hydra_Software_Devel/57   10/14/05 6:47p ssavekar
 * PR 16052: New check-in for XVD
 * 1.Used debug messages to print debug messages from
 * "BXVD_P_PictureDataReady_isr()".
 * 2.Added profiling code for printing out the time needed to execute DM +
 * VDC in "BXVD_P_PictureDataReady_isr()".
 * 3.Added profiling code for tracking PPBs in and out of DM.
 *
 * Hydra_Software_Devel/57   10/14/05 6:24p ssavekar
 * PR 16052: New check-in for XVD
 * 1.Used debug messages to print debug messages from
 * "BXVD_P_PictureDataReady_isr()".
 * 2.Added profiling code for printing out the time needed to execute DM +
 * VDC in "BXVD_P_PictureDataReady_isr()".
 * 3.Added profiling code for tracking PPBs in and out of DM.
 *
 * Hydra_Software_Devel/56   10/12/05 10:46a pblanco
 * PR17052: Changed alignment for CDB/ITBs returned.
 *
 * Hydra_Software_Devel/55   10/11/05 9:27a pblanco
 * PR16052: Have GetChannelDefaultSettings copy defaults from a pre-
 * initialized structure.
 *
 * Hydra_Software_Devel/53   10/4/05 9:48a pblanco
 * PR16052: Changed printfs to BDBG_MGS calls.
 *
 * Hydra_Software_Devel/52   10/3/05 6:00p davidp
 * PR17381: Set display order to Display in FW channel open command.:
 *
 * Hydra_Software_Devel/51   10/3/05 1:11p pblanco
 * PR16052: Implemented BXVD_ReleasePPB.
 *
 * Hydra_Software_Devel/50   10/3/05 7:41a pblanco
 * PR17379: Changed video format and monitor refresh rate to sane default
 * values. Removed channel array NULL test and added some BDBG_MSGs.
 *
 * Hydra_Software_Devel/49   9/30/05 3:38p pblanco
 * PR17379: Make sure we don't attempt to dereference the channel array if
 * BXVD_OpenChannel hasn't been called yet.
 *
 * Hydra_Software_Devel/48   9/30/05 9:38a pblanco
 * PR17380: Added BXVD_GetBufferConfig API modeled after the Rapr tor
 * function of the same name.
 *
 * Hydra_Software_Devel/47   9/27/05 11:49a pblanco
 * PR16052: Implemented BXVD_GetIPictureFoundStatus. Requires test.
 *
 * Hydra_Software_Devel/46   9/27/05 11:35a pblanco
 * PR16052: Implemented BXVD_Get/SetChannelChangeMode. Requires testing.
 *
 * Hydra_Software_Devel/45   9/26/05 2:24p pblanco
 * PR16052: Fixed Set/GetVideoDisplayMode and Set/GetVideoOffset to
 * conform to DM's expectations. Added RUL descriptor mask to FW channel
 * open call.
 *
 * Hydra_Software_Devel/44   9/23/05 6:22a pblanco
 * PR16052: Changed output interrupt field to false so we don't interrupt
 * on each picture.
 *
 * Hydra_Software_Devel/43   9/22/05 1:21p pblanco
 * PR16052: Pass the user specified protocol to the host command channel
 * open instead of a hardwired value.
 *
 * Hydra_Software_Devel/42   9/20/05 3:52p ssavekar
 * PR 16052: New check-in for XVD
 * Changed function "BXVD_RegisterVdcInterrupt()" to set the newly added
 * field in DM data structure that help in DM in deciding if the parity
 * of next picture to be scanned out is top-field/bottom-
 * field/progressive.
 *
 * Hydra_Software_Devel/41   9/19/05 10:15a pblanco
 * PR16052: Finalized timestamp code after email exchange with Santosh.
 *
 * Hydra_Software_Devel/40   9/19/05 6:42a pblanco
 * PR16052: Added isr versions of GetPTS and GetSTC.
 *
 * Hydra_Software_Devel/39   9/16/05 1:46p pblanco
 * PR16052: Added skeletons for XVD_SetSTC and BXVD_GetSTC APIs.
 *
 * Hydra_Software_Devel/38   9/16/05 8:33a pblanco
 * PR16052: Moved BXVD_PauseDecode, BXVD_ResumeDecode and
 * BXVD_ResetDecoder to private functions in bxvd_priv.c
 *
 * Hydra_Software_Devel/37   9/16/05 7:02a pblanco
 * PR16052: Fixed compile error when built with DEBUG=y.
 *
 * Hydra_Software_Devel/36   9/14/05 1:14p pblanco
 * PR16052: Changed function BXVD_GetDisplayFieldMode to take a
 * BXVD_DisplayFieldType argument.
 *
 * Hydra_Software_Devel/35   9/13/05 1:47p pblanco
 * PR16052: Code cleanup.
 *
 * Hydra_Software_Devel/34   9/13/05 10:35a pblanco
 * PR16052: Added missing fields to BXVD_GetChannelDefaultSettings.
 *
 * Hydra_Software_Devel/33   9/9/05 6:21p davidp
 * PR16052: Cleaned up L3 interrupt handling:
 *
 * Hydra_Software_Devel/32   9/9/05 8:25a pblanco
 * PR16052: Added the guts of BXVD_UnInstallInterruptCallback.
 *
 * Hydra_Software_Devel/30   9/8/05 4:40p ssavekar
 * PR 16052: New check-in for XVD
 * Changes for DM integration with XVD and FW.
 *
 * Hydra_Software_Devel/29   9/6/05 10:23a pblanco
 * PR16052: Added code in BXVD_StartDecodewhich calls a routine to
 * convert physical addresses to virtual address offsets for DM.
 *
 * Hydra_Software_Devel/28   9/2/05 9:37a pblanco
 * PR16052: Fixed compile errors when DEBUG=y.
 *
 * Hydra_Software_Devel/27   9/2/05 9:11a davidp
 * PR16052: Add support for multiple L3 interrupts:
 *
 * Hydra_Software_Devel/26   9/1/05 7:29a pblanco
 * PR16052: Calculate the correct CDB write ptr register value based on
 * values returned by Rave.
 *
 * Hydra_Software_Devel/25   8/30/05 1:35p pblanco
 * PR16052: XVD/Rave integration modifications.
 *
 * Hydra_Software_Devel/24   8/29/05 8:37a pblanco
 * PR16052: Support for new FW.
 *
 * Hydra_Software_Devel/23   8/25/05 5:17p davidp
 * PR16052: Add FW command mailbox interrupt support:
 *
 * Hydra_Software_Devel/22   8/24/05 1:11p pblanco
 * PR16052: Removed extraneous code and debugging information. Added
 * support for multiple AVC instances.
 *
 * Hydra_Software_Devel/21   8/23/05 2:03p pblanco
 * PR16052: Additional host command support, code cleanup and bug fixes.
 *
 * Hydra_Software_Devel/21   8/23/05 1:46p pblanco
 * PR16052: Additional host command support, code cleanup and bug fixes.
 *
 * Hydra_Software_Devel/19   8/18/05 2:12p pblanco
 * PR16052: Code fixes for BXVD_OpenChannel and BXVD_StartDecode.
 *
 * Hydra_Software_Devel/18   8/12/05 7:00a pblanco
 * PR16052: Added CreateEvent for command mailbox and temporarily
 * commented out some host command code.
 *
 * Hydra_Software_Devel/17   8/5/05 1:38a ssavekar
 * PR 16052: New check-in for XVD
 * PR 16052: New check-in for XVD
 * Added code for creating and enabling "DataReadyISR" that in turn calls
 * DM. Also added code for installing a callback for "DataReadyISR". This
 * callback can be registered with a VDC function by app.
 *
 * Hydra_Software_Devel/16   8/4/05 3:18p ebrakus
 * PR16052: Added support forBufferCOnfig
 *
 * Hydra_Software_Devel/15   8/3/05 2:20p ebrakus
 * PR16052: MIN  or cleanup
 *
 * Hydra_Software_Devel/14   8/2/05 9:11a pblanco
 * PR16052: Changed default settings to false for self test and stream
 * type.
 *
 * Hydra_Software_Devel/13   7/26/05 10:30a pblanco
 * PR16052: Added chip reset code.
 *
 * Hydra_Software_Devel/12   7/25/05 3:19p pblanco
 * PR16052: Made code changes suggested by Jason in code review.
 *
 * Hydra_Software_Devel/11   7/25/05 12:25p ebrakus
 * PR16052: modifications to use Avc contexts
 *
 * Hydra_Software_Devel/10   7/22/05 11:36a pblanco
 * PR16052: Code cleanup and data structure factoring.
 *
 * Hydra_Software_Devel/9   7/21/05 3:04p pblanco
 * PR16052: Added firmware init code.
 *
 * Hydra_Software_Devel/8   7/20/05 3:16p pblanco
 * PR16052: Additional development.
 *
 * Hydra_Software_Devel/7   7/19/05 3:08p pblanco
 * PR16052: Additional development.
 *
 * Hydra_Software_Devel/6   7/18/05 3:14p pblanco
 * PR16052: Additional code development.
 *
 * Hydra_Software_Devel/5   7/15/05 1:07p pblanco
 * PR16052: Clean build with new code and data.
 *
 * Hydra_Software_Devel/3   7/6/05 1:37p pblanco
 * PR16052: Buildable but mostly non-functional version of API. Note that
 * bxvd_temp_defs.h will disappear in the near future.
 *
 * Hydra_Software_Devel/2   7/1/05 9:11a pblanco
 * PR16052: Some code cleanup
 *
 * Hydra_Software_Devel/1   6/30/05 1:27p pblanco
 * PR16052: Initial checkin
 *
 * Hydra_Software_Devel/1   6/30/05 12:05p pblanco
 * PR16052: Initial checkin
 *
 *
 ***************************************************************************/
#include "bstd.h"                /* standard types */
#include "bdbg.h"                /* Dbglib */
#include "bkni.h"                /* malloc */
#include "bfmt.h"
#include "bxvd.h"
#include "bxvd_platform.h"
#include "bavc.h"
#include "bxvd_vdec_info.h"
#include "bxvd_ppb.h"
#include "bxvd_priv.h"
#include "bxvd_reg.h"
#include "btmr.h"
#include "bxvd_pvr.h"
#include "bxvd_intr.h"
#include "bxvd_image.h"
#include "bchp_common.h"
#include "bxvd_status.h"
#include "bxvd_decoder.h"

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#if DEBUG_STILL_PICTURE
#include <stdio.h>
#endif

#include "bxvd_dbg.h"

#include "bxdm_dih.h"
#include "bxvd_dip.h"
#include "bxdm_pp.h"

BDBG_MODULE(BXVD);

/* This is temporary until defined in chp */
#ifndef BCHP_AVD0_INTR2_CPU_STATUS_AVD_ARC_INTR_SHIFT
#define BCHP_AVD0_INTR2_CPU_STATUS_AVD_ARC_INTR_SHIFT 2
#endif

static const BXVD_DecodeMode s_stDefaultDecodeModes[] =
{
   BXVD_DecodeMode_eAVC_HD,
   BXVD_DecodeMode_eAVC_SD,
   BXVD_DecodeMode_eMPEG2_HD,
   BXVD_DecodeMode_eMPEG2_SD,
   BXVD_DecodeMode_eVC1_HD,
   BXVD_DecodeMode_eVC1_SD,
   BXVD_DecodeMode_eStill_HD,
   BXVD_DecodeMode_eStill_SD,
   BXVD_DecodeMode_eTerminateList
};


/* Default settings. */
static const BXVD_Settings s_stDefaultSettings =
{
   false,                     /* Depreciated - run internal FW memory diagnostic */
   NULL,                      /* Depreciated - TimerDev */
   BXVD_P_WATCHDOG_TIMEOUT,   /* Depreciated - Watchdog timeout Value, approx .75 sec */
   (BXVD_DecodeMode*)(&(s_stDefaultDecodeModes[0])),
   0,                         /* Decoder instance (always 0 for 7401, 7118) */
   BXVD_RaveEndianess_eBig,
   NULL,                      /* Framework general heap, must be assigned by application on 7400 */
   NULL,                      /* Secure memory heap, optionally assigned by application */

   NULL,                      /* Set the BIMG interface to NULL by default */
   NULL,                      /* Set the BIMG context to NULL by default */

   NULL,                      /* Set the AVD Boot Callback to NULL by default */
   NULL,                      /* Set the AVD Boot Callback data pointer to NULL by default */

   NULL,                      /* Set the AVD Reset Callback to NULL by default */
   NULL,                      /* Set the AVD Reset Callback data pointer to NULL by default */
#ifdef BXVD_ENABLE_DVD_API
   BXVD_DisplayMgrMode_eUOD1, /* Set Display Manager mode to UOD 1.x (DVD Build)*/
#else
   BXVD_DisplayMgrMode_eSTB,  /* Set Display Manager mode to STB (non-DVD Build)*/
#endif
   {0,0,0},                   /* Custom FW video memory configuration */
   0,                         /* Suggested size: 16 kb decoder debug log buffer size, 0 is default */
   false,                     /* SVC Inter Layer Bandwidth Optimized, disabled by default */ 
   0                          /* Number of channel contexts to pre-allocate */ 
};

static const  BAVC_VideoCompressionStd  VideoCmprStdList_H264[] =
{
   BAVC_VideoCompressionStd_eH264
};

static const  BAVC_VideoCompressionStd  VideoCmprStdList_MPEG2[] =
{
   BAVC_VideoCompressionStd_eMPEG2
};

static const  BAVC_VideoCompressionStd  VideoCmprStdList_VC1[] =
{
   BAVC_VideoCompressionStd_eVC1
};

/* Default channel settings */
static const BXVD_ChannelSettings s_stDefaultChannelSettings =
{
   BFMT_VERT_59_94Hz,
   0, /* PR20202: Initial removal delay */
   10, /* PR19284: Lip sync fix */
   BAVC_FrameRateCode_eUnknown, /* PR27373: User-settable default frame rate if unknown from stream */
   BXVD_DecodeResolution_eHD,
   (BAVC_VideoCompressionStd *)&VideoCmprStdList_H264,
   1,                          /* DisplayInterrupt */
   0,                          /* VDCRectangleNum */
   BXVD_DisplayInterrupt_eZero,
   BXVD_ChannelMode_eVideo,
   true,
   false,
   false,
   false,
   false,
   false,
   false,
   false,
   NULL,
   NULL,
   NULL,
   BXVD_1080pScanMode_eDefault,
   BXVD_PictureDropMode_eFrame            /* use frame drops */
};

#define BXVD_DECODESETTINGS_SIGNATURE INT32_C (0xeeeea1a1)

/* Default channel settings */
static const BXVD_DecodeSettings s_stDefaultDecodeSettings =
{
   BAVC_VideoCompressionStd_eMPEG2,       /* Compression Standard */
   (uint32_t)NULL,                        /* Deprecated, MultiStreamId */
   (uint32_t)NULL,                        /* Deprecated, VideoSubStreamId */
   false,                                 /* Playback boolean */
   false,                                 /* CRCMode boolean */
   {0,0},                                 /* Deprecated, BAVC_XptOutput */
   0,                                     /* Deprecated, TimeBase */
   NULL,                                  /* Xpt ContextMap, must be specified */
   {NULL},                                /* Xpt ContextMap Extended, Optional */
   0,                                     /* Xpt ContextMap Extended Number, Optional */
   BXVD_DECODESETTINGS_SIGNATURE,         /* Signature, indicates BXVD_GetDecodeDefaultSettings
                                             being called */
   BXVD_STC_eZero,                        /* STC number, is STC0 for instance 0 and STC1 for instance 1 */
   BXVD_DisplayInterrupt_eZero,
   BXVD_DECODE_MODE_RECT_NUM_INVALID,     /* Rectangle Number, use channel number if invalid */
   false,                                 /* Disable DM ASTM mode */
   true,                                  /* Disable DM bVsyncModeOnPcrDiscontinuity */
   BXVD_HITSMode_eDisabled,               /* Disable MPEG HITS Mode */
   false,                                 /* Disable H264 Zero Delay Output Mode */
   BAVC_FrameRateCode_eUnknown,           /* FrameRate */
   0,                                     /* preroll rate of '0' */
   BXVD_ErrorHandling_eOff,               /* picture error handling is off */
   BXVD_FrameRateDetectionMode_eOff,      /* Frame Rate Detection (FRD) is off */
   false,                                 /* Disable Blu-ray decode mode */
   BXVD_ProgressiveOverrideMode_eTopBottom, /* TopBottom 480p/576p/1080p progressive override */
   BXVD_TimestampMode_eDecode,            /* timestamps are expected in display order */
   false,                                 /* Don't treat I Frame as RAP for AVC decode */
   false,                                 /* AVC error concealment disabled by default */
   false,                                 /* AVC I only field output mode, disabled by default */
   false,                                 /* SW3556-1058:: conditionally ignore the DPB output delay syntax */
   0,                                     /* SEI Message Flags */
   false,                                 /* P frame skip mode enabled */ 
   false,                                 /* External picture provider interface NOT in use */
   false,                                 /* Disable AVC Aspect Ratio Override mode */
   false,                                 /* Disable 3D decode for SVC protocol */
   false,                                 /* Disable SW Coefficient decode of AVC streams */
   false,                                 /* Ignore AVC Num Reorder Frames equal zero */
   false,                                 /* Disable Early Picture Delivery Mode */
   NULL,                                  /* SW7425-1064: with linked channels, BXVD_StartDecode will be called 
                                           * once for both channel, hence the need to link the decode settings. */
   false                                  /* Disable Userdata in BTP mode */
};

/* Default device VDC interrupt settings */
static const BXVD_DeviceVdcInterruptSettings  s_stDefaultDevVdcIntrSettings =
{
   0,0,0,                                   /* Top/Bot/Frame field masks */
   BXVD_DisplayInterrupt_eZero,
   BXVD_DeviceVdcIntrSettingsFlags_None,
   0     /* To be removed */
};

/* Static array defining CDB and ITB sizes for various
   Buffer Configuration Modes */
static const BAVC_CdbItbConfig sCdbItbCfg[] =
{
   { {          0, 0, false }, {          0, 0, false }, false },  /*  BXVD_DecodeMode_eTerminateList  */
   { { 0x00180000, 8, false }, { 256 * 1024, 8, false }, false },  /*  BXVD_DecodeMode_eAll            */
   { { 0x00180000, 8, false }, { 256 * 1024, 8, false }, false },  /*  BXVD_DecodeMode_eAVC_HD         */
   { { 0x00100000, 8, false }, { 128 * 1024, 8, false }, false },  /*  BXVD_DecodeMode_eAVC_SD         */
   { { 0x00180000, 8, false }, { 512 * 1024, 8, false }, false },  /*  BXVD_DecodeMode_eMPEG2_HD       */
   { { 0x00100000, 8, false }, { 128 * 1024, 8, false }, false },  /*  BXVD_DecodeMode_eMPEG2_SD       */
   { { 0x00180000, 8, false }, { 256 * 1024, 8, false }, false },  /*  BXVD_DecodeMode_eVC1_HD         */
   { { 0x00100000, 8, false }, { 128 * 1024, 8, false }, false },  /*  BXVD_DecodeMode_eVC1_SD         */
   { { 0x00000000, 8, false }, {          0, 8, false }, false },  /*  BXVD_DecodeMode_eStill_HD       */
   { { 0x00000000, 8, false }, {          0, 8, false }, false },  /*  BXVD_DecodeMode_eStill_SD       */
   { { 0x00000000, 8, false }, {          0, 8, false }, false }   /*  BXVD_DecodeMode_eCustom         */
};

#if BDBG_DEBUG_BUILD
static const char* sDecodeModeNameLUT[BXVD_DecodeMode_eMaxModes] =
{
   "BXVD_DecodeMode_eTerminateList",
   "BXVD_DecodeMode_eAll",
   "BXVD_DecodeMode_eAVC_HD",
   "BXVD_DecodeMode_eAVC_SD",
   "BXVD_DecodeMode_eMPEG2_HD",
   "BXVD_DecodeMode_eMPEG2_SD",
   "BXVD_DecodeMode_eVC1_HD",
   "BXVD_DecodeMode_eVC1_SD",
   "BXVD_DecodeMode_eStill_HD",
   "BXVD_DecodeMode_eStill_SD",
   "BXVD_DecodeMode_eCustom"
};

static const char* sRaveEndianessNameLUT[BXVD_RaveEndianess_eMaxValue] =
{
   "BXVD_RaveEndianess_eBig",
   "BXVD_RaveEndianess_eLittle"
};

static const char* sDisplayMgrModeNameLUT[BXVD_DisplayMgrMode_eMaxModes] =
{
   "BXVD_DisplayMgrMode_eSTB",
};

static const char* sInstanceNameLUT[BXVD_MAX_INSTANCE_COUNT] =
{
   "[00]",
#if BXVD_MAX_INSTANCE_COUNT > 1
   "[01]",
#endif
};

static const char* sChannelNameLUT[BXVD_MAX_INSTANCE_COUNT][BXVD_MAX_VIDEO_CHANNELS] =
{
   {
      "[00][00]",
      "[00][01]",
      "[00][02]",
      "[00][03]",
      "[00][04]",
      "[00][05]",
      "[00][06]",
      "[00][07]",
      "[00][08]",
      "[00][09]",
      "[00][10]",
      "[00][11]",
      "[00][12]",
      "[00][13]",
      "[00][14]",
      "[00][15]",
   },
#if BXVD_MAX_INSTANCE_COUNT > 1
   {
      "[01][00]",
      "[01][01]",
      "[01][02]",
      "[01][03]",
      "[01][04]",
      "[01][05]",
      "[01][06]",
      "[01][07]",
      "[01][08]",
      "[01][09]",
      "[01][10]",
      "[01][11]",
      "[01][12]",
      "[01][13]",
      "[01][14]",
      "[01][15]",
   },
#endif
};
#endif

/***************************************************************************
BXVD_Open: Opens and initializes XVD
****************************************a**********************************/
BERR_Code BXVD_Open(BXVD_Handle         *phXvd,
                    BCHP_Handle         hChip,
                    BREG_Handle         hRegister,
                    BMEM_Handle         hFWMemory,
                    BMEM_Handle         hMemory,
                    BINT_Handle         hInterrupt,
                    const BXVD_Settings *pDefSettings)
{
   BERR_Code rc;
   BXVD_P_Context *pXvd = (BXVD_P_Context*)NULL;

   uint32_t uiDecodeModeNum;
   BXVD_P_MemCfgMode eMemCfgMode;
   BTMR_Settings tmrSettings;
   BDBG_Level eDefaultDebugLevel;

#if BDBG_DEBUG_BUILD
   uint32_t uiGISBMask;
#endif

   BDBG_ENTER(BXVD_Open);

#if BXVD_PPB_EXTENDED   
   /* This tests to see if private PPB crc element is at same offset as public PPB crc element */
   BDBG_CASSERT(offsetof(BXVD_P_PPB, crc) == offsetof(BXVD_PPB, crc));
#endif

   BDBG_ASSERT(phXvd);
   BDBG_ASSERT(hChip);
   BDBG_ASSERT(hRegister);
   BDBG_ASSERT(hMemory);
   BDBG_ASSERT(hInterrupt);

   /* Set handle to NULL in case the allocation fails */
   *phXvd = NULL;

   BXVD_P_VALIDATE_PDEFSETTINGS(pDefSettings);

   pXvd = (BXVD_P_Context*)(BKNI_Malloc(sizeof(BXVD_P_Context)));
   if (!pXvd)
   {
      return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
   }

   BDBG_REGISTER_INSTANCE(pXvd);
   rc = BDBG_GetLevel(&eDefaultDebugLevel);
   rc = BDBG_SetInstanceLevel(pXvd, eDefaultDebugLevel);

   /* Zero out the newly allocated context */
   BKNI_Memset((void*)pXvd, BXVD_P_MEM_ZERO, sizeof(BXVD_P_Context));

   /* Set the defaults */
   if (pDefSettings)
   {
      BKNI_Memcpy((void *)&pXvd->stSettings,
                  (void *)pDefSettings,
                  sizeof(BXVD_Settings));

      /* We need to copy the list of decode modes because the pointer
       * we get may be from the application's stack which will not be
       * persistent */
      /* Check if list is NULL */
      if (pDefSettings->pDecodeModes != NULL) {
         /* First find out how large of an array to allocate */
         uiDecodeModeNum = 0;
         while (pDefSettings->pDecodeModes[uiDecodeModeNum] != BXVD_DecodeMode_eTerminateList) {
            uiDecodeModeNum++;
         }
         uiDecodeModeNum++;
         /* Allocate the memory */
         pXvd->stSettings.pDecodeModes = (BXVD_DecodeMode*) BKNI_Malloc(uiDecodeModeNum * sizeof(BXVD_DecodeMode));
         if (!pXvd->stSettings.pDecodeModes)
         {
            BXVD_Close(pXvd);
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
         }

         pXvd->bAllocDecodeModeList = true;
         /* Copy the list */
         BKNI_Memcpy(pXvd->stSettings.pDecodeModes,
                     pDefSettings->pDecodeModes,
                     uiDecodeModeNum * sizeof(BXVD_DecodeMode));
      }
   }
   else
   {
      BXVD_GetDefaultSettings(&pXvd->stSettings);
   }

   /* validate instance id */
   if (pXvd->stSettings.uiAVDInstance >= BXVD_MAX_INSTANCE_COUNT)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   rc = BXVD_P_InitFreeChannelList(pXvd);  
   if (BERR_SUCCESS != rc)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
   }

#if BXVD_P_FW_DEBUG_DRAM_LOGGING
   if (pXvd->stSettings.uiDecoderDebugLogBufferSize == 0)
   {
      pXvd->stSettings.uiDecoderDebugLogBufferSize = 16*1024; /* Log size in bytes */
   }
#endif
   /* Set the decoder instance here. This will always be 0 for single decoder systems */
   pXvd->uDecoderInstance = pXvd->stSettings.uiAVDInstance;

   rc = BDBG_SetInstanceName(pXvd, sInstanceNameLUT[pXvd->uDecoderInstance]);

   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - phXvd = 0x%08x", phXvd));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - hChip = 0x%08x", hChip));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - hRegister = 0x%08x", hRegister));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - hFWMemory = 0x%08x", hFWMemory));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - hMemory = 0x%08x", hMemory));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - hInterrupt = 0x%08x", hInterrupt));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - pDefSettings = 0x%08x", pDefSettings));

#if BDBG_DEBUG_BUILD
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.bSelfTest = %d (deprecated)", pXvd->stSettings.bSelfTest));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.hTimerDev = 0x%08x (deprecated)", pXvd->stSettings.hTimerDev));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.ulWatchdogTimeValue = 0x$08x (deprecated)", pXvd->stSettings.ulWatchdogTimeValue));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.pDecodeModes = 0x%08x", pXvd->stSettings.pDecodeModes));
   if (pXvd->stSettings.pDecodeModes)
   {
      uiDecodeModeNum = 0;
      do
      {
         if (pXvd->stSettings.pDecodeModes[uiDecodeModeNum] < BXVD_DecodeMode_eMaxModes)
         {
            BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.pDecodeModes[%d] = %s (%d)", uiDecodeModeNum,
                                     sDecodeModeNameLUT[pXvd->stSettings.pDecodeModes[uiDecodeModeNum]],
                                     pXvd->stSettings.pDecodeModes[uiDecodeModeNum]));
         }
         else
         {
            BXVD_DBG_WRN(pXvd, ("BXVD_Open() - BXVD_Settings.pDecodeModes[%d] = %s (%d)", uiDecodeModeNum,
                                     "Unknown/Invalid Value!",
                                     pXvd->stSettings.pDecodeModes[uiDecodeModeNum]));
         }
         uiDecodeModeNum++;
      } while (pXvd->stSettings.pDecodeModes[uiDecodeModeNum] != BXVD_DecodeMode_eTerminateList);
   }
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.uiAVDInstance = %d", pXvd->stSettings.uiAVDInstance));

   if (pXvd->stSettings.eRaveEndianess < BXVD_RaveEndianess_eMaxValue)
   {
      BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.eRaveEndianess = %s (%d)",
                               sRaveEndianessNameLUT[pXvd->stSettings.eRaveEndianess],
                               pXvd->stSettings.eRaveEndianess));
   }
   else
   {
      BXVD_DBG_WRN(pXvd, ("BXVD_Open() - BXVD_Settings.eRaveEndianess = %s (%d)",
                               "Unknown/Invalid Value!",
                               pXvd->stSettings.eRaveEndianess));
   }

   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.hFrmMemory = 0x%08x", pXvd->stSettings.hFrmMemory));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.hSecureMemory = 0x%08x", pXvd->stSettings.hSecureMemory));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.pImgInterface = 0x%08x", pXvd->stSettings.pImgInterface));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.pImgContext = 0x%08x", pXvd->stSettings.pImgContext));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.pAVDBootCallback = 0x%08x", pXvd->stSettings.pAVDBootCallback));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.pAVDBootCallbackData = 0x%08x", pXvd->stSettings.pAVDBootCallbackData));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.pAVDResetCallback = 0x%08x", pXvd->stSettings.pAVDResetCallback));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.pAVDResetCallbackData = 0x%08x", pXvd->stSettings.pAVDResetCallbackData));

   if (pXvd->stSettings.eDisplayMgrMode < BXVD_DisplayMgrMode_eMaxModes)
   {
      BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.eDisplayMgrMode = %s (%d)",
                               sDisplayMgrModeNameLUT[pXvd->stSettings.eDisplayMgrMode],
                               pXvd->stSettings.eDisplayMgrMode));
   }
   else
   {
      BXVD_DBG_WRN(pXvd, ("BXVD_Open() - BXVD_Settings.eDisplayMgrMode = %s (%d)",
                               "Unknown/Invalid Value!",
                               pXvd->stSettings.eDisplayMgrMode));
   }

   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.stFWMemConfig.uiGeneralHeapSize = 0x%08x", pXvd->stSettings.stFWMemConfig.uiGeneralHeapSize));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.stFWMemConfig.uiCabacHeapSize = 0x%08x", pXvd->stSettings.stFWMemConfig.uiCabacHeapSize));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.stFWMemConfig.uiPictureHeapSize = 0x%08x", pXvd->stSettings.stFWMemConfig.uiPictureHeapSize));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - BXVD_Settings.uiDecoderDebugLogBufferSize = 0x%08x", pXvd->stSettings.uiDecoderDebugLogBufferSize));
#endif

   /* Each platform has a mask of supported decode protocols, used to validate in BXVD_Startdecode. */
   pXvd->uiSupportedProtocolsMask = BXVD_P_PLATFORM_SUPPORTED_PROTOCOLS;

   /* Create timer */
   if (NULL != pXvd->stSettings.hTimerDev)
   {
      rc = BTMR_GetDefaultTimerSettings(&tmrSettings);

      tmrSettings.type = BTMR_Type_eSharedFreeRun;
      tmrSettings.exclusive = false;
      
      if (BERR_SUCCESS == rc)
      {
         rc = BTMR_CreateTimer(pXvd->stSettings.hTimerDev,
                               &pXvd->hTimer,
                               &tmrSettings);

         if (BERR_SUCCESS != rc)
         {
            pXvd->hTimer = NULL;
            BXVD_DBG_WRN(pXvd, ("Error creating timer"));
         }

      }
   }

   /* Store handles in the context */
   pXvd->hChip = hChip;
   pXvd->hInterrupt = hInterrupt;
   pXvd->hReg = hRegister;

   /* Set handle type */
   pXvd->eHandleType = BXVD_P_HandleType_XvdMain;

   /* Set the heaps */
   pXvd->hPictureHeap = hMemory;

   if (pXvd->stSettings.hFrmMemory)
   {
      pXvd->hSystemHeap = pXvd->stSettings.hFrmMemory;
   }
   else
   {
      pXvd->hSystemHeap = hMemory; /* System heap not specified, use hMemory */
   }

   if (hFWMemory)
   {
      pXvd->hCodeHeap = hFWMemory;
   }
   else
   {
      pXvd->hCodeHeap = pXvd->hSystemHeap;
   }

   if (pXvd->stSettings.hSecureMemory)
   {
      pXvd->hCabacHeap = pXvd->stSettings.hSecureMemory;
   }
   else
   {
      pXvd->hCabacHeap = pXvd->hSystemHeap; /* Secure heap not specified, use general application heap */
   }

   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - hPictureHeap = 0x%08x", pXvd->hPictureHeap));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - hSystemHeap = 0x%08x", pXvd->hSystemHeap));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - hCodeHeap = 0x%08x", pXvd->hCodeHeap));
   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - hCabacHeap = 0x%08x", pXvd->hCabacHeap));


#if !(BXVD_USE_CUSTOM_IMAGE)
   if (pXvd->stSettings.pImgInterface != &BXVD_IMAGE_Interface)
   {
      BXVD_DBG_WRN(pXvd, ("*******************"));
      BXVD_DBG_WRN(pXvd, ("You've linked in the default XVD BIMG interface and context."));
      BXVD_DBG_WRN(pXvd, ("However, you are providing your own version(s) to BXVD_Open()."));
      BXVD_DBG_WRN(pXvd, ("You should compile with BXVD_USE_CUSTOM_IMAGE=1 to prevent linkage"));
      BXVD_DBG_WRN(pXvd, ("of the default BIMG interface and context to reduce the binary size"));
      BXVD_DBG_WRN(pXvd, ("*******************"));
   }

#if !(BXVD_USE_CUSTOM_CONTEXT)
   if (pXvd->stSettings.pImgContext != BXVD_IMAGE_Context)
   {
      BXVD_DBG_WRN(pXvd, ("*******************"));
      BXVD_DBG_WRN(pXvd, ("You've linked in the default XVD BIMG context."));
      BXVD_DBG_WRN(pXvd, ("However, you are providing your own version to BXVD_Open()."));
      BXVD_DBG_WRN(pXvd, ("You should compile with BXVD_USE_CUSTOM_CONTEXT=1 to prevent linkage"));
      BXVD_DBG_WRN(pXvd, ("of the default BIMG context to reduce the binary size"));
      BXVD_DBG_WRN(pXvd, ("*******************"));
   }
#endif
#endif

   if ((pXvd->stSettings.pImgInterface == NULL) ||
       (pXvd->stSettings.pImgContext == NULL)) {
      BXVD_DBG_ERR(pXvd, ("*******************"));
      BXVD_DBG_ERR(pXvd, ("You've compiled with either BXVD_USE_CUSTOM_IMAGE=1 or BXVD_USE_CUSTOM_CONTEXT=1."));
      BXVD_DBG_ERR(pXvd, ("However, you have NOT provided your own version(s) of"));
      BXVD_DBG_ERR(pXvd, ("the BIMG interface and context to BXVD_Open()."));
      BXVD_DBG_ERR(pXvd, ("If you want to use the default BIMG, use BXVD_USE_CUSTOM_IMAGE=0 or BXVD_USE_CUSTOM_CONTEXT=0"));
      BXVD_DBG_ERR(pXvd, ("Otherwise, you MUST provide your own implementation of BIMG."));
      BXVD_DBG_ERR(pXvd, ("*******************"));
   }

   /* Set pointer back to parent structure */
   pXvd->stDecoderContext.hXvd = pXvd;

   /* Allocate the channel handle array */
   pXvd->ahChannel = (BXVD_ChannelHandle*)(BKNI_Malloc(sizeof(BXVD_ChannelHandle) * BXVD_MAX_VIDEO_CHANNELS));

   if (!pXvd->ahChannel)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
   }

   BKNI_Memset((void*)pXvd->ahChannel, 0x0, (sizeof(BXVD_ChannelHandle) * BXVD_MAX_VIDEO_CHANNELS));

   pXvd->hStillChannel = (BXVD_P_Channel*)(BKNI_Malloc(sizeof(BXVD_P_Channel)));

   if (!pXvd->hStillChannel)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
   }

   /* Allocate the multi-decode picture list */
   pXvd->pVDCPictureBuffers = (BAVC_XVD_Picture*)(BKNI_Malloc(sizeof(BAVC_XVD_Picture) * BXVD_MAX_VIDEO_CHANNELS));

   if (!pXvd->pVDCPictureBuffers)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
   }

   BKNI_Memset((void*)pXvd->pVDCPictureBuffers, 0x0, (sizeof(BAVC_XVD_Picture) * BXVD_MAX_VIDEO_CHANNELS));

   rc = BERR_TRACE(BKNI_CreateEvent(&(pXvd->stDecoderContext.hFWCmdDoneEvent)));
   if (rc != BERR_SUCCESS)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(rc);
   }

   /*
    * Initialize stripe width and bank height prior to calling
    * platform specific register initialization and AVD core
    * boot. These are currently set to hardcoded values for all
    * platforms except the 7440 and 7405. These two platforms will
    * set the values in their specific register initialization code.
    */
   pXvd->uiDecode_StripeWidth = BXVD_P_AVD_INIT_STRIPE_WIDTH;
   pXvd->uiDecode_StripeMultiple = BXVD_P_AVD_INIT_STRIPE_MULTIPLE;

   /*
    * Initialize Chip Product Revision register address to be passed to FW.
    */ 
   pXvd->uiChip_ProductRevision = BXVD_P_CHIP_PRODUCT_REVISION; 

#if BXVD_P_POWER_MANAGEMENT

   /* Use bchp power method if resource is defined */ 
#if BCHP_PWR_RESOURCE_AVD0

   /* Assume power (clocks) is off */
   pXvd->bHibernate = true;
   pXvd->PowerStateCurrent = BXVD_P_PowerState_ePwrOff;

   /* Wake up decoder */
   BXVD_P_SetHibernateState(pXvd, false);

#endif
#endif

   BXVD_P_INIT_REG_PTRS(pXvd);

   BXVD_P_GET_MEMORY_CONFIG(pXvd, eMemCfgMode);

   BXVD_P_ValidateHeaps(pXvd, eMemCfgMode);

#if BXVD_P_POWER_MANAGEMENT
#if !BCHP_PWR_RESOURCE_AVD0
   {
      /* Use legacy power management method, need reg pointers setup to access clock registers */ 
      bool bHibernateState;

      /* XVD should not be in hibernation state, verify the clocks are enabled */

      BXVD_P_GetHibernateState(pXvd, &bHibernateState);

      if (bHibernateState == true)
      {
         BXVD_DBG_WRN(pXvd, ("XVD clocks NOT enabled, clocks being enabled"));

         pXvd->PowerStateCurrent = BXVD_P_PowerState_eClkOff;
         /* Wake up decoder */
         BXVD_P_SetHibernateState(pXvd, false);
      }
      else
      {
         /* Clocks are enabled */
         pXvd->PowerStateCurrent = BXVD_P_PowerState_eOn;
      }
   }
#endif
#endif

   /* Validate the uiDecode_StripeWidth and uiDecode_StripeMultiple
    * values that may have been overridden by the platform's init reg
    * ptr code */
   if (pXvd->uiDecode_StripeWidth >= BXVD_P_STRIPE_WIDTH_NUM)
   {
      BXVD_DBG_ERR(pXvd, ("Unsupported stripe width: %d", pXvd->uiDecode_StripeWidth));
      BXVD_Close(pXvd);
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   if (pXvd->uiDecode_StripeMultiple >= BXVD_P_STRIPE_MULTIPLE_NUM)
   {
      BXVD_DBG_ERR(pXvd, ("Unsupported stripe multiple: %d", pXvd->uiDecode_StripeMultiple));
      BXVD_Close(pXvd);
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

#if BDBG_DEBUG_BUILD
   /* Check to see if AVD has access to the GISB */
   uiGISBMask = BREG_Read32(pXvd->hReg,
                            pXvd->stPlatformInfo.stReg.uiSunGisbArb_ReqMask);

   if (uiGISBMask & pXvd->stPlatformInfo.stReg.uiSunGisbArb_ReqMaskAVDMask)
   {
      BXVD_DBG_ERR(pXvd, ("***ERROR*** AVD access to GISB DISABLE! GISB Arbiter Request Mask: %08x", uiGISBMask));
   }
#endif
 
   /* Initialize compatibility FWMemConfig tables */
   rc = BXVD_P_InitializeFWMemConfigCompatibilityModeTable(pXvd);
   if (rc != BERR_SUCCESS)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(rc);
   }

   /* Initialize memory */
   rc = BXVD_P_SETUP_FW_MEMORY(pXvd);
   if (rc != BERR_SUCCESS)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(rc);
   }

   rc = BXVD_P_Boot(pXvd);
   if(rc != BERR_SUCCESS)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(rc);
   }

   {
      BXVD_DisplayInterruptProvider_P_ChannelSettings stXvdDipChSettings;

      BDBG_ASSERT( BXVD_DisplayInterrupt_eMax == 2 );

      /* Setup Display Interrupt #0 */
      rc = BXVD_DisplayInterruptProvider_P_GetDefaultChannelSettings( &stXvdDipChSettings );
      if (rc != BERR_SUCCESS)
      {
         BXVD_Close(pXvd);
         return BERR_TRACE(rc);
      }

      stXvdDipChSettings.hXvd = pXvd;
      stXvdDipChSettings.hInterrupt = pXvd->hInterrupt;
      stXvdDipChSettings.hRegister = pXvd->hReg;
      stXvdDipChSettings.interruptId = (BINT_Id) pXvd->stPlatformInfo.stReg.uiInterrupt_PicDataRdy;
      stXvdDipChSettings.uiInterruptMaskRegister = pXvd->stPlatformInfo.stReg.uiBvnf_Intr2_3_AvdMaskClear;
      stXvdDipChSettings.uiInterruptClearRegister = pXvd->stPlatformInfo.stReg.uiBvnf_Intr2_3_AvdClear,
      stXvdDipChSettings.eDisplayInterrupt = BXVD_DisplayInterrupt_eZero;

      BXVD_P_SAVE_DIP_CHANNEL_DISPLAY_INFO_0(stXvdDipChSettings, pXvd);

      rc = BXVD_DisplayInterruptProvider_P_OpenChannel( &pXvd->hXvdDipCh[BXVD_DisplayInterrupt_eZero], &stXvdDipChSettings);
      if (rc != BERR_SUCCESS)
      {
         BXVD_Close(pXvd);
         return BERR_TRACE(rc);
      }

      rc = BXDM_DisplayInterruptHandler_Create( &pXvd->hXdmDih[BXVD_DisplayInterrupt_eZero] );
      if (rc != BERR_SUCCESS)
      {
         BXVD_Close(pXvd);
         return BERR_TRACE(rc);
      }

      /* Setup Display Interrupt #1 */
      if ( 0 != pXvd->stPlatformInfo.stReg.uiInterrupt_PicDataRdy1 )
      {
         rc = BXVD_DisplayInterruptProvider_P_GetDefaultChannelSettings( &stXvdDipChSettings );
         if (rc != BERR_SUCCESS)
         {
            BXVD_Close(pXvd);
            return BERR_TRACE(rc);
         }

         stXvdDipChSettings.hXvd = pXvd;
         stXvdDipChSettings.hInterrupt = pXvd->hInterrupt;
         stXvdDipChSettings.hRegister = pXvd->hReg;
         stXvdDipChSettings.interruptId = pXvd->stPlatformInfo.stReg.uiInterrupt_PicDataRdy1;
         stXvdDipChSettings.uiInterruptMaskRegister = pXvd->stPlatformInfo.stReg.uiBvnf_Intr2_3_AvdMaskClear;
         stXvdDipChSettings.uiInterruptClearRegister = pXvd->stPlatformInfo.stReg.uiBvnf_Intr2_3_AvdClear,
         stXvdDipChSettings.eDisplayInterrupt = BXVD_DisplayInterrupt_eOne;

         BXVD_P_SAVE_DIP_CHANNEL_DISPLAY_INFO_1(stXvdDipChSettings, pXvd);

         rc = BXVD_DisplayInterruptProvider_P_OpenChannel( &pXvd->hXvdDipCh[BXVD_DisplayInterrupt_eOne], &stXvdDipChSettings);
         if (rc != BERR_SUCCESS)
         {
            BXVD_Close(pXvd);
            return BERR_TRACE(rc);
         }

         rc = BXDM_DisplayInterruptHandler_Create( &pXvd->hXdmDih[BXVD_DisplayInterrupt_eOne] );
         if (rc != BERR_SUCCESS)
         {
            BXVD_Close(pXvd);
            return BERR_TRACE(rc);
         }
      }
   }

   rc = BXVD_P_SetupFWSubHeap(pXvd);
   if(rc != BERR_SUCCESS)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(rc);
   }

   rc = BXVD_Status_Open(pXvd, &pXvd->hXvdStatus);
   if(rc != BERR_SUCCESS)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(rc);
   }

   rc = BXVD_P_SetupStillPictureCompatibilityMode(pXvd);
   if(rc != BERR_SUCCESS)
   {
      BXVD_Close(pXvd);
      return BERR_TRACE(rc);
   }

#if BXVD_P_FW_DEBUG_DRAM_LOGGING
   /* Enable debug logging, with PDR_isr routine reading and printing log */
   BXVD_DBG_ControlDecoderDebugLog(pXvd, BXVD_DBG_DebugLogging_eStart);
#endif

#if BXVD_P_POWER_MANAGEMENT
   /* If a channel is open for Still Picture Compatibility Mode, don't hibernate */
   if ( !pXvd->bStillPictureCompatibilityMode )
   {
      /* Put decoder in hibernate state */
      BXVD_P_SetHibernateState(pXvd, true);
   }
#endif
   /* Get the firmware revision */
   BXVD_GetRevision(pXvd, &(pXvd->sRevisionInfo));
   BXVD_DBG_WRN(pXvd, ("BXVD_Open() - Hardware revision: %c, Firmware revision: %x", BXVD_P_CORE_REVISION, pXvd->sRevisionInfo.ulDecoderFwRev));

   /* Give the user the new context */
   *phXvd = (BXVD_Handle)pXvd;

   BXVD_DBG_MSG(pXvd, ("BXVD_Open() - pXvd = 0x%08x", pXvd));

   BDBG_LEAVE(BXVD_Open);
   return BERR_TRACE(BERR_SUCCESS);
}

/******************************************************************************
BXVD_Close: Release allocated resources and close XVD
******************************************************************************/
BERR_Code BXVD_Close(BXVD_Handle hXvd)
{
   BERR_Code eStatus = BERR_SUCCESS;

   BDBG_ENTER(BXVD_Close);

   BDBG_ASSERT(hXvd);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

#if BXVD_P_POWER_MANAGEMENT
   /* Wake up the decoder */
   BXVD_P_SetHibernateState(hXvd, false);
#endif

   /* We must teardown the still picture channel before we destroy the
    * callbacks because we need to get the response back from the
    * decoder when closing the channel */
   eStatus = BERR_TRACE(BXVD_P_TeardownStillPictureCompatibilityMode(hXvd));
   if (BERR_SUCCESS != eStatus)
   {
      BXVD_P_FreeXVDContext(hXvd);
      return BERR_TRACE(eStatus);
   }

   eStatus = BERR_TRACE(BXVD_P_DestroyInterrupts(hXvd));
   if (BERR_SUCCESS != eStatus)
   {
      BXVD_P_Reset740x(hXvd, (uint32_t)NULL);

      BXVD_P_FreeXVDContext(hXvd);
      return BERR_TRACE(eStatus);
   }

   BXVD_P_Reset740x(hXvd, (uint32_t)NULL);

   eStatus = BERR_TRACE(BXVD_P_TeardownFWSubHeap(hXvd));
   if (BERR_SUCCESS != eStatus)
   {
      BXVD_P_FreeXVDContext(hXvd);
      return BERR_TRACE(eStatus);
   }

   eStatus = BERR_TRACE(BXVD_P_TEAR_DOWN_FW_MEMORY(hXvd));
   if (BERR_SUCCESS != eStatus)
   {
      BXVD_P_FreeXVDContext(hXvd);

      return BERR_TRACE(eStatus);
   }

   if (hXvd->hXvdStatus)
   {
      eStatus = BXVD_Status_Close(hXvd->hXvdStatus);
      if(BERR_SUCCESS != eStatus)
      {
         BXVD_P_FreeXVDContext(hXvd);
         return BERR_TRACE(eStatus);
      }
   }

#if BXVD_P_POWER_MANAGEMENT

   /* Put the decoder to sleep  */
   BXVD_P_SET_POWER_STATE(hXvd, BXVD_P_PowerState_ePwrOff);

#endif

   BDBG_UNREGISTER_INSTANCE(hXvd);

   BXVD_P_FreeXVDContext(hXvd);

   /* Null the handle */
   hXvd = NULL;

   BDBG_LEAVE(BXVD_Close);
   return BERR_TRACE(eStatus);
}


/**************************************************************************
 Summary: BXVD_GetHardwareCapabilties
    Returns the decoder HW video protocol capabilities
 
 Description:
    Using the specified video decoder handle, determine the supported 
    video compression standards. 

 Returns: 
    BERR_SUCCESS:  Hardware capabilities determined successfully.
    BXVD_ERR_INVALID_HANDLE: BXVD_Handle not valid. 
**************************************************************************/

BERR_Code BXVD_GetHardwareCapabilities(BXVD_Handle hXvd,                 
                                       BXVD_HardwareCapabilities *pCap)
{
   BAVC_VideoCompressionStd  eVideoCmprStd;

   BDBG_ENTER(BXVD_GetHardwareCapabilities);

   BDBG_ASSERT(hXvd);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   for (eVideoCmprStd = BAVC_VideoCompressionStd_eH264; eVideoCmprStd <	BAVC_VideoCompressionStd_eMax; eVideoCmprStd++)
   {
      pCap->bCodecSupported[eVideoCmprStd] = BXVD_P_IsDecodeProtocolSupported(hXvd, eVideoCmprStd);
   }

   BDBG_LEAVE(BXVD_GetHardwareCapabilities);
   return BERR_TRACE(BERR_SUCCESS);
}


/******************************************************************************
BXVD_GetDefaultSettings: Get the default settings for the XVD device.
******************************************************************************/
BERR_Code BXVD_GetDefaultSettings(BXVD_Settings *pDefaultSettings)
{
   BDBG_ENTER(BXVD_GetDefaultSettings);

   BDBG_ASSERT(pDefaultSettings);

   BKNI_Memcpy((void*)pDefaultSettings,
               (void *)&s_stDefaultSettings,
               sizeof(BXVD_Settings));

#if !(BXVD_USE_CUSTOM_IMAGE)
   pDefaultSettings->pImgInterface = &BXVD_IMAGE_Interface;
#if !(BXVD_USE_CUSTOM_CONTEXT)
   pDefaultSettings->pImgContext = BXVD_IMAGE_Context;
#endif
#endif

   BDBG_LEAVE(BXVD_GetDefaultSettings);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
BXVD_GetChannelDefaultSettings: Gets the default settings of the desired XVD
                                channel.
****************************************************************************/
BERR_Code BXVD_GetChannelDefaultSettings(BXVD_Handle          hXvd,
                                         unsigned int         uiChannelNum,
                                         BXVD_ChannelSettings *pChnDefSettings)
{
   BXVD_DecodeMode eDecodeMode;

   BDBG_ENTER(BXVD_GetChannelDefaultSettings);
   BSTD_UNUSED(hXvd);

   BDBG_ASSERT(hXvd);
   BSTD_UNUSED(uiChannelNum);
   BDBG_ASSERT(pChnDefSettings);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   BKNI_Memcpy((void *)pChnDefSettings,
               (void *)&s_stDefaultChannelSettings,
               sizeof(BXVD_ChannelSettings));

   /* coverity[read_parm_fld:FALSE] */
   BXVD_DBG_MSG(hXvd, ("pChnDefSettings->ui32MonitorRefreshRate = 0x%x (%d)",
                            pChnDefSettings->ui32MonitorRefreshRate,
                            pChnDefSettings->ui32MonitorRefreshRate));

   /* If application using the legacy single decode XVD API, we need to
    * set the channel's default max resolution and protocol based on
    * decode modes provided at BXVD_Open() */
   BXVD_P_ParseDecodeMode(hXvd, hXvd->stSettings.pDecodeModes, &eDecodeMode);
   switch (eDecodeMode)
   {
      case BXVD_DecodeMode_eAll:
      case BXVD_DecodeMode_eAVC_HD:
         pChnDefSettings->eDecodeResolution = BXVD_DecodeResolution_eHD;
         pChnDefSettings->peVideoCmprStdList = (BAVC_VideoCompressionStd *) &VideoCmprStdList_H264;
         pChnDefSettings->uiVideoCmprCount = 1;
         break;

      case BXVD_DecodeMode_eAVC_SD:
         pChnDefSettings->eDecodeResolution = BXVD_DecodeResolution_eSD;
         pChnDefSettings->peVideoCmprStdList = (BAVC_VideoCompressionStd *) &VideoCmprStdList_H264;
         pChnDefSettings->uiVideoCmprCount = 1;
         break;

      case BXVD_DecodeMode_eMPEG2_HD:
         pChnDefSettings->eDecodeResolution = BXVD_DecodeResolution_eHD;
         pChnDefSettings->peVideoCmprStdList = (BAVC_VideoCompressionStd *) &VideoCmprStdList_MPEG2;
         pChnDefSettings->uiVideoCmprCount = 1;
         break;

      case BXVD_DecodeMode_eMPEG2_SD:
         pChnDefSettings->eDecodeResolution = BXVD_DecodeResolution_eSD;
         pChnDefSettings->peVideoCmprStdList = (BAVC_VideoCompressionStd *) &VideoCmprStdList_MPEG2;
         pChnDefSettings->uiVideoCmprCount = 1;
         break;

      case BXVD_DecodeMode_eVC1_HD:
         pChnDefSettings->eDecodeResolution = BXVD_DecodeResolution_eHD;
         pChnDefSettings->peVideoCmprStdList = (BAVC_VideoCompressionStd *) &VideoCmprStdList_VC1;
         pChnDefSettings->uiVideoCmprCount = 1;
         break;

      case BXVD_DecodeMode_eVC1_SD:
         pChnDefSettings->eDecodeResolution = BXVD_DecodeResolution_eSD;
         pChnDefSettings->peVideoCmprStdList = (BAVC_VideoCompressionStd *) &VideoCmprStdList_VC1;
         pChnDefSettings->uiVideoCmprCount = 1;
         break;

      case BXVD_DecodeMode_eCustom:
         /* Application is using new multi-decode XVD API, so they
          * should set the VideoCmprStdList and eDecodeResolution
          * accordingly for this channel */
         break;

      default:
         BXVD_DBG_ERR(hXvd, ("Invalid decode mode detected"));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   BDBG_LEAVE(BXVD_GetChannelDefaultSettings);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
BXVD_GetDecodeDefaultSettings:  Gets the default settings of the desired
                                Decoder
****************************************************************************/
BERR_Code BXVD_GetDecodeDefaultSettings
(
   BXVD_ChannelHandle   hXvdCh,             /* [in] XVD channel handle */
   BXVD_DecodeSettings  *pDecodeDefSettings /* [out] default channel settings */
   )
{
   BDBG_ENTER(BXVD_GetDecodeDefaultSettings);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pDecodeDefSettings);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   BKNI_Memcpy((void *)pDecodeDefSettings,
               (void *)&s_stDefaultDecodeSettings,
               sizeof(BXVD_DecodeSettings));

   /* AVD0 should use STC0 and AVD1 should use STC1 to maintain
    * backwards compatibility with pre-mosaic firmware */
   if (hXvdCh->pXvd->uDecoderInstance == 0)
   {
      pDecodeDefSettings->eSTC = BXVD_STC_eZero;
   }
   else
   {
      pDecodeDefSettings->eSTC = BXVD_STC_eOne;
   }

   /* VDC Rectangle number defaults to channel number */
   pDecodeDefSettings->uiVDCRectangleNum = hXvdCh->ulChannelNum;

   BDBG_LEAVE(BXVD_GetDecodeDefaultSettings);
   return BERR_TRACE(BERR_SUCCESS);

}

#if BXVD_P_POWER_MANAGEMENT

BERR_Code BXVD_Standby(BXVD_Handle hXvd)
{
   BXVD_ChannelHandle  hXvdCh;
   uint32_t            chanNum;

   BERR_Code rc = BERR_SUCCESS;

   BXVD_DBG_MSG(hXvd, ("BXVD_Standby decoder:%d", hXvd->uDecoderInstance));

   for ( chanNum = 0; chanNum < BXVD_MAX_VIDEO_CHANNELS; chanNum++)
   {
      hXvdCh = hXvd->ahChannel[chanNum];

      if (hXvdCh != NULL)
      {
         if (hXvdCh->eDecoderState == BXVD_P_DecoderState_eActive)
         {
            BXVD_DBG_WRN(hXvdCh, ("BXVD_Standy() - Channel active: %d", chanNum));

            rc = BXVD_ERR_DECODER_ACTIVE;
            break;
         }
      }
   }

   hXvd->PowerStateSaved = hXvd->PowerStateCurrent;

   /* If clocks are off, turn them on to reset decoder */
   if (hXvd->PowerStateCurrent == BXVD_P_PowerState_eClkOff)
   {
      BXVD_P_SET_POWER_STATE(hXvd, BXVD_P_PowerState_eOn);
   }

   /* Halt ARCs and reset decoder */
   rc = BERR_TRACE(BXVD_P_Reset740x(hXvd, hXvd->uDecoderInstance));

   if (rc != BERR_SUCCESS) 
   { 
      return BERR_TRACE(rc);
   }

   BXVD_P_SET_POWER_STATE(hXvd, BXVD_P_PowerState_ePwrOff);

   return rc;
}

BERR_Code BXVD_Resume(BXVD_Handle hXvd)
{

   BERR_Code rc;

   BXVD_DBG_MSG(hXvd, ("BXVD_Resume decoder:%d", hXvd->uDecoderInstance));

   BXVD_P_SET_POWER_STATE(hXvd, BXVD_P_PowerState_eOn);

   rc = BXVD_ProcessWatchdog(hXvd);

   if (hXvd->PowerStateSaved != BXVD_P_PowerState_eOn)
   {
      BXVD_DBG_MSG(hXvd, ("Decoder:%d, Set saved_Power_State:%d\n", hXvd->uDecoderInstance, hXvd->PowerStateSaved));

      BXVD_P_SET_POWER_STATE(hXvd, hXvd->PowerStateSaved);
   }

   return rc;
}

#endif /* BXVD_P_POWER_MANAGEMENT */

BERR_Code BXVD_ProcessWatchdog(BXVD_Handle hXvd)
{
   BXVD_ChannelHandle  hXvdCh;
   uint32_t            chanNum;
   uint32_t            i;
   BAVC_XptContextMap  XptContextMap;
   BAVC_XptContextMap  aXptContextMap_Extended[BXVD_NUM_EXT_RAVE_CONTEXT];
   bool                bStillMode;

   BERR_Code rc;

   BDBG_ASSERT(hXvd);

   BDBG_ENTER(BXVD_ProcessWatchdog);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   for ( chanNum = 0; chanNum < BXVD_MAX_VIDEO_CHANNELS; chanNum++)
   {
      hXvdCh = hXvd->ahChannel[chanNum];

      if (hXvdCh != NULL)
      {
         /* Only reset DM on Video channels, not Still Picture channel */
         if (hXvdCh->sChSettings.eChannelMode == BXVD_ChannelMode_eVideo)
         {
            BKNI_EnterCriticalSection();
            /* Reset Display manager state */
            BXDM_PictureProvider_WatchdogReset_isr(hXvdCh->hPictureProvider);
            BXVD_Decoder_WatchdogReset_isr( hXvdCh );
            BKNI_LeaveCriticalSection();
         }
      }
   }

   rc = BXVD_P_Boot(hXvd);
   if(rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   {
      BXVD_DisplayInterrupt eDisplayInterrupt;
      for ( eDisplayInterrupt = 0; eDisplayInterrupt < BXVD_DisplayInterrupt_eMax; eDisplayInterrupt++ )
      {
         rc = BXVD_DisplayInterruptProvider_P_ProcessWatchdog( hXvd->hXvdDipCh[eDisplayInterrupt] );
         if(rc != BERR_SUCCESS)
         {
            return BERR_TRACE(rc);
         }
      }
   }

   if (hXvd->bFWDbgLoggingStarted == true)
   {
      /* Re-enable debug logging */
      BXVD_DBG_ControlDecoderDebugLog(hXvd, BXVD_DBG_DebugLogging_eStart);
   }

   for (chanNum = 0; chanNum < BXVD_MAX_VIDEO_CHANNELS; chanNum++)
   {
      hXvdCh = hXvd->ahChannel[chanNum];

      if (hXvdCh != NULL)
      {
         BXVD_DBG_MSG(hXvdCh, ("Reopen decoder channel:%d", chanNum));

         if (hXvdCh->sChSettings.eChannelMode == BXVD_ChannelMode_eStill)
         {
            bStillMode = true;
         }
         else
         {
            bStillMode = false;
         }
         /* Channel handle exists, reopen decoder channel */
         if (bStillMode && hXvd->bStillPictureCompatibilityMode)
         {
            /* Do not re-open the channel here.  It will be re-opened
             * in the next call to BXVD_DecodeStillPicture() */
            hXvdCh->bDecoderChannelOpened = false;
         }
         else
         {
            rc = BXVD_P_HostCmdSendDecChannelOpen((BXVD_Handle)hXvd,
                                                  hXvdCh,
                                                  bStillMode,
                                                  hXvdCh->sChSettings.eDecodeResolution,
                                                  &(hXvdCh->stDecodeFWMemSize),
                                                  &(hXvdCh->stDecodeFWBaseAddrs));

            if (rc != BERR_SUCCESS)
            {
               return BERR_TRACE(rc);
            }

            if (hXvdCh->eDecoderState == BXVD_P_DecoderState_eActive)
            {
               /* Decoder was running when watchdog timer expired, so now need to restart decoder.*/
               BXVD_DBG_MSG(hXvdCh, ("Decoder was running, restart decoder"));
               hXvdCh->eDecoderState = BXVD_P_DecoderState_eNotActive;

               /* Decoder counters should not be cleared */
               hXvdCh->bPreserveCounters = true;

               /* PVR state should not be set to defaults */
               hXvdCh->bPreserveState = true;

               /* Reset XPT Rave CDB read register address */
               XptContextMap.CDB_Read = hXvdCh->ulXptCDB_Read;

               hXvdCh->sDecodeSettings.pContextMap = &XptContextMap;

               for (i = 0; i < hXvdCh->sDecodeSettings.uiContextMapExtNum; i++)
               {
                  hXvdCh->sDecodeSettings.aContextMapExtended[i] = &aXptContextMap_Extended[i];
                  aXptContextMap_Extended[i].CDB_Read = hXvdCh->aulXptCDB_Read_Extended[i];
               }
               
               rc = BERR_TRACE(BXVD_StartDecode(hXvdCh, &hXvdCh->sDecodeSettings));

               hXvdCh->bPreserveState = false;
            }
         }
      }
   }

   BDBG_LEAVE(BXVD_ProcessWatchdog);

   return BERR_TRACE(rc);
}

BXVD_CallbackFunc BXVD_Callback(void *pParm1, int parm2)
{
   BSTD_UNUSED(pParm1);
   BSTD_UNUSED(parm2);

   return (void *)0;
}

#if BDBG_DEBUG_BUILD

#define BXVD_S_MAX_FRAMERATE ( BAVC_FrameRateCode_e20 + 1 )

static const char* sFrameRateCodeNameLUT[BXVD_S_MAX_FRAMERATE] =
{
   "BAVC_FrameRateCode_eUnknown",
   "BAVC_FrameRateCode_e23_976",
   "BAVC_FrameRateCode_e24",
   "BAVC_FrameRateCode_e25",
   "BAVC_FrameRateCode_e29_97",
   "BAVC_FrameRateCode_e30",
   "BAVC_FrameRateCode_e50",
   "BAVC_FrameRateCode_e59_94",
   "BAVC_FrameRateCode_e60",
   "BAVC_FrameRateCode_e14_985",
   "BAVC_FrameRateCode_e7_493",
   "BAVC_FrameRateCode_e10",
   "BAVC_FrameRateCode_e15",
   "BAVC_FrameRateCode_e20"
};

static const char* sDecodeResolutionNameLUT[BXVD_DecodeResolution_eMaxModes] =
{
   "BXVD_DecodeResolution_eHD",
   "BXVD_DecodeResolution_eSD",
   "BXVD_DecodeResolution_eCIF",
   "BXVD_DecodeResolution_eQCIF"
};

#if 0
/* need to conditionally define this for compatibility with older version of avc.h */
#ifndef BAVC_VideoCompressionStd_eMax 
#define BAVC_VideoCompressionStd_eMax 14
#endif
#endif

#define BXVD_S_MAX_VIDEO_PROTOCOL 19

static const char* sVideoCompressionStdNameLUT[BXVD_S_MAX_VIDEO_PROTOCOL] =
{
   "BAVC_VideoCompressionStd_eH264",
   "BAVC_VideoCompressionStd_eMPEG2",
   "BAVC_VideoCompressionStd_eH261",
   "BAVC_VideoCompressionStd_eH263",
   "BAVC_VideoCompressionStd_eVC1",
   "BAVC_VideoCompressionStd_eMPEG1",
   "BAVC_VideoCompressionStd_eMPEG2DTV",
   "BAVC_VideoCompressionStd_eVC1SimpleMain",
   "BAVC_VideoCompressionStd_eMPEG4Part2",
   "BAVC_VideoCompressionStd_eAVS",
   "BAVC_VideoCompressionStd_eMPEG2_DSS_PES",
   "BAVC_VideoCompressionStd_eSVC",
   "BAVC_VideoCompressionStd_eSVC_BL",
   "BAVC_VideoCompressionStd_eMVC",
   "BAVC_VideoCompressionStd_eVP6",
   "BAVC_VideoCompressionStd_eVP7",
   "BAVC_VideoCompressionStd_eVP8",
   "BAVC_VideoCompressionStd_eRV9",
   "BAVC_VideoCompressionStd_eSPARK"
};

static const char* sChannelModeNameLUT[BXVD_ChannelMode_eMax] =
{
   "BXVD_ChannelMode_eVideo",
   "BXVD_ChannelMode_eStill"
};

static const char* s1080pScanModeNameLUT[BXVD_1080pScanMode_eMax] =
{
   "BXVD_1080pScanMode_eDefault",
   "BXVD_1080pScanMode_eAdvanced"
};

static const char* sPictureDropModeNameLUT[BXVD_PictureDropMode_eMax] =
{
   "BXVD_PictureDropMode_eField",
   "BXVD_PictureDropMode_eFrame"
};
#endif

/***************************************************************************
BXVD_OpenChannel: API used to open a video channel.
****************************************************************************/
BERR_Code BXVD_OpenChannel(BXVD_Handle                hXvd,
                           BXVD_ChannelHandle         *phXvdCh,
                           unsigned int               uiChannelNum,
                           const BXVD_ChannelSettings *pChDefSettings)
{
   BERR_Code rc = BERR_SUCCESS;
   BXVD_P_Channel       *pXvdCh = NULL;
   bool bStillMode = false;

   BXDM_PictureProvider_Settings stPictureProviderSettings;

   BDBG_Level eDefaultDebugLevel;

#if BDBG_DEBUG_BUILD
   uint32_t uiProtocolNum;
#endif

   BDBG_ENTER(BXVD_OpenChannel);

   BDBG_ASSERT(hXvd);
   BDBG_ASSERT(phXvdCh);

   BDBG_ASSERT(uiChannelNum<BXVD_MAX_VIDEO_CHANNELS);
   BDBG_ASSERT(pChDefSettings);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* Set channel handle to NULL in case the allocation fails */
   *phXvdCh = NULL;

   /* validate channel number */
   if (uiChannelNum >= BXVD_MAX_VIDEO_CHANNELS)
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

#if BXVD_P_POWER_MANAGEMENT
   if ((hXvd->uiOpenChannels == 0) &&  (hXvd->bHibernate == true))
   {
      /* Wake up the decoder */
      BXVD_P_SetHibernateState(hXvd, false);
   }
#endif

   /* Check if channel is already open */
   if (hXvd->ahChannel[uiChannelNum] != NULL)
   {
      BDBG_ERR(("BXVD_OpenChannel() - Channel [%d] already open. Cannot open again!", uiChannelNum));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   if (pChDefSettings->eChannelMode == BXVD_ChannelMode_eStill)
   {
      bStillMode = true;
   }

   if (bStillMode)
   {
      if (hXvd->bStillChannelAllocated)
      {
         BDBG_ERR(("BXVD_OpenChannel() - Still channel already exists [%d]. Cannot create more than one",
                   hXvd->uiStillChannelNum));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      pXvdCh = hXvd->hStillChannel;
   }

   else
   {
      BXVD_P_GetChannelHandle(hXvd, &pXvdCh);

      if (!pXvdCh)
      {
         return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
      }
   }      

   BDBG_REGISTER_INSTANCE(pXvdCh);
   rc = BDBG_GetLevel(&eDefaultDebugLevel);
   rc = BDBG_SetInstanceLevel(pXvdCh, eDefaultDebugLevel);
   rc = BDBG_SetInstanceName(pXvdCh, sChannelNameLUT[hXvd->uDecoderInstance][uiChannelNum]);

   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - hXvd = 0x%08x", hXvd));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - phXvdCh = 0x%08x", phXvdCh));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - uiChannelNum = %d", uiChannelNum));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - pChDefSettings = 0x%08x", pChDefSettings));

   /* Zero out the newly allocated context */
   BKNI_Memset((void*)pXvdCh, BXVD_P_MEM_ZERO, sizeof(BXVD_P_Channel));

   /* Set the defaults */
   BKNI_Memcpy((void *)&pXvdCh->sChSettings,
               (void *)pChDefSettings,
               sizeof(BXVD_ChannelSettings));

   /* Assigm ptr to the video compression standards list */

   pXvdCh->sChSettings.peVideoCmprStdList = &pXvdCh->asVideoCmprStdList[0];

   /* Copy the list */
   /* coverity[deref_ptr_in_call: FALSE] */
   BKNI_Memcpy(pXvdCh->sChSettings.peVideoCmprStdList,
               pChDefSettings->peVideoCmprStdList,
               pChDefSettings->uiVideoCmprCount * sizeof(BAVC_VideoCompressionStd));

   pXvdCh->sChSettings.uiVideoCmprCount = pChDefSettings->uiVideoCmprCount;

   /* Setup default channel heaps.
    *
    * We want the heaps that are specified at BXVD_OpenChannel() to
    * follow the same fall-back as the heaps specified at BXVD_Open().
    * So, if hChannelSystemHeap is specified, but hChannelCabacHeap
    * and/or hChannelPictureHeap are not, then we want them to both
    * default to the hChannelSystemHeap and NOT to XVD's sub-heaps.
    *
    * This still allows full flexibilty for specifying a separate heap
    * for each buffer type (context, cabac, and picture), but the
    * defaults are in-line with what users would expect from
    * BXVD_Open().
    *
    * Typically, on UMA platforms, the application would only need to
    * specify hChannelSystemHeap if it wanted all channel memory
    * allocations (context, cabac, and picture buffers) to come from
    * that heap instead of XVD's sub-heaps.  We don't require the app
    * to explicitly specify the hChannelCabacHeap and
    * hChannelPictureHeap heaps, but there's no harm in doing so,
    * either.
    *
    * On non-UMA platforms, the application should specify both the
    * hChannelSystemHeap and hChannelPictureHeap in order for channel
    * specific heap memory allocation to work properly. */
   if (pXvdCh->sChSettings.hChannelCabacHeap == NULL)
   {
      pXvdCh->sChSettings.hChannelCabacHeap = pXvdCh->sChSettings.hChannelSystemHeap;
   }

   if (pXvdCh->sChSettings.hChannelPictureHeap == NULL)
   {
      pXvdCh->sChSettings.hChannelPictureHeap = pXvdCh->sChSettings.hChannelSystemHeap;
   }

#if BDBG_DEBUG_BUILD
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.ui32MonitorRefreshRate = %d", pXvdCh->sChSettings.ui32MonitorRefreshRate));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.ulRemovalDelay = %d", pXvdCh->sChSettings.ulRemovalDelay));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.uiVsyncDiffThreshDefault = %d", pXvdCh->sChSettings.uiVsyncDiffThreshDefault));
   if (pXvdCh->sChSettings.eDefaultFrameRate < BXVD_S_MAX_FRAMERATE)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.eDefaultFrameRate = %s (%d)",
                                 sFrameRateCodeNameLUT[pXvdCh->sChSettings.eDefaultFrameRate],
                                 pXvdCh->sChSettings.eDefaultFrameRate));
   }
   else
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.eDefaultFrameRate = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sChSettings.eDefaultFrameRate));
   }

   if (pXvdCh->sChSettings.eDecodeResolution < BXVD_DecodeResolution_eMaxModes)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.eDecodeResolution = %s (%d)",
                                 sDecodeResolutionNameLUT[pXvdCh->sChSettings.eDecodeResolution],
                                 pXvdCh->sChSettings.eDecodeResolution));
   }
   else
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.eDecodeResolution = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sChSettings.eDecodeResolution));
   }
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.uiVideoCmprCount = %d", pXvdCh->sChSettings.uiVideoCmprCount));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.peVideoCmprStdList = %08p", pXvdCh->sChSettings.peVideoCmprStdList));
   if (pXvdCh->sChSettings.peVideoCmprStdList)
   {
      for (uiProtocolNum = 0; uiProtocolNum < pXvdCh->sChSettings.uiVideoCmprCount; uiProtocolNum++)
      {
         if (pXvdCh->sChSettings.peVideoCmprStdList[uiProtocolNum] < BXVD_S_MAX_VIDEO_PROTOCOL)
         {
            BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.peVideoCmprStdList[%d] = %s (%d)",
                                       uiProtocolNum,
                                       sVideoCompressionStdNameLUT[pXvdCh->sChSettings.peVideoCmprStdList[uiProtocolNum]],
                                       pXvdCh->sChSettings.peVideoCmprStdList[uiProtocolNum]));
         }
         else
         {
            BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.peVideoCmprStdList[%d] = %s (%d)",
                                       uiProtocolNum,
                                       "Unknown/Invalid Value!",
                                       pXvdCh->sChSettings.peVideoCmprStdList[uiProtocolNum]));
         }
      }
   }

   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.uiVDCRectangleNum = %d",
                              pXvdCh->sChSettings.uiVDCRectangleNum));

   if (pXvdCh->sChSettings.eChannelMode < BXVD_ChannelMode_eMax)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.eChannelMode = %s (%d)",
                                 sChannelModeNameLUT[pXvdCh->sChSettings.eChannelMode],
                                 pXvdCh->sChSettings.eChannelMode));
   }
   else
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.eChannelMode = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sChSettings.eChannelMode));
   }
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.bMPEG2BTPEnable = %d", pXvdCh->sChSettings.bMPEG2BTPEnable));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.bAVC41Enable = %d", pXvdCh->sChSettings.bAVC41Enable));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.bAVC51Enable = %d", pXvdCh->sChSettings.bAVC51Enable));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.bBluRayEnable = %d", pXvdCh->sChSettings.bBluRayEnable));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.bExcessDirModeEnable = %d", pXvdCh->sChSettings.bExcessDirModeEnable));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.bSVC3DModeEnable = %d", pXvdCh->sChSettings.bSVC3DModeEnable));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.hChannelSystemHeap = 0x%08x", pXvdCh->sChSettings.hChannelSystemHeap));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.hChannelCabacHeap = 0x%08x", pXvdCh->sChSettings.hChannelCabacHeap));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.hChannelPictureHeap = 0x%08x", pXvdCh->sChSettings.hChannelPictureHeap));

   if (pXvdCh->sChSettings.e1080pScanMode < BXVD_1080pScanMode_eMax)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.e1080pScanMode = %s (%d)",
                                 s1080pScanModeNameLUT[pXvdCh->sChSettings.e1080pScanMode],
                                 pXvdCh->sChSettings.e1080pScanMode));
   }
   else
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.e1080pScanMode = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sChSettings.e1080pScanMode));
   }

   if (pXvdCh->sChSettings.ePictureDropMode < BXVD_PictureDropMode_eMax)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.ePictureDropMode = %s (%d)",
                                 sPictureDropModeNameLUT[pXvdCh->sChSettings.ePictureDropMode],
                                 pXvdCh->sChSettings.ePictureDropMode));
   }
   else
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_OpenChannel() - BXVD_ChannelSettings.ePictureDropMode = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sChSettings.ePictureDropMode));
   }
#endif

   /* Store handles in the context */
   pXvdCh->pXvd = hXvd;

   /* Save channel number */
   pXvdCh->ulChannelNum = uiChannelNum;

   /* Set handle type */
   pXvdCh->eHandleType = BXVD_P_HandleType_XvdChannel;

   BXDM_PictureProvider_GetDefaultSettings(&stPictureProviderSettings);

   rc = BXDM_PictureProvider_Create(&pXvdCh->hPictureProvider, &stPictureProviderSettings);

   if (rc != BERR_SUCCESS)
   {
      BXVD_CloseChannel(pXvdCh);
      return BERR_TRACE(rc);
   }

   BKNI_EnterCriticalSection(); 

   rc = BXVD_P_InitChannel(pXvdCh);

   BKNI_LeaveCriticalSection();

   if (rc != BERR_SUCCESS)
   {
      BXVD_CloseChannel(pXvdCh);
      return BERR_TRACE(rc);
   }

   /* Allocate memory based on whether this is a still or video decode channel */
   if (bStillMode)
   {
      /* Still Decode Channel */
      rc = BXVD_P_GetStillDecodeFWMemSize(hXvd,
                                          pXvdCh->sChSettings.eDecodeResolution,
                                          pXvdCh->sChSettings.peVideoCmprStdList,
                                          pXvdCh->sChSettings.uiVideoCmprCount,
                                          &(pXvdCh->stDecodeFWMemSize));


      if (rc != BERR_SUCCESS)
      {
         BXVD_CloseChannel(pXvdCh);
         return BERR_TRACE(rc);
      }
   }
   else
   {
      /* Video Decode Channel memory size */
      rc = BXVD_P_GetDecodeFWMemSize(hXvd,
                                     pXvdCh->sChSettings.eDecodeResolution,
                                     pXvdCh->sChSettings.peVideoCmprStdList,
                                     pXvdCh->sChSettings.uiVideoCmprCount,
                                     &(pXvdCh->sChSettings),
                                     &(pXvdCh->stDecodeFWMemSize));
      if (rc != BERR_SUCCESS)
      {
         BXVD_CloseChannel(pXvdCh);
         return BERR_TRACE(rc);
      }
   }

   rc = BXVD_P_AllocateFWMem(hXvd,
                             pXvdCh,
                             &(pXvdCh->stDecodeFWMemSize),
                             &(pXvdCh->stDecodeFWBaseAddrs));

   if (rc != BERR_SUCCESS)
   {
      BXVD_CloseChannel(pXvdCh);
      return BERR_TRACE(rc);
   }


   if (!bStillMode || !hXvd->bStillPictureCompatibilityMode)
   {
      /* We open the channel now only if we not in a still picture
       * compatibility mode, otherwise we open/close during
       * BXVD_DecodeStillPicture(), since we need to pass in the CDB
       * as the cabac bin buffer */
      rc = BXVD_P_HostCmdSendDecChannelOpen((BXVD_Handle)hXvd,
                                            pXvdCh,
                                            bStillMode,
                                            pXvdCh->sChSettings.eDecodeResolution,
                                            &(pXvdCh->stDecodeFWMemSize),
                                            &(pXvdCh->stDecodeFWBaseAddrs));

      if (rc != BERR_SUCCESS)
      {
         BXVD_CloseChannel(pXvdCh);
         return BERR_TRACE(rc);
      }
   }

   /* Update the open channel count */
   hXvd->uiOpenChannels++;

   /* Return the new channel context to the user */
   *phXvdCh = (BXVD_ChannelHandle)pXvdCh;

   if (bStillMode)
   {
      hXvd->bStillChannelAllocated = true;
      hXvd->uiStillChannelNum = uiChannelNum;
   }

   hXvd->ahChannel[uiChannelNum] = *phXvdCh;

   BXVD_Status_OpenChannel(hXvd->hXvdStatus, pXvdCh);

   BXVD_DBG_MSG(pXvdCh, ("BXVD_OpenChannel() - pXvdCh = 0x%08x", pXvdCh));

   BDBG_LEAVE(BXVD_OpenChannel);

   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
BXVD_CloseChannel: API used to close a channel
***************************************************************************/
BERR_Code BXVD_CloseChannel(BXVD_ChannelHandle hXvdCh)
{
   BERR_Code rc = BERR_SUCCESS;
   BXVD_Handle hXvd = NULL;

   BDBG_ENTER(BXVD_CloseChannel);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   hXvd = (BXVD_Handle) hXvdCh->pXvd;
   BDBG_ASSERT(hXvd);

   if ( hXvdCh->sChSettings.eChannelMode != BXVD_ChannelMode_eStill )
   {
      if ( hXvdCh->sDecodeSettings.bExternalPictureProviderMode == false )
      {      
         BXDM_DisplayInterruptHandler_Handle hXdmDih; 
      
         BXDM_PictureProvider_GetDIH( hXvdCh->hPictureProvider, &hXdmDih );

         if ( NULL != hXdmDih )
         {
            BERR_TRACE(BXDM_DisplayInterruptHandler_RemovePictureProviderInterface(
                          hXdmDih,
                          BXDM_PictureProvider_GetPicture_isr,
                          hXvdCh->hPictureProvider));
         }
      }
   }

   /* Critical section to prevent ISR routines from using data about to be freed. */
   BKNI_EnterCriticalSection();

   /* NULL the channel entry to prevent DM from accessing the channel structure */
   hXvd->ahChannel[hXvdCh->ulChannelNum] = NULL;

   BKNI_LeaveCriticalSection();

   /* If the decoder is running, stop it now */
   if (hXvdCh->eDecoderState == BXVD_P_DecoderState_eActive)
   {
      BXVD_DBG_MSG(hXvdCh, ("Decoder is active... calling BXVD_StopDecode"));
      rc = BXVD_StopDecode(hXvdCh);
   }

   if (hXvdCh->bDecoderChannelOpened)
   {
      rc = BERR_TRACE(BXVD_P_HostCmdSendDecChannelClose(hXvd, hXvdCh));
   }

   BXVD_P_FreeFWMem(hXvd,
                    hXvdCh,
                    hXvdCh->stDecodeFWBaseAddrs.uiFWContextBase,
                    hXvdCh->stDecodeFWBaseAddrs.uiFWCabacBase,
                    hXvdCh->stDecodeFWBaseAddrs.uiFWPicBlockBase,
                    hXvdCh->stDecodeFWBaseAddrs.uiFWCabacWorklistBase,
                    hXvdCh->stDecodeFWBaseAddrs.uiFWInterLayerPicBase);

   if ( NULL != hXvdCh->hPictureProvider )
   {
      rc = BERR_TRACE(BXDM_PictureProvider_Destroy( hXvdCh->hPictureProvider ));
   }

   /* Adjust the number of active channels */
   hXvd->uiOpenChannels--;

   /* If still decode channel, we need to clear the flag. */
   if (hXvdCh->sChSettings.eChannelMode == BXVD_ChannelMode_eStill)
   {
      hXvd->bStillChannelAllocated = false;
   }

   /* Free the video compression standards list */

   hXvdCh->sChSettings.peVideoCmprStdList = NULL;
   hXvdCh->sChSettings.uiVideoCmprCount = 0;

   BXVD_Status_CloseChannel(hXvd->hXvdStatus, hXvdCh);

   BDBG_UNREGISTER_INSTANCE(hXvdCh);

   if (hXvdCh->sChSettings.eChannelMode != BXVD_ChannelMode_eStill)
   {
      /* Keep channel context to use again later */
      BXVD_P_KeepChannelHandle(hXvd, hXvdCh);
   }

#if BXVD_P_POWER_MANAGEMENT
   if (hXvd->uiOpenChannels == 0)
   {
      /* Put decoder in hibernate state */
      BXVD_P_SetHibernateState(hXvd, true);
   }
#endif

   BDBG_LEAVE(BXVD_CloseChannel);
   return BERR_TRACE(rc);
}

#if BDBG_DEBUG_BUILD
static const char* sTimebaseNameLUT[BAVC_Timebase_e3 + 1] =
{
   "BAVC_Timebase_e0",
   "BAVC_Timebase_e1",
   "BAVC_Timebase_e2",
   "BAVC_Timebase_e3"
};

static const char* sSTCNameLUT[BXVD_STC_eMax] =
{
   "BXVD_STC_eZero",
   "BXVD_STC_eOne",
   "BXVD_STC_eTwo",
   "BXVD_STC_eThree",
   "BXVD_STC_eFour",
   "BXVD_STC_eFive",
   "BXVD_STC_eSix",
   "BXVD_STC_eSeven"
};

static const char* sDisplayInterruptNameLUT[BXVD_DisplayInterrupt_eMax] =
{
   "BXVD_DisplayInterrupt_eZero",
   "BXVD_DisplayInterrupt_eOne"
};

static const char* sHITSModeNameLUT[BXVD_HITSMode_eClean + 1] =
{
   "BXVD_HITSMode_eDisabled",
   "BXVD_HITSMode_eLegacy",
   "BXVD_HITSMode_eClean"
};

static const char* sErrorHandlingNameLUT[BXVD_ErrorHandling_ePrognostic + 1] =
{
   "BXVD_ErrorHandling_eOff",
   "BXVD_ErrorHandling_ePicture",
   "BXVD_ErrorHandling_ePrognostic"
};

static const char* sFrameRateDetectionModeNameLUT[BXVD_FrameRateDetectionMode_eStable + 1] =
{
   "BXVD_FrameRateDetectionMode_eOff",
   "BXVD_FrameRateDetectionMode_eFast",
   "BXVD_FrameRateDetectionMode_eStable"
};

static const char* sTimestampModeNameLUT[BXVD_TimestampMode_eMaxModes] =
{
   "BXVD_TimestampMode_eDecode",
   "BXVD_TimestampMode_eDisplay"
};
#endif

/***************************************************************************
BXVD_StartDecode: Starts decode on a given channel.
****************************************************************************/
BERR_Code BXVD_StartDecode(BXVD_ChannelHandle        hXvdChannel,
                           const BXVD_DecodeSettings *psDecodeSettings)
{
   uint32_t uiContextReg;
   uint32_t uiContextRegExtend[BXVD_NUM_EXT_RAVE_CONTEXT];
   uint32_t uiCDBBase;
   uint32_t uiChannelMode;
   uint32_t uiVirtAddr;

   uint32_t uiVecIndex;
   uint32_t i;

   BXVD_P_Context *pXvd;
   BXVD_P_Channel *pXvdCh;

   /* SW7425-3177: for mapping BAVC_VideoCompressionStd to BXVD_P_PPB_Protocol.
    * Will be passed to AVD via BXVD_P_HostCmdSendDecChannelStart() */
   BXVD_P_PPB_Protocol eProtocol;
   BERR_Code rc;

   BXVD_DisplayInterrupt eDisplayInterrupt;

   BDBG_ENTER(BXVD_StartDecode);

   BDBG_ASSERT(psDecodeSettings);

   /* Check handle type for correctness */
   if (hXvdChannel->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   pXvdCh = (BXVD_P_Channel*)hXvdChannel;
   BDBG_ASSERT(pXvdCh);

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - hXvdChannel = 0x%08x", hXvdChannel));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - psDecodeSettings = 0x%08x", psDecodeSettings));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - eChannelType: %08x", pXvdCh->eChannelType));

   pXvd = pXvdCh->pXvd;
   BDBG_ASSERT(pXvd);

   if (pXvdCh->eDecoderState == BXVD_P_DecoderState_eActive)
   {
      BXVD_DBG_MSG(pXvdCh, ("Decoder is already active"));

      BXVD_StopDecode(pXvdCh);

      return BERR_TRACE(BXVD_ERR_DECODER_ACTIVE);
   }

   if (BXVD_P_IsDecodeProtocolSupported( pXvd, psDecodeSettings->eVideoCmprStd ) == false)
   {
      BXVD_DBG_WRN(pXvdCh, ("Video compression standard:%u not supported on this platform.", psDecodeSettings->eVideoCmprStd));

      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   if (psDecodeSettings->pContextMap == NULL)
   {
      BXVD_DBG_MSG(pXvdCh, ("NULL XPT Context pointer"));

      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Save decode settings */
   BKNI_Memcpy((void*)&(pXvdCh->sDecodeSettings),
               (void*)psDecodeSettings,
               sizeof(BXVD_DecodeSettings));

   /* Since GetDecodeDefaultSettings was added recently, we need to
    * check the signature of the BXVD_DecodeSettings struct to see if
    * the app called it.  Legacy apps won't call it, so we need to set
    * the recently added eSTC parameter accordingly to maintain backwards
    * compatibility with pre-mosaic firmware */
   if (pXvdCh->sDecodeSettings.uiSignature != BXVD_DECODESETTINGS_SIGNATURE)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - No signature found!  Using legacy defaults."));

      /* AVD0 should use STC0 and AVD1 should use STC1 to maintain
       * backwards compatibility with pre-mosaic firmware */
      if (pXvd->uDecoderInstance == 0)
      {
         pXvdCh->sDecodeSettings.eSTC = BXVD_STC_eZero;
      }
      else
      {
         pXvdCh->sDecodeSettings.eSTC = BXVD_STC_eOne;
      }

      /* Set the default display interrupt */
      pXvdCh->sDecodeSettings.eDisplayInterrupt = s_stDefaultDecodeSettings.eDisplayInterrupt;

      /* VDC Rectangle number defaults to channel number */
      pXvdCh->sDecodeSettings.uiVDCRectangleNum = pXvdCh->ulChannelNum;

      /* Set ASTM Mode to default */
      pXvdCh->sDecodeSettings.bAstmMode = s_stDefaultDecodeSettings.bAstmMode;

      /* Set Enable auto vsync mode to default */
      pXvdCh->sDecodeSettings.bVsyncModeOnPcrDiscontinuity = s_stDefaultDecodeSettings.bVsyncModeOnPcrDiscontinuity;

      /* Disable the preroll mode */
      pXvdCh->sDecodeSettings.uiPreRollRate = s_stDefaultDecodeSettings.uiPreRollRate;

      /* Picture error handling is off by default. */
      pXvdCh->sDecodeSettings.eErrorHandling = s_stDefaultDecodeSettings.eErrorHandling;

      /* Frame Rate Detection (FRD) is off by default */
      pXvdCh->sDecodeSettings.eFrameRateDetectionMode = s_stDefaultDecodeSettings.eFrameRateDetectionMode;

      /* Set the signature, so we don't reset the decode settings when
       * we are re-starting the decode in the case of a watchdog
       * and/or flush */
      pXvdCh->sDecodeSettings.uiSignature = BXVD_DECODESETTINGS_SIGNATURE;
   }

   /* SW7425-1064: if this is the "enhanced" channel of a pair of linked channels,
    * use the interrupt number of the base channel.  Otherwise just use the number
    * for this channel.
    */
   if ( BXVD_P_ChannelType_eEnhanced == pXvdCh->eChannelType 
       && NULL != pXvdCh->hXvdChLinked )
   {
      eDisplayInterrupt = pXvdCh->hXvdChLinked->sDecodeSettings.eDisplayInterrupt;

      /* TODO: is the following assignment needed? */
      /* pXvdCh->sDecodeSettings.eDisplayInterrupt = eDisplayInterrupt;*/

#if BXVD_P_USE_TWO_DECODERS
      /* If the XVD handles don't match, the two channels are being decoded
       * on separate decoders.  The "enhanced" decoder needs to be driven off
       * the same interrupt as the "base" decoder.  Normally this regsitration
       * is handled by a call to"BXVD_RegisterVdcDeviceInterrupt".
       */
      if ( pXvdCh->pXvd != pXvdCh->hXvdChLinked->pXvd )
      {
         BXVD_DisplayInterruptProvider_P_InterruptSettings stInterruptConfig;

         /* Save the interrupt settings for the enhanced channel,
          * these will be restored in BXVD_StopDecode.
          */
         BXVD_DisplayInterruptProvider_P_GetInterruptConfiguration( 
                     pXvdCh->pXvd->hXvdDipCh[ eDisplayInterrupt ],
                     &(pXvdCh->stEnhancedInterruptConfig) 
                     );

         /* Get the interrupt settings for the base channel. */
         BXVD_DisplayInterruptProvider_P_GetInterruptConfiguration( 
                     pXvdCh->hXvdChLinked->pXvd->hXvdDipCh[ eDisplayInterrupt ],
                     &stInterruptConfig 
                     );

         /* Set the interrupt settings on the enhanced channel to match the base channel. */
         rc = BXVD_DisplayInterruptProvider_P_SetInterruptConfiguration(
                     pXvdCh->pXvd->hXvdDipCh[ eDisplayInterrupt ],
                     &stInterruptConfig 
                     );

         /* TODO: Could this call fail? If so, what are the appropriate actions? */
         if (rc != BERR_SUCCESS )
         {
            BXVD_DBG_ERR(pXvdCh, ("BXVD_StartDecode->BXVD_DisplayInterruptProvider_P_SetInterruptConfiguration returned %d on the base decoder", rc));
            return BERR_TRACE(rc);
         }
      }
#endif

   }
   else
   {
     eDisplayInterrupt = pXvdCh->sDecodeSettings.eDisplayInterrupt;
   }

   /* check for BAVC_VideoCompressionStd_eVC1SimpleMain and replace with BAVC_VideoCompressionStd_eVC1
      note, this is done in the saved settings so the working state will be saved  */
   if (pXvdCh->sDecodeSettings.eVideoCmprStd == BAVC_VideoCompressionStd_eVC1SimpleMain)
   {
      pXvdCh->sDecodeSettings.eVideoCmprStd = BAVC_VideoCompressionStd_eVC1;
   }

   /* If frame rate in decode settings is not valid, then use channel default frame rate */
   if (pXvdCh->sDecodeSettings.eDefaultFrameRate == BAVC_FrameRateCode_eUnknown)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - Unknown default frame rate(%d)!  Using channel default(%d)",
                                 pXvdCh->sChSettings.eDefaultFrameRate,
                                 pXvdCh->sDecodeSettings.eDefaultFrameRate));
      pXvdCh->sDecodeSettings.eDefaultFrameRate = pXvdCh->sChSettings.eDefaultFrameRate;
   }

#if BDBG_DEBUG_BUILD
   if (pXvdCh->sDecodeSettings.eVideoCmprStd < BXVD_S_MAX_VIDEO_PROTOCOL)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eVideoCmprStd = %s (%d)",
                                 sVideoCompressionStdNameLUT[pXvdCh->sDecodeSettings.eVideoCmprStd],
                                 pXvdCh->sDecodeSettings.eVideoCmprStd));
   }
   else
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eVideoCmprStd = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sDecodeSettings.eVideoCmprStd));
   }
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.ulMultiStreamId = %d (deprecated)", pXvdCh->sDecodeSettings.ulMultiStreamId));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.ulVideoSubStreamId = %d (deprecated)", pXvdCh->sDecodeSettings.ulVideoSubStreamId));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bPlayback = %d", pXvdCh->sDecodeSettings.bPlayback));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bCrcMode = %d", pXvdCh->sDecodeSettings.bCrcMode));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.stDataXprtOutput.eXptOutputId = %d (deprecated)", pXvdCh->sDecodeSettings.stDataXprtOutput.eXptOutputId));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.stDataXprtOutput.eXptSourceId = %d (deprecated)", pXvdCh->sDecodeSettings.stDataXprtOutput.eXptSourceId));
   if (pXvdCh->sDecodeSettings.eTimeBase <= BAVC_Timebase_e3)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eTimeBase = %s (%d)",
                                 sTimebaseNameLUT[pXvdCh->sDecodeSettings.eTimeBase],
                                 pXvdCh->sDecodeSettings.eTimeBase));
   }
   else
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eTimeBase = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sDecodeSettings.eTimeBase));
   }

   if (pXvdCh->sDecodeSettings.eTimestampMode < BXVD_TimestampMode_eMaxModes)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eTimestampMode = %s (%d)",
                                 sTimestampModeNameLUT[pXvdCh->sDecodeSettings.eTimestampMode],
                                 pXvdCh->sDecodeSettings.eTimestampMode));
   }
   else
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eTimestampMode = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sDecodeSettings.eTimestampMode));
   }

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bIFrameAsRAP = %d",
                              pXvdCh->sDecodeSettings.bIFrameAsRAP));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bAVCErrorConcealmentMode = %d",
                              pXvdCh->sDecodeSettings.bAVCErrorConcealmentMode));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bIOnlyFieldOutputMode = %d",
                              pXvdCh->sDecodeSettings.bIOnlyFieldOutputMode));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.pContextMap = 0x%08x", pXvdCh->sDecodeSettings.pContextMap));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.pContextMap->CDB_Read = 0x%08x", pXvdCh->sDecodeSettings.pContextMap->CDB_Read));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.uiSignature = 0x%08x", pXvdCh->sDecodeSettings.uiSignature));
   for (i=0; i< pXvdCh->sDecodeSettings.uiContextMapExtNum; i++)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.aContextMapExtended[%d] = 0x%08x", i, pXvdCh->sDecodeSettings.aContextMapExtended[i]));
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.aContextMapExtended[%d]->CDB_Read = 0x%08x", i, pXvdCh->sDecodeSettings.aContextMapExtended[i]->CDB_Read));
   }

   /* This checks to see if STC index is supported on this platform */ 
   if (pXvdCh->sDecodeSettings.eSTC < BXVD_P_STC_MAX)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eSTC = %s (%d)",
                                 sSTCNameLUT[pXvdCh->sDecodeSettings.eSTC],
                                 pXvdCh->sDecodeSettings.eSTC));
   }
   else
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eSTC = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sDecodeSettings.eSTC));
   }
   if (eDisplayInterrupt < BXVD_DisplayInterrupt_eMax)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eDisplayInterrupt = %s (%d)",
                                 sDisplayInterruptNameLUT[eDisplayInterrupt],
                                 eDisplayInterrupt));
   }
   else
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eDisplayInterrupt = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 eDisplayInterrupt));
   }
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.uiVDCRectangleNum = %d", pXvdCh->sDecodeSettings.uiVDCRectangleNum));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bAstmMode = %d", pXvdCh->sDecodeSettings.bAstmMode));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bVsyncModeOnPcrDiscontinuity = %d", pXvdCh->sDecodeSettings.bVsyncModeOnPcrDiscontinuity));
   if (pXvdCh->sDecodeSettings.eHITSMode <= BXVD_HITSMode_eClean)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eHITSMode = %s (%d)",
                                 sHITSModeNameLUT[pXvdCh->sDecodeSettings.eHITSMode],
                                 pXvdCh->sDecodeSettings.eHITSMode));
   }
   else
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eHITSMode = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sDecodeSettings.eHITSMode));
   }
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bZeroDelayOutputMode = %d", pXvdCh->sDecodeSettings.bZeroDelayOutputMode));
   if (pXvdCh->sDecodeSettings.eDefaultFrameRate < BXVD_S_MAX_FRAMERATE)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eDefaultFrameRate = %s (%d)",
                                 sFrameRateCodeNameLUT[pXvdCh->sDecodeSettings.eDefaultFrameRate],
                                 pXvdCh->sDecodeSettings.eDefaultFrameRate));
   }
   else
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eDefaultFrameRate = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sDecodeSettings.eDefaultFrameRate));
   }
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.uiPreRollRate = %d", pXvdCh->sDecodeSettings.uiPreRollRate));
   if (pXvdCh->sDecodeSettings.eErrorHandling <= BXVD_ErrorHandling_ePrognostic)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eErrorHandling = %s (%d)",
                                 sErrorHandlingNameLUT[pXvdCh->sDecodeSettings.eErrorHandling],
                                 pXvdCh->sDecodeSettings.eErrorHandling));
   }
   else
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eErrorHandling = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sDecodeSettings.eErrorHandling));
   }
   if (pXvdCh->sDecodeSettings.eFrameRateDetectionMode <= BXVD_FrameRateDetectionMode_eStable)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eFrameRateDetectionMode = %s (%d)",
                                 sFrameRateDetectionModeNameLUT[pXvdCh->sDecodeSettings.eFrameRateDetectionMode],
                                 pXvdCh->sDecodeSettings.eFrameRateDetectionMode));
   }
   else
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.eFrameRateDetectionMode = %s (%d)",
                                 "Unknown/Invalid Value!",
                                 pXvdCh->sDecodeSettings.eFrameRateDetectionMode));
   }
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bIgnoreDPBOutputDelaySyntax = %d",
                              pXvdCh->sDecodeSettings.bIgnoreDPBOutputDelaySyntax));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.uiSEIMessageFlags = %d",
                              pXvdCh->sDecodeSettings.uiSEIMessageFlags));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bPFrameSkipDisable = %d",
                              pXvdCh->sDecodeSettings.bPFrameSkipDisable));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bExternalPictureProviderMode = %d",
                              pXvdCh->sDecodeSettings.bExternalPictureProviderMode));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bAVCAspectRatioOverridMode = %d",
                              pXvdCh->sDecodeSettings.bAVCAspectRatioOverrideMode));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bSVC3DModeEnable = %d",
                              pXvdCh->sDecodeSettings.bSVC3DModeEnable));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bSWCoefAVCDecodeModeEnable = %d",
                              pXvdCh->sDecodeSettings.bSWCoefAVCDecodeModeEnable));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bIgnoreNumReorderFramesEqZero = %d",
                         pXvdCh->sDecodeSettings.bIgnoreNumReorderFramesEqZero));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bEarlyPictureDeliveryMode = %d",
                         pXvdCh->sDecodeSettings.bEarlyPictureDeliveryMode));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - BXVD_DecodeSettings.bUserDataBTPModeEnable = %d",
                         pXvdCh->sDecodeSettings.bUserDataBTPModeEnable));

#endif

   /* Read CDB Base register to verify that the CDB Base is 256 byte aligned. */
   uiContextReg = ((pXvdCh->sDecodeSettings.pContextMap->CDB_Read)+4);
   uiCDBBase = BXVD_Reg_Read32( pXvdCh->pXvd, uiContextReg );

   if (uiCDBBase != (uiCDBBase & (~0xff)))
   {
      BXVD_DBG_WRN(pXvdCh, ("CBD Base not 256 byte aligned: 0x%08x", uiCDBBase));
   }

#if !BXVD_P_AVD_ARC600
   {
      uint32_t uiCDBEnd, uiITBEnd;

      /* Read CDB End register to verify that the CDB end is accessible by ARC */
      uiContextReg = ((pXvdCh->sDecodeSettings.pContextMap->CDB_Read)+8);
      uiCDBEnd = BXVD_Reg_Read32( pXvdCh->pXvd, uiContextReg );

      if (uiCDBEnd >= BXVD_P_ARC300_RAM_LIMIT)
      {
         BXVD_DBG_ERR(pXvdCh, ("CDB: %0x allocated in region greater than 768MB!", uiCDBEnd));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      /* Read CDB End register to verify that the CDB end is accessible by ARC */
      uiContextReg = ((pXvdCh->sDecodeSettings.pContextMap->CDB_Read)+44);
      uiITBEnd = BXVD_Reg_Read32( pXvdCh->pXvd, uiContextReg );

      if (uiITBEnd >= BXVD_P_ARC300_RAM_LIMIT)
      {
         BXVD_DBG_ERR(pXvdCh, ("ITB:%0x allocated in region greater than 768MB!", uiITBEnd));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

   }
#endif

   /* Calculate the register address for the appropriate context */
   uiContextReg = ((pXvdCh->sDecodeSettings.pContextMap->CDB_Read)-4) + BCHP_PHYSICAL_OFFSET;

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - XPT Rave Context reg base = 0x%08x", uiContextReg));

   pXvdCh->ulXptCDB_Read = pXvdCh->sDecodeSettings.pContextMap->CDB_Read;

   if (pXvdCh->sDecodeSettings.uiContextMapExtNum != 0 )
   {
      for (i=0; i< pXvdCh->sDecodeSettings.uiContextMapExtNum; i++)
      {
         /* Read CDB Base register to verify that the CDB Base is 256 byte aligned. */
         uiContextRegExtend[i] = ((pXvdCh->sDecodeSettings.aContextMapExtended[i]->CDB_Read)+4);
         uiCDBBase = BXVD_Reg_Read32( pXvdCh->pXvd, uiContextRegExtend[i] );

         if (uiCDBBase != (uiCDBBase & (~0xff)))
         {
            BXVD_DBG_MSG(pXvdCh, ("Extended[%d] CBD Base not 256 byte aligned: 0x%08x", i, uiCDBBase));
         }

         /* Calculate the register address for the appropriate context */
         uiContextRegExtend[i] = ((pXvdCh->sDecodeSettings.aContextMapExtended[i]->CDB_Read)-4) + BCHP_PHYSICAL_OFFSET;

         BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - XPT Rave Context Extended[%d] reg base = 0x%08x",
                                    i, uiContextRegExtend[i]));

         /* Save extended CDB read pointer register address to restore when restart is needed */
         pXvdCh->aulXptCDB_Read_Extended[i] = pXvdCh->sDecodeSettings.aContextMapExtended[i]->CDB_Read;
      }
   }
   else
   {
      uiContextRegExtend[0] = (uint32_t) NULL;
   }

   if ((pXvdCh->sDecodeSettings.eVideoCmprStd == BAVC_VideoCompressionStd_eH264) &&
       (pXvdCh->stDecoderContext.bHostSparseMode == true))
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - Sparse mode TRUE"));

      if (pXvdCh->eCurrentSkipMode == BXVD_SkipMode_eDecode_IPB)
      {
         uiChannelMode = VDEC_CHANNEL_MODE_SPARSE_NOSKIP;
      }
      else if (pXvdCh->eCurrentSkipMode == BXVD_SkipMode_eDecode_Ref_Only)
      {
         uiChannelMode = VDEC_CHANNEL_MODE_SPARSE_REFONLY;
      }
      else if (pXvdCh->eCurrentSkipMode == BXVD_SkipMode_eDecode_IP_Only)
      {
         uiChannelMode = VDEC_CHANNEL_MODE_SPARSE_IPONLY;
      }
      else if (pXvdCh->eCurrentSkipMode == BXVD_SkipMode_eDecode_I_Only)
      {
         uiChannelMode = VDEC_CHANNEL_MODE_SPARSE_IONLY;
      }
      else
      {
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - FW ChanMode:%d CurSkipMode:%d", uiChannelMode, pXvdCh->eCurrentSkipMode));
   }
   else if (pXvdCh->stDecoderContext.bHostSparseMode == true)
   {
      BXVD_DBG_WRN(pXvdCh, ("BXVD_StartDecode() - Sparse mode enabled on Non H264 stream, Sparse mode now disabled"));

      pXvdCh->stDecoderContext.bHostSparseMode = false;

      uiChannelMode = VDEC_CHANNEL_MODE_SPARSE_NORMAL;
   }
   else
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - Sparse mode FALSE"));
      uiChannelMode = VDEC_CHANNEL_MODE_SPARSE_NORMAL;
   }

   if (((pXvdCh->sDecodeSettings.eVideoCmprStd == BAVC_VideoCompressionStd_eH264) ||
        (pXvdCh->sDecodeSettings.eVideoCmprStd == BAVC_VideoCompressionStd_eMPEG2)) &&
       (pXvdCh->sDecodeSettings.bZeroDelayOutputMode == true))
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_ZERO_DELAY;
   }

   if ((pXvdCh->sDecodeSettings.eVideoCmprStd == BAVC_VideoCompressionStd_eH264) &&
       (pXvdCh->sDecodeSettings.bIFrameAsRAP == true))
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_IFRAME_AS_RAP;
   }

   if ((pXvdCh->sDecodeSettings.eVideoCmprStd == BAVC_VideoCompressionStd_eH264) &&
       (pXvdCh->sDecodeSettings.bAVCErrorConcealmentMode == true))
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_ENA_ERROR_CONCEALMENT;
   }

   if ((pXvdCh->sDecodeSettings.eVideoCmprStd == BAVC_VideoCompressionStd_eH264) &&
       (pXvdCh->sDecodeSettings.bIOnlyFieldOutputMode == true))
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_ENA_IONLY_FIELD_OUTPUT;
   }

   /* If DQT mode is enabled, tell the decoder to use all available resources. */
   if ( pXvdCh->stDecoderContext.bReversePlayback )
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_NON_LEGACY;
   }


   /* Set HITS mode if specifed in settings. */
   if (pXvdCh->sDecodeSettings.eHITSMode == BXVD_HITSMode_eLegacy)
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_HITS;
   }
   else if (pXvdCh->sDecodeSettings.eHITSMode == BXVD_HITSMode_eClean)
   {
      uiChannelMode |= (VDEC_CHANNEL_MODE_HITS | VDEC_CHANNEL_MODE_CLEAN_HITS);
   }

   /* Set BD playback mode */
   if (pXvdCh->sDecodeSettings.bBluRayDecode)
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_BLU_RAY_DECODE;
   }

   /* Set AVC in AVI mode */
   if (pXvdCh->sDecodeSettings.eTimestampMode == BXVD_TimestampMode_eDisplay)
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_TIMESTAMP_DISPLAY_ORDER;
   }

   if (pXvdCh->sDecodeSettings.uiSEIMessageFlags & BXVD_DECODESETTINGS_SEI_MESSAGE_FRAMEPACKING)
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_ENA_SEI_FRAME_PACK;
   }

   if (pXvdCh->sDecodeSettings.bPFrameSkipDisable)
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_DISABLE_P_SKIP;
   }

   if ((pXvdCh->sDecodeSettings.eVideoCmprStd == BAVC_VideoCompressionStd_eH264) &&
       (pXvdCh->sDecodeSettings.bAVCAspectRatioOverrideMode == true))
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_FILE_FORMAT;
   }

   /*
    * Check if this channel is a still picture decode channel, if so,
    * then a VEC vsync interrupt is not associated with this channel.
    */
   if ( hXvdChannel->sChSettings.eChannelMode == BXVD_ChannelMode_eStill )
   {
      uiVecIndex = BXVD_P_VEC_UNUSED;
   }
   else
   {
      uiVecIndex = eDisplayInterrupt;
   }

   /* SW3556-1058:: conditionally ignore the DPB output delay syntax */
   if (pXvdCh->sDecodeSettings.bIgnoreDPBOutputDelaySyntax == true)
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_DISABLE_DPB_OUTPUT_DELAY;
   }

   if (pXvdCh->sDecodeSettings.bSVC3DModeEnable == true)
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_3D_SVC_DECODE;
   }

   if (pXvdCh->sDecodeSettings.bSWCoefAVCDecodeModeEnable == true)
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_SW_COEF_AVC_DECODE;
   }

   if (pXvdCh->sDecodeSettings.bIgnoreNumReorderFramesEqZero == true)
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_IGN_NUM_REORDR_FRM_ZERO;
   }

   if (pXvdCh->sDecodeSettings.bEarlyPictureDeliveryMode == true)
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_EARLY_PIC_DELIVERY;
   }

   if (pXvdCh->sDecodeSettings.bUserDataBTPModeEnable == true)
   {
      uiChannelMode |= VDEC_CHANNEL_MODE_ENA_UD_BTP;
   }

   /* SW7425-3177: map eVideoCmprStd to a BXVD_P_PPB_Protocol value. */
   rc = BXVD_P_MapToAVDProtocolEnum(
                  pXvd,
                  pXvdCh->sDecodeSettings.eVideoCmprStd,
                  &eProtocol );

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   rc = BXVD_P_HostCmdSendDecChannelStart((BXVD_Handle)pXvd,
                                          hXvdChannel->ulChannelNum,
                                          eProtocol,
                                          uiChannelMode,
                                          uiContextReg,
                                          &uiContextRegExtend[0],
                                          uiVecIndex);

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   /* Critical section to prevent ISR routines from using data not yet initialized. */

   BKNI_EnterCriticalSection();

   /* SW7340-199: reset the "DM" state cached in the channel structure. */
   
   BKNI_Memset(&(pXvdCh->stPictureParameterInfo), 0, sizeof( BXVD_PictureParameterInfo ) );
   pXvdCh->bProgressiveStream_7411 = false;
   pXvdCh->uiPPBSerialNumber = 0;

   if ( eDisplayInterrupt == BXVD_DisplayInterrupt_eZero )
   {
      BXVD_P_SAVE_XVD_CHANNEL_DISPLAY_INFO_0(pXvdCh, pXvd);
   }
   else
   {
      BXVD_P_SAVE_XVD_CHANNEL_DISPLAY_INFO_1(pXvdCh, pXvd);
   }

   /*
    * Initialize a few DM variables including the variable that
    * signifies end of all inits for DM
    */

#if BXVD_P_FW_HIM_API

   /* Using cached address space for Rev K core and later */

   BMEM_ConvertOffsetToAddress( pXvdCh->hSystemHeap, (uint32_t)pXvdCh->stDeliveryQueue.ulQueueOffset, (void *)&uiVirtAddr);
   rc = BMEM_Heap_ConvertAddressToCached( pXvdCh->hSystemHeap, (void *)uiVirtAddr, (void**)&uiVirtAddr);

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   pXvdCh->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureDeliveryQueue = (BXVD_P_PictureDeliveryQueue *)uiVirtAddr;

   BMEM_ConvertOffsetToAddress( pXvdCh->hSystemHeap, (uint32_t)pXvdCh->stReleaseQueue.ulQueueOffset, (void *)&uiVirtAddr);
   rc = BMEM_Heap_ConvertAddressToCached( pXvdCh->hSystemHeap, (void *)uiVirtAddr, (void**)&uiVirtAddr);

   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   pXvdCh->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureReleaseQueue = (BXVD_P_PictureReleaseQueue *)uiVirtAddr;

#else   

   BMEM_ConvertOffsetToAddress( pXvdCh->hSystemHeap, (uint32_t)pXvdCh->ulPicBuf, (void *)&uiVirtAddr);
   pXvdCh->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureDeliveryQueue = (BXVD_P_PictureDeliveryQueue *)uiVirtAddr;

   BMEM_ConvertOffsetToAddress( pXvdCh->hSystemHeap, (uint32_t)pXvdCh->ulPicRelBuf, (void *)&uiVirtAddr);
   pXvdCh->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureReleaseQueue = (BXVD_P_PictureReleaseQueue *)uiVirtAddr;

   BMEM_ConvertOffsetToAddress( pXvd->hCodeHeap, (uint32_t)pXvdCh->ulPicInfoRelBuf, (void *)&uiVirtAddr);
   pXvdCh->stChBufferConfig.pAVD_PI2DMS_Buffer = ( BXVD_P_DM2DMSInfo *)uiVirtAddr;

#endif

   pXvd->auiActiveDecodes[pXvdCh->sChSettings.eChannelMode][eDisplayInterrupt]++;

   BXDM_PictureProvider_SetProtocol_isr(
            pXvdCh->hPictureProvider,
            pXvdCh->sDecodeSettings.eVideoCmprStd
            );

   BXDM_PictureProvider_SetCRCMode_isr(
            pXvdCh->hPictureProvider,
            pXvdCh->sDecodeSettings.bCrcMode
            );

   BXVD_Decoder_SetCRCMode_isr(
            pXvdCh,
            pXvdCh->sDecodeSettings.bCrcMode
            );

   {
      BXDM_PictureProvider_PulldownMode ePulldownMode;

      switch ( pXvdCh->sDecodeSettings.eProgressiveOverrideMode )
      {
         case BXVD_ProgressiveOverrideMode_eBottomTop:
            ePulldownMode = BXDM_PictureProvider_PulldownMode_eBottomTop;
            break;

         case BXVD_ProgressiveOverrideMode_eDisable:
            ePulldownMode = BXDM_PictureProvider_PulldownMode_eUseEncodedFormat;
            break;

         case BXVD_ProgressiveOverrideMode_eTopBottom:
            ePulldownMode = BXDM_PictureProvider_PulldownMode_eTopBottom;
            break;

         default:
            ePulldownMode = BXDM_PictureProvider_PulldownMode_eTopBottom;
            BXVD_DBG_ERR(pXvdCh, ("Unknown progressive override mode: 0x%x (%d)",
                     pXvdCh->sDecodeSettings.eProgressiveOverrideMode, pXvdCh->sDecodeSettings.eProgressiveOverrideMode));
            break;
      }

      BXDM_PictureProvider_Set480pPulldownMode_isr(
               pXvdCh->hPictureProvider,
               ePulldownMode
               );

      BXDM_PictureProvider_Set1080pPulldownMode_isr(
               pXvdCh->hPictureProvider,
               ePulldownMode
               );
   }

   BXVD_SetSTCSource_isr(
            pXvdCh,
            pXvdCh->sDecodeSettings.eSTC
            );

   BXDM_PictureProvider_SetRemovalDelay_isr(
            pXvdCh->hPictureProvider,
            pXvdCh->sChSettings.ulRemovalDelay
            );

   if ( pXvdCh->sDecodeSettings.uiPreRollRate > 1 )
   {
      BXDM_Picture_Rate stPreRollRate;
      BKNI_Memset(&stPreRollRate, 0, sizeof( BXDM_Picture_Rate ) );

      stPreRollRate.uiNumerator = 1;
      stPreRollRate.uiDenominator = pXvdCh->sDecodeSettings.uiPreRollRate;

      BXDM_PictureProvider_SetPreRollRate_isr(
               pXvdCh->hPictureProvider,
               &stPreRollRate
               );
   }

   BXDM_PictureProvider_SetPlaybackMode_isr(
            pXvdCh->hPictureProvider,
            pXvdCh->sDecodeSettings.bPlayback
            );

   BXDM_PictureProvider_SetDefaultFrameRate_isr(
            pXvdCh->hPictureProvider,
            pXvdCh->sDecodeSettings.eDefaultFrameRate
            );

   BXDM_PictureProvider_SetFrameRateDetectionMode_isr(
            pXvdCh->hPictureProvider,
            pXvdCh->sDecodeSettings.eFrameRateDetectionMode
            );

   BXDM_PictureProvider_SetASTMMode_isr(
            pXvdCh->hPictureProvider,
            pXvdCh->sDecodeSettings.bAstmMode
            );

   BXDM_PictureProvider_SetVirtualTSMOnPCRDiscontinuityMode_isr(
            pXvdCh->hPictureProvider,
            pXvdCh->sDecodeSettings.bVsyncModeOnPcrDiscontinuity
            );

   BXDM_PictureProvider_SetChannelSyncMode_isr(
            pXvdCh->hPictureProvider,
            true
            );

   /* If DQT is enabled, force VSYNC mode. */
   if ( true == pXvdCh->stDecoderContext.bReversePlayback )
   {
      /* Set the video display mode */
      BXDM_PictureProvider_SetDisplayMode_isr(
            pXvdCh->hPictureProvider,
            BXDM_PictureProvider_DisplayMode_eVirtualTSM
            );
   }

   BXVD_SetErrorHandlingMode_isr(
            pXvdCh,
            pXvdCh->sDecodeSettings.eErrorHandling
            );

   {
      BXDM_PictureProvider_TrickMode ePictureProviderTrickMode = BXDM_PictureProvider_TrickMode_eAuto;

      /* SWDEPRECATED-1003: needed to turn off the FIC logic during certain trick modes  */
      
      if ( ( true == pXvdCh->stDecoderContext.bHostSparseMode )
           || ( BXVD_SkipMode_eDecode_IPB != pXvdCh->eCurrentSkipMode )
           || ( true == pXvdCh->stDecoderContext.bReversePlayback )
         )
      {
         ePictureProviderTrickMode = BXDM_PictureProvider_TrickMode_eSparsePictures;
      }
      
      BXDM_PictureProvider_SetTrickMode_isr(
            pXvdCh->hPictureProvider,
            ePictureProviderTrickMode
            );
   }


   {
      /* SW7405-4736: the uiInstanceId is used in debug messages to 
       * differentiate one channel from another.  Currently it is printed
       * as a 8 bit value.  Set the upper nibble to distinguish between
       * multiple decoders.
       */
      uint32_t uiInstanceId = pXvdCh->sDecodeSettings.uiVDCRectangleNum;

      uiInstanceId |= ( pXvd->uDecoderInstance & 0xF ) << 4 ;

      BXDM_PictureProvider_SetInstanceID_isr(
            pXvdCh->hPictureProvider,
            uiInstanceId
            );
   }
   
   BXVD_Decoder_StartDecode_isr( pXvdCh );

   {
      /* SW7425-1064: */
      bool bXmoInstalled;

      /* SW7425-1064: true if this is the base channel of a pair of channels with the XMO installed. */
      bXmoInstalled = (( BXVD_P_ChannelType_eBase == pXvdCh->eChannelType ) && ( NULL != pXvdCh->hXmo )); 

      if ( true == bXmoInstalled )
      {
         BXDM_PictureProvider_ChannelChangeSettings stChannelChangeSettings;

         /* In order to release pictures properly when the XMO is installed, XDM must release all pictures
          * when StopDecode is call.  The following code forces "hold last picture" to false.
          * - get the current channel change settings.
          * - save the settings in the XMO structure
          * - set "bHoldLastPicture" to false
          * - set the channel change settings with the modified value
          */
         BXDM_PictureProvider_GetChannelChangeSettings_isr( 
                        pXvdCh->hPictureProvider, 
                        &stChannelChangeSettings );

         BXDM_PictureProvider_XMO_SaveChannelChangeSettings_isr( 
                        pXvdCh->hXmo,
                        &stChannelChangeSettings );

         stChannelChangeSettings.bHoldLastPicture = false;

         BXDM_PictureProvider_SetChannelChangeSettings_isr( 
                        pXvdCh->hPictureProvider, 
                        &stChannelChangeSettings );

         /* Start channel 0 (base) of the XMO. */
         BXDM_PictureProvider_XMO_StartDecode_isr( pXvdCh->hXmo, BXDM_PP_XMO_Base_Index );
      }

      /* SW7425-1064: true if this is the enhanced channel of a pair of channels with the XMO installed. */
      bXmoInstalled = (( BXVD_P_ChannelType_eEnhanced == pXvdCh->eChannelType ) 
                           && ( NULL != pXvdCh->hXvdChLinked )
                           && ( NULL != pXvdCh->hXvdChLinked->hXmo )); 

      if ( true == bXmoInstalled )
      {
         /* SW7425-1064: XDM on the enhanced channel is not used when running with the XMO. 
          * Force XDM to release a picture if it is being held from the previous decode.  
          * This is the only time to get the picture back for this decode.
          */
         BXDM_PictureProvider_FlushHeldPicture_isr( pXvdCh->hPictureProvider );

         /* Start channel 1 (enhanced) of the XMO. */
         BXDM_PictureProvider_XMO_StartDecode_isr( pXvdCh->hXvdChLinked->hXmo, BXDM_PP_XMO_Enhanced_Index );
      }
   }

   /* SW7425-1064: only need to start XDM on "standard" and "base" channels.
    * XDM for the "enhanced" channel is not used.
    */
   if ( BXVD_P_ChannelType_eEnhanced != pXvdCh->eChannelType )
   {
      BXDM_PictureProvider_StartDecode_isr( pXvdCh->hPictureProvider );
   }

   /* Set the decoder state to active */
   pXvdCh->eDecoderState = BXVD_P_DecoderState_eActive;

   BKNI_LeaveCriticalSection();

   /* Determine if the PictureProvider (XDM) needs to be registers/unregistered with the
    * DIH (Display Interrupt Handler) of it it needs to be re-enabled.  None of this needs
    * to happen for still pictures.
    */
   if ( pXvdCh->sChSettings.eChannelMode != BXVD_ChannelMode_eStill )
   {
      BXDM_DisplayInterruptHandler_AddPictureProviderInterface_Settings stXdmDihPictureProviderSettings;
      BXDM_DisplayInterruptHandler_Handle hXdmDih; 
      BXDM_DisplayInterruptHandler_Handle hXdmDihCurrent;

      stXdmDihPictureProviderSettings.uiVDCRectangleNumber = pXvdCh->sDecodeSettings.uiVDCRectangleNum;

      /* Determine which DIH to use. */
      if ( pXvd->hLinkedDecoderXdmDih != NULL )
      {
         hXdmDih = pXvd->hLinkedDecoderXdmDih;
         BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - selecting hLinkedDecoderXdmDih, DIH:%08x",
                                 hXdmDih));

      }
      else if ( pXvd->hAppXdmDih[eDisplayInterrupt] == NULL )
      {
         hXdmDih = pXvd->hXdmDih[eDisplayInterrupt];

         BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - selecting pXvd->hXdmDih[%d], DIH:%08x",
                                 eDisplayInterrupt,
                                 hXdmDih));
      }
      else
      {
         hXdmDih = pXvd->hAppXdmDih[eDisplayInterrupt];

         BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - selecting pXvd->hAppXdmDih[%d], DIH:%08x",
                                 eDisplayInterrupt,
                                 hXdmDih));
      }

      BXDM_PictureProvider_GetDIH( pXvdCh->hPictureProvider, &hXdmDihCurrent );

      if ( BXVD_P_ChannelType_eEnhanced == pXvdCh->eChannelType )
      {
         /* SW7425-1064: if this is the "enhanced" channel of a pair of linked channels,
          * only XDM for the base channel needs to be called.  Disable this instance
          * of XDM on the appropriate DIH.
          */
         if ( NULL != hXdmDihCurrent )
         {
            BKNI_EnterCriticalSection();

            BERR_TRACE(BXDM_DisplayInterruptHandler_SetPictureProviderMode_isr( 
                       hXdmDihCurrent,
                       BXDM_PictureProvider_GetPicture_isr,
                       pXvdCh->hPictureProvider,
                       BXDM_DisplayInterruptHandler_PictureProviderMode_eDisabled
                       ));

            BKNI_LeaveCriticalSection();

            BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - eEnhanced channel:: disabling XDM:%08x on DIH:%08x",
                                    pXvdCh->hPictureProvider,
                                    hXdmDihCurrent));
         }
      }
      else if ( hXdmDih != hXdmDihCurrent )
      {
         if ( NULL != hXdmDihCurrent )
         {
            BERR_TRACE(BXDM_DisplayInterruptHandler_RemovePictureProviderInterface(
                           hXdmDihCurrent,
                           BXDM_PictureProvider_GetPicture_isr,
                           pXvdCh->hPictureProvider));

            BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - removing XDM:%08x from DIH:%08x",
                                    pXvdCh->hPictureProvider,
                                    hXdmDihCurrent));
         }

         if ( pXvdCh->sDecodeSettings.bExternalPictureProviderMode == false )
         {
            /* External picture provider interface is being used for this decode. 
             * Do not add this decode to the default DIH */
            BXDM_PictureProvider_SetDIH( pXvdCh->hPictureProvider, hXdmDih );   

            rc = BXDM_DisplayInterruptHandler_AddPictureProviderInterface(
                     hXdmDih,
                     BXDM_PictureProvider_GetPicture_isr,
                     pXvdCh->hPictureProvider,
                     &stXdmDihPictureProviderSettings);

            BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - adding XDM:%08x to DIH:%08x",
                                    pXvdCh->hPictureProvider,
                                    hXdmDih));
            if (rc != BERR_SUCCESS)
            {
               BXVD_StopDecode(pXvdCh);
               return BERR_TRACE(rc);
            }
         }
      }
      else if ( pXvdCh->sDecodeSettings.bExternalPictureProviderMode == false )
      {
         /* SW7405-3984: the PictureProvider could have been disabled by the middleware.
          * It should always be enabled when StartDecode is called.  If it is an
          * If an external PictureProvider, it is up to the middleware to manage it.
          */
         BKNI_EnterCriticalSection();

         rc = BXDM_DisplayInterruptHandler_SetPictureProviderMode_isr( 
                     hXdmDih,
                     BXDM_PictureProvider_GetPicture_isr,
                     pXvdCh->hPictureProvider,
                     BXDM_DisplayInterruptHandler_PictureProviderMode_eEnabled
                     );

         BKNI_LeaveCriticalSection();

         BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - enabling XDM:%08x on DIH:%08x",
                                    pXvdCh->hPictureProvider,
                                    hXdmDih));
         if (rc != BERR_SUCCESS)
         {
            BXVD_StopDecode(pXvdCh);
            return BERR_TRACE(rc);
         }

      }
   }

   /* Set skip mode if current set mode is not default IPB mode */

   if ((pXvdCh->stDecoderContext.bHostSparseMode == false) &&
       (  pXvdCh->eCurrentSkipMode != BXVD_SkipMode_eDecode_IPB))
   {
      /* Save current Skipmode */
      BXVD_SkipMode ePreservedSkipMode = pXvdCh->eCurrentSkipMode;

      /* Initial skip mode when decode is started is set to IPB (Normal) */
      pXvdCh->eCurrentSkipMode = BXVD_SkipMode_eDecode_IPB;

      /* Set it back to what it was set to since the last BXVD_StopDecode call. */
      if ( BERR_SUCCESS != BXVD_SetSkipPictureModeDecode(pXvdCh, ePreservedSkipMode) )
      {
         BXVD_DBG_WRN(pXvdCh, ("BXVD_StartDecode() - Error restoring skip picture mode"));
      }
   }

   /* PreserveCounters may have been set, now clear it */
   pXvdCh->bPreserveCounters = false;

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - pDisplayInfo: 0x%x",
                              (uint32_t)pXvdCh->stChBufferConfig.AVD_DMS2PI_Buffer.pDisplayInfo));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - pPictureDeliveryQueue: 0x%x",
                              (uint32_t)pXvdCh->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureDeliveryQueue));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - pPictureReleaseQueue: 0x%x",
                              (uint32_t)pXvdCh->stChBufferConfig.AVD_DMS2PI_Buffer.pPictureReleaseQueue));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - pAVD_PI2DMS_Buffer: 0x%x",
                              (uint32_t)pXvdCh->stChBufferConfig.pAVD_PI2DMS_Buffer));

   if ( BXVD_P_ChannelType_eEnhanced != pXvdCh->eChannelType )
   {
      BXVD_Status_StartDecode(pXvd->hXvdStatus, hXvdChannel);
   }

   {
      /* SW7425-1064: */

      bool bStartEnhancedChannel;

      bStartEnhancedChannel = ( BXVD_P_ChannelType_eBase == pXvdCh->eChannelType );
      bStartEnhancedChannel &= ( NULL != pXvdCh->hXvdChLinked );
      bStartEnhancedChannel &= ( NULL != psDecodeSettings->pstEnhancedSettings );
      
      if ( true == bStartEnhancedChannel )
      {
         BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - base channel, calling StartDecode on enhanced channel %08x", 
                                       pXvdCh->hXvdChLinked ));
         rc = BXVD_StartDecode( pXvdCh->hXvdChLinked, psDecodeSettings->pstEnhancedSettings );

         if (rc != BERR_SUCCESS)
         {
            BXVD_DBG_ERR(pXvd, ("BXVD_StartDecoded failed on the enhanced channel, rc = %d", rc ));
            return BERR_TRACE(rc);
         }

      }

   }

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StartDecode() - exit" ));

   BDBG_LEAVE(BXVD_StartDecode);

   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
BXVD_StopDecode: Stops the decode on a channel.
****************************************************************************/
BERR_Code BXVD_StopDecode(BXVD_ChannelHandle hXvdChannel)
{
   /* BERR_Code eStatus = BERR_SUCCESS; */
   BXVD_P_Context *pXvd;
   BXVD_P_Channel *pXvdCh = (BXVD_P_Channel*)hXvdChannel;

   BXVD_ChannelChangeMode ChChangeMode;
   BXDM_DisplayInterruptHandler_Handle hXdmDihCurrent;

   BERR_Code rc;
   BXDM_PictureProvider_Counters stCounters;

   BDBG_ENTER(BXVD_StopDecode);

   BDBG_ASSERT(pXvdCh);

   /* Check handle type for correctness */
   if (hXvdChannel->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   pXvd = pXvdCh->pXvd; /* Get XVD Device handle */

   if ( BXVD_P_ChannelType_eEnhanced != pXvdCh->eChannelType )
   {
      BXVD_Status_StopDecode(pXvd->hXvdStatus, hXvdChannel);
   }

   if (pXvdCh->eDecoderState == BXVD_P_DecoderState_eNotActive)
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StopDecode() - Decoder is already inactive"));
      return BERR_TRACE(BXVD_ERR_DECODER_INACTIVE);
   }

   BKNI_EnterCriticalSection();

   BERR_TRACE(BXDM_PictureProvider_GetCounters_isr(
            pXvdCh->hPictureProvider,
            &stCounters
            ));

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StopDecode() - hXvdChannel = 0x%08x", hXvdChannel));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StopDecode() - Pictures Delayed: %d", stCounters.uiUnderflowCount));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StopDecode() - Decoder error count: %d", stCounters.uiDecodeErrorCount));
   BXVD_DBG_MSG(pXvdCh, ("BXVD_StopDecode() - XDM:%08x", pXvdCh->hPictureProvider));


   /* SW7425-1064: only need to stop XDM on "standard" and "base" channels.
    * XDM for the "enhanced" channel is not being used.
    */
   if ( BXVD_P_ChannelType_eEnhanced != pXvdCh->eChannelType )
   {
      BXDM_PictureProvider_StopDecode_isr(pXvdCh->hPictureProvider);
   }

   /* SW7425-1064: when stopping, XDM cannot hold onto the last picture.  This is the only
    * time to get the picture back to the enhanced channel prior to disconnecting the interfaces.
    * To enforce this, the channel change mode was set to NOT "hold last picture" in StartDecode.
    * The following code restores the channel change mode it to the original value.
    * However if a flush, then the last picture should be held, so don't do anything here.
    */
   {
      BXDM_PictureProvider_ChannelChangeSettings stChannelChangeSettings;
      bool bRestoreChannelChangeSettings;

      bRestoreChannelChangeSettings = ( BXVD_P_ChannelType_eBase == pXvdCh->eChannelType );
      bRestoreChannelChangeSettings &= ( NULL != pXvdCh->hXmo );
      bRestoreChannelChangeSettings &= ( false == pXvdCh->bPreserveState );   /* will be true for a flush */

      if ( true == bRestoreChannelChangeSettings )
      {
         BXDM_PictureProvider_XMO_RetrieveChannelChangeSettings_isr( 
                  pXvdCh->hXmo,
                  &stChannelChangeSettings );

         BXDM_PictureProvider_SetChannelChangeSettings_isr( 
                  pXvdCh->hPictureProvider,
                  &stChannelChangeSettings );
      }
   }

   /* SW7425-1064: Shutdown the XMO filter when stopping the "master" channel.
    * Note: the base channel must be stopped before the enhanced channel.
    */
   if (( NULL != pXvdCh->hXmo ) && ( BXVD_P_ChannelType_eBase == pXvdCh->eChannelType ))
   {
      BXDM_PictureProvider_XMO_StopDecode_isr( pXvdCh->hXmo, 0 );
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StopDecode() - stopping XMO:%08x", pXvdCh->hXmo ));
   }
   
   BXVD_Decoder_StopDecode_isr( pXvdCh );

   pXvd->auiActiveDecodes[pXvdCh->sChSettings.eChannelMode][pXvdCh->eDisplayInterrupt]--;

   if (pXvdCh->bPreserveState == false)
   {
      BXDM_Picture_FrameRateOverride  stFrameRateOverride;

      pXvdCh->eCurrentSkipMode = BXVD_SkipMode_eDecode_IPB;

      /* SW7405-4733: if state is NOT being preserved, the stPlaybackRate will be
       * set to 1/1 in BXDM_PictureProvider_StopDecode_isr.  Reset the pause state
       * here to keep the logic in sync.
       */
      pXvdCh->bPauseActive = false;

      /* SWDEPRECATED-1003: disable the frame rate override. */
      BKNI_Memset( &stFrameRateOverride, 0, sizeof ( BXDM_Picture_FrameRateOverride ));
      stFrameRateOverride.stRate.uiNumerator = 1;
      stFrameRateOverride.stRate.uiDenominator = 1;

      BERR_TRACE(BXDM_PictureProvider_SetFrameRateOverride_isr(
                  pXvdCh->hPictureProvider,
                  &stFrameRateOverride
                  ));
   }

   BKNI_LeaveCriticalSection();

   /* Close channel and free all outstanding pictures */
   rc = BXVD_P_HostCmdSendDecChannelStop((BXVD_P_Context*)pXvdCh->pXvd, pXvdCh->ulChannelNum);

   pXvdCh->eDecoderState = BXVD_P_DecoderState_eNotActive;

   BERR_TRACE(BXVD_GetChannelChangeMode( pXvdCh, &ChChangeMode));

   /* SW7425-1064: XDM should already be disabled or disconnnectd for the "enhanced" channnel */

   if (( ChChangeMode == BXVD_ChannelChangeMode_eMute )
        && ( BXVD_P_ChannelType_eEnhanced != pXvdCh->eChannelType ))
   {
      BXDM_PictureProvider_GetDIH( pXvdCh->hPictureProvider, &hXdmDihCurrent );

      if ( NULL != hXdmDihCurrent )
      {
         BKNI_EnterCriticalSection();

         BERR_TRACE(BXDM_DisplayInterruptHandler_SetPictureProviderMode_isr( 
                       hXdmDihCurrent,
                       BXDM_PictureProvider_GetPicture_isr,
                       pXvdCh->hPictureProvider,
                       BXDM_DisplayInterruptHandler_PictureProviderMode_eDisabled
                       ));

         BKNI_LeaveCriticalSection();

         BXVD_DBG_MSG(pXvdCh, ("BXVD_StopDecode() - disabling XDM:%08x on DIH:%08x", 
                                    pXvdCh->hPictureProvider,
                                    hXdmDihCurrent ));
      }
   }

   /* SW7425-1064: Now that the "base" channel has been stopped, call 
    * BXVD_StopDecode on the "enhanced" channel.
    */
   if ( BXVD_P_ChannelType_eBase == pXvdCh->eChannelType 
         && NULL != pXvdCh->hXvdChLinked )
   {
      BXVD_DBG_MSG(pXvdCh, ("BXVD_StopDecode() - base channel, calling StopDecode on enhanced channel %08x", 
                                    pXvdCh->hXvdChLinked ));

      BXVD_StopDecode( pXvdCh->hXvdChLinked );

#if BXVD_P_USE_TWO_DECODERS
      /* If the XVD handles don't match, the two channels are being decoded
       * on separate decoders.  The "enhanced" decoder interrupts were mucked 
       * with in BXVD_StartDecode, restore the interrupts to their original state
       * TODO: this shouldn't need to happen on a flush.
       * TODO: is the correct time for the restore?
       */
      if ( pXvdCh->pXvd != pXvdCh->hXvdChLinked->pXvd )
      {
         /* Set the interrupt settings on the enhanced channel back to their original values. */
         rc = BXVD_DisplayInterruptProvider_P_SetInterruptConfiguration(
                     pXvdCh->hXvdChLinked->pXvd->hXvdDipCh[ pXvdCh->sDecodeSettings.eDisplayInterrupt ],
                     &(pXvdCh->hXvdChLinked->stEnhancedInterruptConfig)
                     );

         /* TODO: Could this call fail? If so, what are the appropriate actions? */
         if (rc != BERR_SUCCESS )
         {
            BXVD_DBG_ERR(pXvdCh, ("BXVD_StopDecode->BXVD_DisplayInterruptProvider_P_SetInterruptConfiguration returned %d on the base decoder", rc));
            return BERR_TRACE(rc);
         }
      }
#endif
   }

   BXVD_DBG_MSG(pXvdCh, ("BXVD_StopDecode() - exit" ));

   BDBG_LEAVE(BXVD_StopDecode);
   return BERR_TRACE(rc);
}

/***************************************************************************
BXVD_GetChannelStatus: Requests the channel status from the firmware.
****************************************************************************/
BERR_Code BXVD_GetChannelStatus(BXVD_ChannelHandle hXvdCh,
                                BXVD_ChannelStatus *psChannelStatus )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetChannelStatus);

   BKNI_EnterCriticalSection();
   rc = BXVD_GetChannelStatus_isr(
      hXvdCh,
      psChannelStatus
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetChannelStatus);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetChannelStatus_isr(BXVD_ChannelHandle hXvdCh,
                                    BXVD_ChannelStatus *psChannelStatus )
{
   uint32_t uiPictureDeliveryCount;
   BXDM_PictureProvider_Counters stCounters;
   BXVD_Decoder_Counters         stDecoderCounters;
   const BXDM_Picture * pstPicture;
   BXVD_Handle hXvd;
   uint32_t uiOffset;

   BDBG_ENTER(BXVD_GetChannelStatus_isr);
   BDBG_ASSERT(psChannelStatus);

   if (hXvdCh == NULL)
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   hXvd = hXvdCh->pXvd;

   if ( hXvd == NULL )
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Get the number of pictures on the Unified Picture Queue. */
   BXVD_Decoder_GetPictureCount_isr(
            hXvdCh,
            &uiPictureDeliveryCount);

   /* SWDTV-7262: Get the number of pictures on the Delivery Queue. */
   if ( true == hXvdCh->stDecoderContext.bDecoderHasBeenInitialized )
   {
      uint32_t uiWriteOffset;
      uint32_t uiReadOffset;
      uint32_t uiDepth;

      BXVD_P_DELIVERY_QUEUE_GET_READ_OFFSET( hXvdCh, uiReadOffset );
      BXVD_P_DELIVERY_QUEUE_GET_WRITE_OFFSET( hXvdCh, uiWriteOffset );

      uiReadOffset -= BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;
      uiWriteOffset -= BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE;

      BXVD_P_GET_QUEUE_DEPTH( uiReadOffset, uiWriteOffset, uiDepth );

      /* SW7231-788: for SVC 3D and MVC, the Delivery Queue contains pairs of pictures.  
       * Divide uiDepth by 2 to return the number of pictures instead of the number of PPB's.  
       * This has the added benefit of returning '0' if a single base picture is sitting 
       * at the end of the queue, i.e. an incomplete pair of pictures.
       */
      if  ( ( true == hXvdCh->sDecodeSettings.bSVC3DModeEnable )
            || ( BAVC_VideoCompressionStd_eMVC == hXvdCh->sDecodeSettings.eVideoCmprStd )
          )
      {
         uiDepth /= 2 ;
      }

      /* The number of outstanding pictures is equal to the sum on the Unified
       * Picture Queue and the Delivery Queue.
       */
      uiPictureDeliveryCount += uiDepth;
   }

   /*
    * Copy the DM state information into the channel status structure.
    */

   BERR_TRACE(BXDM_PictureProvider_GetCounters_isr(
               hXvdCh->hPictureProvider,
               &stCounters
               ));

   BXDM_PictureProvider_GetCurrentPicturePtr_isr(
               hXvdCh->hPictureProvider,
               &pstPicture
               );

   BERR_TRACE(BXVD_Decoder_GetCounters_isr(
               hXvdCh,
               &stDecoderCounters
               ));

   psChannelStatus->uiDisplayManagerDroppedCount = stCounters.uiDisplayManagerDroppedCount;

   psChannelStatus->uiDecoderDroppedCount = stCounters.uiDecoderDroppedCount;

   psChannelStatus->uiDisplayedParityFailureCount = stCounters.uiDisplayedParityFailureCount;

   psChannelStatus->uiDisplayedCount = stCounters.uiDisplayedCount;

   psChannelStatus->uiPicturesDecodedCount = stCounters.uiPicturesDecodedCount;

   psChannelStatus->uiVsyncUnderflowCount = stCounters.uiVsyncUnderflowCount;

   psChannelStatus->uiDecodeErrorCount = stCounters.uiDecodeErrorCount;

   psChannelStatus->uiVsyncCount = stCounters.uiVsyncCount;

   psChannelStatus->uiIFrameCount = stCounters.uiIFrameCount;

   psChannelStatus->uiErrorIFrameCount = stCounters.uiErrorIFrameCount;

   /* SW7425-3558: use a pointer to save the overhead of a copy. */
   if ( NULL != pstPicture )
   {
      psChannelStatus->eVideoProtocol = pstPicture->stProtocol.eProtocol;
      psChannelStatus->eProtocolLevel = pstPicture->stProtocol.eLevel;
      psChannelStatus->eProtocolProfile = pstPicture->stProtocol.eProfile;
   }
   else
   {
      psChannelStatus->eVideoProtocol = 0;
      psChannelStatus->eProtocolLevel = 0;
      psChannelStatus->eProtocolProfile = 0;
   }

   psChannelStatus->uiPicturesReceivedCount = stCounters.uiPicturesReceivedCount;

   psChannelStatus->ulPictureDeliveryCount = uiPictureDeliveryCount;

   psChannelStatus->ulUnderflowCount = stCounters.uiUnderflowCount;

   psChannelStatus->uiDecoderInputOverflow = stDecoderCounters.uiDecoderInputOverflow;

   /*
   ** Retrieve the CABAC bin buffer depth for this channel.
   ** The register offset is calculated as follows;
   **   uiDecode_CabacBinDepth + ( ( 16 + ulChannelNum ) * 4 )
   **
   ** The *4 is since each location is 32-bits (4 bytes).
   */
   uiOffset = ( hXvdCh->ulChannelNum + 16 ) * 4;
   uiOffset += hXvd->stPlatformInfo.stReg.uiDecode_CabacBinDepth;

   psChannelStatus->uiCabacBinDepth = BXVD_Reg_Read32( hXvdCh->pXvd, uiOffset );

   BXVD_P_GET_VIDEO_DECODER_STATUS( hXvdCh, psChannelStatus->uiAVDStatusBlock );

   BDBG_LEAVE(BXVD_GetChannelStatus_isr);

   return BERR_TRACE(BERR_SUCCESS);

}

/***************************************************************************
BXVD_DisableForFlush: Stops decode on a channel.
****************************************************************************/
BERR_Code BXVD_DisableForFlush(BXVD_ChannelHandle hXvdChannel)
{
   BERR_Code rc;
   BXDM_PictureProvider_PreserveStateSettings stPreserveStateSettings;

   BDBG_ENTER(BXVD_DisableForFlush);

   /* Check handle type for correctness */
   if (hXvdChannel->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* Save currently set channel change mode, then set to hold last frame for flush */
   BERR_TRACE(BXDM_PictureProvider_GetDefaultPreserveStateSettings(
            hXvdChannel->hPictureProvider,
            &stPreserveStateSettings
            ));

   BKNI_EnterCriticalSection();

   stPreserveStateSettings.bDisplay = true;

   BERR_TRACE(BXDM_PictureProvider_SetPreserveStateSettings_isr(
            hXvdChannel->hPictureProvider,
            &stPreserveStateSettings
            ));

   BKNI_LeaveCriticalSection();

   /* Don't reset decoder settings when stopping decode for a flush */
   hXvdChannel->bPreserveState = true;

   rc = BERR_TRACE(BXVD_StopDecode(hXvdChannel));

   hXvdChannel->bPreserveState = false;

   BDBG_LEAVE(BXVD_DisableForFlush);
   return BERR_TRACE( rc );
}


/***************************************************************************
BXVD_FlushDecode: Flushes the decode on a channel.
****************************************************************************/
BERR_Code BXVD_FlushDecode(BXVD_ChannelHandle hXvdChannel)
{
   BERR_Code rc;

   BAVC_XptContextMap  XptContextMap;
   BAVC_XptContextMap  aXptContextMap_Extended[BXVD_NUM_EXT_RAVE_CONTEXT];

   uint32_t i;

   BDBG_ENTER(BXVD_FlushDecode);

   /* Check handle type for correctness */
   if (hXvdChannel->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* Reset XPT Rave CDB read register address */
   XptContextMap.CDB_Read = hXvdChannel->ulXptCDB_Read;

   hXvdChannel->sDecodeSettings.pContextMap = &XptContextMap;

   for (i = 0; i < hXvdChannel->sDecodeSettings.uiContextMapExtNum; i++)
   {
      hXvdChannel->sDecodeSettings.aContextMapExtended[i] = &aXptContextMap_Extended[i];
      aXptContextMap_Extended[i].CDB_Read = hXvdChannel->aulXptCDB_Read_Extended[i];
   }

   hXvdChannel->bPreserveState = true;

   rc = BERR_TRACE(BXVD_StartDecode(hXvdChannel,
                                    &hXvdChannel->sDecodeSettings));

   hXvdChannel->bPreserveState = false;

   BDBG_LEAVE(BXVD_FlushDecode);
   return BERR_TRACE(rc);
}

/***************************************************************************
BXVD_GetDecodeSettings: Returns the last decode settings for a specific
                        channel.
****************************************************************************/
BERR_Code BXVD_GetDecodeSettings(const BXVD_ChannelHandle hXvdCh,
                                 BXVD_DecodeSettings      *psDecodeSettings)
{
   BDBG_ENTER(BXVD_GetDecodeSettings);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(psDecodeSettings);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   BKNI_Memcpy((void *)psDecodeSettings,
               (void *)&hXvdCh->sDecodeSettings,
               sizeof(BXVD_DecodeSettings));

   BDBG_LEAVE(BXVD_GetDecodeSettings);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
BXVD_GetTotalChannels: API used to retrieve the max video channels supported.
****************************************************************************/
BERR_Code BXVD_GetTotalChannels(BXVD_Handle hXvd,
                                unsigned    *puiTotalChannels)
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetTotalChannels);

   rc = BXVD_GetTotalChannels_isr(
      hXvd,
      puiTotalChannels
      );

   BDBG_LEAVE(BXVD_GetTotalChannels);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetTotalChannels_isr(BXVD_Handle hXvd,
                                    unsigned    *puiTotalChannels)
{
   BDBG_ENTER(BXVD_GetTotalChannels_isr);
   BSTD_UNUSED(hXvd);

   BDBG_ASSERT(hXvd);
   BDBG_ASSERT(puiTotalChannels);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   *puiTotalChannels = BXVD_MAX_VIDEO_CHANNELS;

   BDBG_LEAVE(BXVD_GetTotalChannels_isr);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
BXVD_GetRevision: API used to retrieve the 7401 FW version
****************************************************************************/
BERR_Code BXVD_GetRevision(BXVD_Handle       hXvd,
                           BXVD_RevisionInfo *psRevInfo)
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetRevision);

   rc = BXVD_GetRevision_isr(
      hXvd,
      psRevInfo
      );

   BDBG_LEAVE(BXVD_GetRevision);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetRevision_isr(BXVD_Handle       hXvd,
                               BXVD_RevisionInfo *psRevInfo)
{
   BDBG_ENTER(BXVD_GetRevision_isr);

   BDBG_ASSERT(hXvd);
   BDBG_ASSERT(psRevInfo);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   *psRevInfo = hXvd->sRevisionInfo;

   BDBG_LEAVE(BXVD_GetRevision_isr);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
BXVD_RegisterVdcInterrupt: This API is used to register the VDC interrupt
                           with the video firmware.
****************************************************************************/
BERR_Code BXVD_RegisterVdcInterrupt(BXVD_ChannelHandle hXvdCh,
                                    BINT_Id            VDCIntId,
                                    BAVC_Polarity      eFieldPolarity)
{
   uint32_t value;
   BXVD_DisplayInterruptProvider_P_InterruptSettings stInterruptConfig;

   BERR_Code rc;

   BXVD_Handle hXvd;
   BDBG_ENTER(BXVD_RegisterVdcInterrupt);

   BDBG_ASSERT(hXvdCh);
   BSTD_UNUSED(eFieldPolarity);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   hXvd = hXvdCh->pXvd;

   /* Compare the beginning of the strings */
   value = BCHP_INT_ID_GET_SHIFT(VDCIntId);

   /* Adjust the value so the parity of the next vsync can be recognized */
   value = 1 << value;

   /* Write the top field polarity registers */

   /* Check to see if the polarity changed and fix it */
   /* ssavekar: 09/20/2005: changing the code to set variables in
      structure "stRULIDMasks" */
   BXVD_DisplayInterruptProvider_P_GetInterruptConfiguration( hXvd->hXvdDipCh[BXVD_DisplayInterrupt_eZero],
                                                              &stInterruptConfig );

   if (eFieldPolarity == BAVC_Polarity_eBotField)
   {
      stInterruptConfig.stRULIDMasks.ui32BottomFieldRULIDMask = value ;
   }

   if (eFieldPolarity == BAVC_Polarity_eTopField)
   {
      stInterruptConfig.stRULIDMasks.ui32TopFieldRULIDMask = value ;
   }

   if (eFieldPolarity == BAVC_Polarity_eFrame)
   {
      stInterruptConfig.stRULIDMasks.ui32ProgressiveFieldRULIDMask = value ;
   }

   /* Link DIH to DIP */
   /* TODO: Allow external DIH to be passed in */
   rc = BXVD_DisplayInterruptProvider_InstallCallback_DisplayInterrupt(
            hXvd->hXvdDipCh[BXVD_DisplayInterrupt_eZero],
            BXDM_DisplayInterruptHandler_Callback_isr,
            hXvd->hXdmDih[BXVD_DisplayInterrupt_eZero] );

   if (rc != BERR_SUCCESS )
   {
      return BERR_TRACE(rc);
   }

   rc = BXVD_DisplayInterruptProvider_P_SetInterruptConfiguration( hXvd->hXvdDipCh[BXVD_DisplayInterrupt_eZero],
                                                                   &stInterruptConfig );
   if (rc != BERR_SUCCESS )
   {
      return BERR_TRACE(rc);
   }

   BDBG_LEAVE(BXVD_RegisterVdcInterrupt);

   return BERR_TRACE(rc);
}

/***************************************************************************************
BXVD_GetVdcDeviceInterruptDefaultSettings: Gets the DeviceVdcInterrupt default settings
***************************************************************************************/
BERR_Code BXVD_GetVdcDeviceInterruptDefaultSettings
(
   BXVD_Handle                      hXvd,
   BXVD_DeviceVdcInterruptSettings  *pDefDevVdcIntrSettings
   )
{
   BDBG_ENTER(BXVD_GetVdcDeviceInterruptDefaultSettings);
   BSTD_UNUSED(hXvd);

   BDBG_ASSERT(hXvd);

   BDBG_ASSERT(pDefDevVdcIntrSettings);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   BKNI_Memcpy((void *)pDefDevVdcIntrSettings,
               (void *)&s_stDefaultDevVdcIntrSettings,
               sizeof(BXVD_DeviceVdcInterruptSettings));

   BDBG_LEAVE(BXVD_GetVdcDeviceInterruptDefaultSettings);

   return BERR_TRACE(BERR_SUCCESS);
}



/***************************************************************************
BXVD_RegisterVdcInterrupt: This API is used to register the VDC interrupt
                           with the video firmware.
****************************************************************************/
BERR_Code BXVD_RegisterVdcDeviceInterrupt
(
   BXVD_Handle                      hXvd,
   BXVD_DeviceVdcInterruptSettings  *pDevVdcIntrSettings
   )
{
   uint32_t uiValue;
   uint32_t uiRegMask = 0;
   uint32_t uiCurentRULIDMask;
   BXVD_DisplayInterruptProvider_P_InterruptSettings stInterruptConfig;
   BXDM_DisplayInterruptHandler_Handle hXdmDih; 

#if BXVD_P_POWER_MANAGEMENT
   bool bSavedHibernate = false;
#endif

   BERR_Code rc;

   BDBG_ENTER(BXVD_RegisterVdcInterrupt);

   BDBG_ASSERT(hXvd);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* If top/bottom field bits are to be treated as frame for Graphic processing, set frame mask */
   if ((pDevVdcIntrSettings->uiFlags & BXVD_DeviceVdcIntrSettingsFlags_UseFieldAsFrame) &&
       (pDevVdcIntrSettings->VDCIntId_Frame != 0))
   {
      BXVD_DBG_ERR(hXvd, ("Invalid FieldAsFrame mask, Frame bit specified for UseFieldAsFrame"));

      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   BXVD_DisplayInterruptProvider_P_GetInterruptConfiguration( hXvd->hXvdDipCh[pDevVdcIntrSettings->eDisplayInterrupt],
                                                              &stInterruptConfig );

   /* Save current RULID Mask */
   uiCurentRULIDMask = (stInterruptConfig.stRULIDMasks.ui32TopFieldRULIDMask |
                        stInterruptConfig.stRULIDMasks.ui32BottomFieldRULIDMask |
                        stInterruptConfig.stRULIDMasks.ui32ProgressiveFieldRULIDMask);

   if (pDevVdcIntrSettings->VDCIntId_Topfield != 0)
   {
      /* Save Top field mask */
      uiValue = BCHP_INT_ID_GET_SHIFT(pDevVdcIntrSettings->VDCIntId_Topfield);

      uiValue = 1 << uiValue;

      uiRegMask = uiValue;

      stInterruptConfig.stRULIDMasks.ui32TopFieldRULIDMask = uiValue;
   }
   else
   {
      stInterruptConfig.stRULIDMasks.ui32TopFieldRULIDMask = 0;
   }

   if (pDevVdcIntrSettings->VDCIntId_Botfield != 0)
   {
      /* Save bottom field mask */
      uiValue = BCHP_INT_ID_GET_SHIFT(pDevVdcIntrSettings->VDCIntId_Botfield);

      uiValue = 1 << uiValue;

      stInterruptConfig.stRULIDMasks.ui32BottomFieldRULIDMask = uiValue;

      uiRegMask |= uiValue;
   }
   else
   {
      stInterruptConfig.stRULIDMasks.ui32BottomFieldRULIDMask = 0;
   }

   if (pDevVdcIntrSettings->VDCIntId_Frame != 0)
   {
      /* Save bottom field mask */
      uiValue = BCHP_INT_ID_GET_SHIFT(pDevVdcIntrSettings->VDCIntId_Frame);

      uiValue = 1 << uiValue;

      stInterruptConfig.stRULIDMasks.ui32ProgressiveFieldRULIDMask = uiValue;

      uiRegMask |= uiValue;
   }
   else
   {
      stInterruptConfig.stRULIDMasks.ui32ProgressiveFieldRULIDMask = 0;
   }

   /* If top/bottom field bits are to be treated as frame for Graphic processing, set frame mask */
   if (pDevVdcIntrSettings->uiFlags & BXVD_DeviceVdcIntrSettingsFlags_UseFieldAsFrame)
   {
      stInterruptConfig.stRULIDMasks.ui32ProgressiveFieldRULIDMask = uiRegMask;
      stInterruptConfig.stRULIDMasks.ui32TopFieldRULIDMask = 0;
      stInterruptConfig.stRULIDMasks.ui32BottomFieldRULIDMask = 0;
   }

#if BXVD_P_POWER_MANAGEMENT
   if (hXvd->bHibernate == true)
   {
      bSavedHibernate = true;

      /* Wake up decoder */
      BXVD_P_SetHibernateState(hXvd, false);
   }
#endif

   if (!(pDevVdcIntrSettings->uiFlags & BXVD_DeviceVdcIntrSettingsFlags_Linked))
   {
      /* Clear linked hXvdDih that maybe set */
      hXvd->hLinkedDecoderXdmDih = (BXDM_DisplayInterruptHandler_Handle) NULL;

      /* Link DIH to DIP */
      if (pDevVdcIntrSettings->hAppXdmDih == NULL)
      {
         hXdmDih = hXvd->hXdmDih[pDevVdcIntrSettings->eDisplayInterrupt];
         hXvd->hAppXdmDih[pDevVdcIntrSettings->eDisplayInterrupt] = NULL;
      }
      else 
      {
         hXdmDih = pDevVdcIntrSettings->hAppXdmDih;
         hXvd->hAppXdmDih[pDevVdcIntrSettings->eDisplayInterrupt] = hXdmDih;
      }

      rc = BXVD_DisplayInterruptProvider_InstallCallback_DisplayInterrupt(
                hXvd->hXvdDipCh[pDevVdcIntrSettings->eDisplayInterrupt],
                BXDM_DisplayInterruptHandler_Callback_isr,
                hXdmDih );

      if (rc != BERR_SUCCESS )
      {
         return BERR_TRACE(rc);
      }
   }

   rc = BXVD_DisplayInterruptProvider_P_SetInterruptConfiguration( hXvd->hXvdDipCh[pDevVdcIntrSettings->eDisplayInterrupt],
                                                                   &stInterruptConfig );
   if (rc != BERR_SUCCESS )
   {
      return BERR_TRACE(rc);
   }

#if BXVD_P_POWER_MANAGEMENT

   if (bSavedHibernate == true)
   {
      /* Put decoder back in hibernate state */
      BXVD_P_SetHibernateState(hXvd, true);
   }
#endif

   BDBG_LEAVE(BXVD_RegisterVdcDeviceInterrupt);

   return BERR_TRACE(rc);
}

/***************************************************************************
BXVD_InstallDeviceInterruptCallback: Used to enable and install an application
                                     callback for Device relevant Interrupt.
****************************************************************************/
BERR_Code BXVD_InstallDeviceInterruptCallback(BXVD_Handle           hXvd,
                                              BXVD_DeviceInterrupt  eInterrupt,
                                              BXVD_CallbackFunc     fCallBack,
                                              void                  *pParm1,
                                              int                   parm2)
{
#if BXVD_P_POWER_MANAGEMENT
   bool bSavedHibernate = false;
#endif

   BERR_Code rc;
   struct BXVD_P_InterruptCallbackInfo *callback;

   BDBG_ENTER(BXVD_InstallDeviceInterruptCallback);

   BDBG_ASSERT(hXvd);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   if (eInterrupt >= BXVD_DeviceInterrupt_eMaxInterrupts) {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

#if BXVD_P_POWER_MANAGEMENT
   if (hXvd->bHibernate == true)
   {
      bSavedHibernate = true;

      /* Wake up decoder */
      BXVD_P_SetHibernateState(hXvd, false);
   }
#endif

   callback = &hXvd->stDeviceInterruptCallbackInfo[eInterrupt];

   switch (eInterrupt)
   {
      case BXVD_DeviceInterrupt_eWatchdog:

         rc = BERR_TRACE(BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_Watchdog_ISR));
         if (rc != BERR_SUCCESS )
         {
            return BERR_TRACE(rc);
         }
         break ;

      case BXVD_DeviceInterrupt_eVidInstrChecker:

         rc = BERR_TRACE(BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_VICHReg_ISR));
         if (rc != BERR_SUCCESS )
         {
            return BERR_TRACE(rc);
         }

         rc = BERR_TRACE(BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_VICHSCB_ISR));
         if (rc != BERR_SUCCESS )
         {
            return BERR_TRACE(rc);
         }

         rc = BERR_TRACE(BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_VICHInstrRd_ISR));
         if (rc != BERR_SUCCESS )
         {
            return BERR_TRACE(rc);
         }

#if BXVD_P_AVD_ARC600
         /* VICH Inner Loop Instruction read violation */
         rc = BERR_TRACE(BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_VICHILInstrRd_ISR));
         if (rc != BERR_SUCCESS)
         {
            return BERR_TRACE(rc);
         }     

         if (hXvd->bSVCCapable == true)
         {
            /* VICH Base Layer Instruction read violation */
            rc = BERR_TRACE(BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_VICHBLInstrRd_ISR));
            if (rc != BERR_SUCCESS)
            {
               return BERR_TRACE(rc);
            }
         }
#endif
         break ;

      case BXVD_DeviceInterrupt_eStereoSeqError:

         rc = BERR_TRACE(BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_StereoSeqError_ISR));
         if (rc != BERR_SUCCESS )
         {
            return BERR_TRACE(rc);
         }

         break ;

      default:
         break;
   }

   switch (eInterrupt)
   {
      case BXVD_DeviceInterrupt_eDisplayInterrupt0:
         BXDM_DisplayInterruptHandler_InstallCallback_DisplayInterrupt(
                  hXvd->hXdmDih[BXVD_DisplayInterrupt_eZero],
                  (BXDM_DisplayInterruptHandler_Display_isr) fCallBack,
                  pParm1,
                  parm2);
         break;

      case BXVD_DeviceInterrupt_eDisplayInterrupt1:
         BXDM_DisplayInterruptHandler_InstallCallback_DisplayInterrupt(
                  hXvd->hXdmDih[BXVD_DisplayInterrupt_eOne],
                  (BXDM_DisplayInterruptHandler_Display_isr) fCallBack,
                  pParm1,
                  parm2);
         break;

      case BXVD_DeviceInterrupt_ePictureDataReady0:
         BXDM_DisplayInterruptHandler_InstallCallback_PictureDataReadyInterrupt(
                  hXvd->hXdmDih[BXVD_DisplayInterrupt_eZero],
                  (BXDM_DisplayInterruptHandler_PictureDataReady_isr) fCallBack,
                  pParm1,
                  parm2);
         break;

      case BXVD_DeviceInterrupt_ePictureDataReady1:
         BXDM_DisplayInterruptHandler_InstallCallback_PictureDataReadyInterrupt(
                  hXvd->hXdmDih[BXVD_DisplayInterrupt_eOne],
                  (BXDM_DisplayInterruptHandler_PictureDataReady_isr) fCallBack,
                  pParm1,
                  parm2);
         break;

      default:
         BKNI_EnterCriticalSection();
         callback->BXVD_P_pAppIntCallbackPtr = (BXVD_IntCallbackFunc)fCallBack;
         callback->pParm1 = pParm1;
         callback->parm2 = parm2;
         BKNI_LeaveCriticalSection();
         break;
   }

#if BXVD_P_POWER_MANAGEMENT

   if (bSavedHibernate == true)
   {
      /* Put decoder back in hibernate state */
      BXVD_P_SetHibernateState(hXvd, true);
   }
#endif

   BDBG_LEAVE(BXVD_InstallDeviceInterruptCallback);

   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
BXVD_UnInstallInterruptCallback: Used to disable and un-install an application
                                 callback for a device relevant Interrupt.
****************************************************************************/
BERR_Code BXVD_UnInstallDeviceInterruptCallback(BXVD_Handle           hXvd,
                                                BXVD_DeviceInterrupt  eInterrupt)
{
   struct BXVD_P_InterruptCallbackInfo *callback;


   BERR_Code eStatus = BERR_SUCCESS;

#if BXVD_P_POWER_MANAGEMENT
   bool bSavedHibernate = false;
#endif

   BDBG_ENTER(BXVD_UnInstallDeviceInterruptCallback);

   BDBG_ASSERT(hXvd);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   if (eInterrupt >= BXVD_DeviceInterrupt_eMaxInterrupts) {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

#if BXVD_P_POWER_MANAGEMENT
   if (hXvd->bHibernate == true)
   {
      bSavedHibernate = true;

      /* Wake up decoder */
      BXVD_P_SetHibernateState(hXvd, false);
   }
#endif


   callback = &hXvd->stDeviceInterruptCallbackInfo[eInterrupt];

   switch (eInterrupt)
   {
      case BXVD_DeviceInterrupt_eWatchdog:
         eStatus = BERR_TRACE(BINT_DisableCallback((hXvd->stDecoderContext.pCbAVC_Watchdog_ISR)));
         break ;

      case BXVD_DeviceInterrupt_eVidInstrChecker:

         eStatus = BERR_TRACE(BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_VICHReg_ISR));
         if (eStatus != BERR_SUCCESS )
         {
            return BERR_TRACE(eStatus);
         }

         eStatus = BERR_TRACE(BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_VICHSCB_ISR));
         if (eStatus != BERR_SUCCESS )
         {
            return BERR_TRACE(eStatus);
         }

         eStatus = BERR_TRACE(BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_VICHInstrRd_ISR));
         if (eStatus != BERR_SUCCESS )
         {
            return BERR_TRACE(eStatus);
         }

#if BXVD_P_AVD_ARC600
         /* VICH Inner Loop Instruction read violation */
         eStatus = BERR_TRACE(BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_VICHILInstrRd_ISR));
         if (eStatus != BERR_SUCCESS)
         {
            return BERR_TRACE(eStatus);
         }     

         if (hXvd->bSVCCapable == true)
         {
            /* VICH Base Layer Instruction read violation */
            eStatus = BERR_TRACE(BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_VICHBLInstrRd_ISR));
            if (eStatus!= BERR_SUCCESS)
            {
               return BERR_TRACE(eStatus);
            }
         }
#endif
         break;

      case BXVD_DeviceInterrupt_eStereoSeqError:

         eStatus = BERR_TRACE(BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_StereoSeqError_ISR));
         if (eStatus != BERR_SUCCESS )
         {
            return BERR_TRACE(eStatus);
         }

         break ;

      default:
         break;
   }

   switch (eInterrupt)
   {
      case BXVD_DeviceInterrupt_eDisplayInterrupt0:
         BXDM_DisplayInterruptHandler_UnInstallCallback_DisplayInterrupt(
                  hXvd->hXdmDih[BXVD_DisplayInterrupt_eZero]);
         break;

      case BXVD_DeviceInterrupt_eDisplayInterrupt1:
         BXDM_DisplayInterruptHandler_UnInstallCallback_DisplayInterrupt(
                  hXvd->hXdmDih[BXVD_DisplayInterrupt_eOne]);
         break;

      case BXVD_DeviceInterrupt_ePictureDataReady0:
         BXDM_DisplayInterruptHandler_UnInstallCallback_PictureDataReadyInterrupt(
                  hXvd->hXdmDih[BXVD_DisplayInterrupt_eZero]);
         break;

      case BXVD_DeviceInterrupt_ePictureDataReady1:
         BXDM_DisplayInterruptHandler_UnInstallCallback_PictureDataReadyInterrupt(
                  hXvd->hXdmDih[BXVD_DisplayInterrupt_eOne]);
         break;

      default:
         break;
   }
   BKNI_EnterCriticalSection();
   callback->BXVD_P_pAppIntCallbackPtr = NULL;
   callback->pParm1 = NULL;
   callback->parm2 = (int) NULL;
   BKNI_LeaveCriticalSection();

#if BXVD_P_POWER_MANAGEMENT

   if (bSavedHibernate == true)
   {
      /* Put decoder back in hibernate state */
      BXVD_P_SetHibernateState(hXvd, true);
   }
#endif

   BDBG_LEAVE(BXVD_UnInstallDeviceInterruptCallback);

   return eStatus;
}

static void BXVD_S_FirstPtsReady_XDMCompatibility_isr(
         void *pPrivateContext,
         int32_t iPrivateParam,
         const BXDM_PictureProvider_PTSInfo *pstPTSInfo )
{
   BXVD_ChannelHandle hXvdCh = (BXVD_ChannelHandle) pPrivateContext;
   BXVD_Interrupt eInterrupt = (BXVD_Interrupt) iPrivateParam;
   struct BXVD_P_InterruptCallbackInfo *callback = &hXvdCh->stInterruptCallbackInfo[eInterrupt];
   BXDM_PictureProvider_PTSInfo stPTSInfo = *pstPTSInfo;

   /* The XVD FirstPTSReady callback is overloaded.  This logic implements the overloaded functionality
    * using the explicit XDM FirstPTSReady and FirstCodedPTSReady callbacks
    */
   if ( BXDM_PictureProvider_PTSType_eCoded == pstPTSInfo->ePTSType )
   {
      /* We already have a coded PTS, so we DISABLE the XDM FirstCodedPTSReady callback so that the
       * XVD FirstPTSReady callback is backwards compatible.
       */
      BXDM_PictureProvider_Callback_SetEnable_isr(
               hXvdCh->hPictureProvider,
               BXDM_PictureProvider_Callback_eFirstCodedPTSReady,
               false
               );
   }
   else
   {
      /* We don't have a coded PTS, so we ENABLE the XDM FirstCodedPTSReady callback so that the
       * XVD FirstPTSReady callback is backwards compatible.  When we get a coded PTS, XDM will
       * call back into the app accordingly
       */
      BXDM_PictureProvider_Callback_SetEnable_isr(
               hXvdCh->hPictureProvider,
               BXDM_PictureProvider_Callback_eFirstCodedPTSReady,
               true
               );
   }

   /* Forward PTS info to the FirstPTSReady app callback */
   if ( NULL != callback->BXVD_P_pAppIntCallbackPtr )
   {
      callback->BXVD_P_pAppIntCallbackPtr(
               callback->pParm1,
               callback->parm2,
               &stPTSInfo);
   }

   return;
}

static void BXVD_S_ClipEvent_XDMCompatibility_isr(
         void *pPrivateContext,
         int32_t iPrivateParam,
         const BXDM_PictureProvider_Callback_ClipEventInfo *pstClipEventInfo )
{
   BXVD_ChannelHandle hXvdCh = (BXVD_ChannelHandle) pPrivateContext;
   BXVD_Interrupt eInterrupt = (BXVD_Interrupt) iPrivateParam;
   struct BXVD_P_InterruptCallbackInfo *callback = &hXvdCh->stInterruptCallbackInfo[eInterrupt];
   uint32_t uiPTS = pstClipEventInfo->uiPTS;

   /* Forward PTS info to the FirstPTSReady app callback */
   if ( NULL != callback->BXVD_P_pAppIntCallbackPtr )
   {
      callback->BXVD_P_pAppIntCallbackPtr(
               callback->pParm1,
               callback->parm2,
               &uiPTS);
   }

   return;
}

static void BXVD_S_RequestSTC_XDMCompatibility_isr(
         void *pPrivateContext,
         int32_t iPrivateParam,
         const BXDM_PictureProvider_PTSInfo *pstPTSInfo )
{
   BXVD_ChannelHandle hXvdCh = (BXVD_ChannelHandle) pPrivateContext;
   BXVD_Interrupt eInterrupt = (BXVD_Interrupt) iPrivateParam;
   struct BXVD_P_InterruptCallbackInfo *callback = &hXvdCh->stInterruptCallbackInfo[eInterrupt];
   uint32_t uiPTS = pstPTSInfo->ui32RunningPTS;

   /* Forward PTS info to the FirstPTSReady app callback */
   if ( NULL != callback->BXVD_P_pAppIntCallbackPtr )
   {
      callback->BXVD_P_pAppIntCallbackPtr(
               callback->pParm1,
               callback->parm2,
               &uiPTS);
   }

   return;
}

static void BXVD_S_DecodeError_XDMCompatibility_isr(
         void *pPrivateContext,
         int32_t iPrivateParam,
         const BXDM_PictureProvider_Callback_DecodeErrorInfo *pstDecodeErrorInfo
         )
{
   BXVD_ChannelHandle hXvdCh = (BXVD_ChannelHandle) pPrivateContext;
   BXVD_Interrupt eInterrupt = (BXVD_Interrupt) iPrivateParam;
   struct BXVD_P_InterruptCallbackInfo *callback = &hXvdCh->stInterruptCallbackInfo[eInterrupt];
   uint32_t uiDecodeErrorCount = pstDecodeErrorInfo->uiDecodeErrorCount;

   /* Forward PTS info to the FirstPTSReady app callback */
   if ( NULL != callback->BXVD_P_pAppIntCallbackPtr )
   {
      callback->BXVD_P_pAppIntCallbackPtr(
               callback->pParm1,
               callback->parm2,
               &uiDecodeErrorCount);
   }

   return;
}

static void BXVD_S_PictureParameters_XDMCompatibility_isr(
         void *pPrivateContext,
         int32_t iPrivateParam,
         const BXDM_PictureProvider_Callback_PictureParameterInfo *pstPictureParameters
         )
{
   BXVD_ChannelHandle hXvdCh = (BXVD_ChannelHandle) pPrivateContext;
   BXVD_Interrupt eInterrupt = (BXVD_Interrupt) iPrivateParam;
   struct BXVD_P_InterruptCallbackInfo *callback = &hXvdCh->stInterruptCallbackInfo[eInterrupt];

   /* bProgressiveStream_7411 is sticky.  It is updated only when a
    * source change has been detected. A source change is a change in
    * any of the following parameters
    *
    *  - Frame Rate
    *  - Resolution
    *  - Aspect Ratio
    *  - Color Primaries
    */
   if ((hXvdCh->stPictureParameterInfo.eFrameRateCode != pstPictureParameters->pstMFDPicture->eFrameRateCode)
       || (hXvdCh->stPictureParameterInfo.ulSourceHorizontalSize != pstPictureParameters->pstMFDPicture->ulSourceHorizontalSize)
       || (hXvdCh->stPictureParameterInfo.ulSourceVerticalSize != pstPictureParameters->pstMFDPicture->ulSourceVerticalSize)
       || (hXvdCh->stPictureParameterInfo.eAspectRatio != pstPictureParameters->pstMFDPicture->eAspectRatio)
       || (hXvdCh->stPictureParameterInfo.eColorPrimaries != pstPictureParameters->pstMFDPicture->eColorPrimaries)
      )
   {
      hXvdCh->bProgressiveStream_7411 = ( BXDM_Picture_Sequence_eProgressive == pstPictureParameters->pstUnifiedPicture->stPictureType.eSequence );
   }

   BKNI_Memset( &hXvdCh->stPictureParameterInfo, 0, sizeof( BXVD_PictureParameterInfo ) );

   BDBG_ASSERT(pstPictureParameters);
   BDBG_ASSERT(pstPictureParameters->pstUnifiedPicture);
   BDBG_ASSERT(pstPictureParameters->pstMFDPicture);

   /*
    * Not all elements in "*pstPPInfo" are set.
    *
    * Two of associated PR's from V1:  PR28082 and PR31593.
    *
    * Note: in V1, the "PictureParameterInfo" structure was updated in
    * "BXVD_P_PreparePictureParametersDataStruct_isr()" and
    * "BXVD_P_PrepareDataStructForVDC_isr()"
    *
    */
   hXvdCh->stPictureParameterInfo.eAspectRatio = pstPictureParameters->pstMFDPicture->eAspectRatio;
   hXvdCh->stPictureParameterInfo.eColorPrimaries = pstPictureParameters->pstMFDPicture->eColorPrimaries;
   hXvdCh->stPictureParameterInfo.eFrameRateCode = pstPictureParameters->pstMFDPicture->eFrameRateCode;
   hXvdCh->stPictureParameterInfo.eMatrixCoefficients = pstPictureParameters->pstMFDPicture->eMatrixCoefficients;
   hXvdCh->stPictureParameterInfo.eTransferCharacteristics = pstPictureParameters->pstMFDPicture->eTransferCharacteristics;
   hXvdCh->stPictureParameterInfo.uiSampleAspectRatioX = pstPictureParameters->pstMFDPicture->uiSampleAspectRatioX;
   hXvdCh->stPictureParameterInfo.uiSampleAspectRatioY = pstPictureParameters->pstMFDPicture->uiSampleAspectRatioY;
   hXvdCh->stPictureParameterInfo.ulSourceHorizontalSize = pstPictureParameters->pstMFDPicture->ulSourceHorizontalSize;
   hXvdCh->stPictureParameterInfo.ulSourceVerticalSize = pstPictureParameters->pstMFDPicture->ulSourceVerticalSize;
   hXvdCh->stPictureParameterInfo.ulDisplayHorizontalSize = pstPictureParameters->pstMFDPicture->ulDisplayHorizontalSize;
   hXvdCh->stPictureParameterInfo.ulDisplayVerticalSize = pstPictureParameters->pstMFDPicture->ulDisplayVerticalSize;
   hXvdCh->stPictureParameterInfo.i32_HorizontalPanScan = pstPictureParameters->pstMFDPicture->i32_HorizontalPanScan;
   hXvdCh->stPictureParameterInfo.i32_VerticalPanScan = pstPictureParameters->pstMFDPicture->i32_VerticalPanScan;

   hXvdCh->stPictureParameterInfo.bFrameProgressive = pstPictureParameters->pstMFDPicture->bFrameProgressive;

   hXvdCh->stPictureParameterInfo.bStreamProgressive_7411 = hXvdCh->bProgressiveStream_7411;
   hXvdCh->stPictureParameterInfo.bStreamProgressive = ( BXDM_Picture_Sequence_eProgressive == pstPictureParameters->pstUnifiedPicture->stPictureType.eSequence );

   hXvdCh->stPictureParameterInfo.uiProfile = pstPictureParameters->pstUnifiedPicture->stProtocol.eProfile;
   hXvdCh->stPictureParameterInfo.uiLevel = pstPictureParameters->pstUnifiedPicture->stProtocol.eLevel;

   /* SW7550-177: copy the Afd info into the picture parameter struct. */
   hXvdCh->stPictureParameterInfo.bValidAfd = pstPictureParameters->pstMFDPicture->bValidAfd;
   hXvdCh->stPictureParameterInfo.uiAfd = pstPictureParameters->pstMFDPicture->ulAfd;

   hXvdCh->stPictureParameterInfo.uiMacroBlockCntInter = pstPictureParameters->pstUnifiedPicture->stStats.stMacroBlockCount.uiInterCoded;
   hXvdCh->stPictureParameterInfo.uiMacroBlockCntIntra = pstPictureParameters->pstUnifiedPicture->stStats.stMacroBlockCount.uiIntraCoded;
   hXvdCh->stPictureParameterInfo.uiMacroBlockCntTotal = pstPictureParameters->pstUnifiedPicture->stStats.stMacroBlockCount.uiTotal;

   hXvdCh->stPictureParameterInfo.uiPictureCodingType = pstPictureParameters->pstUnifiedPicture->stPictureType.eCoding;

   hXvdCh->stPictureParameterInfo.uiBitRate = pstPictureParameters->pstUnifiedPicture->stStats.uiBitRate;
   hXvdCh->stPictureParameterInfo.uiLowDelayFlag = pstPictureParameters->pstUnifiedPicture->stPictureType.bLowDelay;
   hXvdCh->stPictureParameterInfo.uiVideoFormat = pstPictureParameters->pstUnifiedPicture->stDisplayInfo.eVideoFormat;

   hXvdCh->stPictureParameterInfo.uiFrameRateExtN = pstPictureParameters->pstUnifiedPicture->stFrameRate.stExtension.uiNumerator;
   hXvdCh->stPictureParameterInfo.uiFrameRateExtD = pstPictureParameters->pstUnifiedPicture->stFrameRate.stExtension.uiDenominator;

   /* SW3556-836: dereference the fixed_frame_rate_flag */
   hXvdCh->stPictureParameterInfo.uiFixedFrameRateFlag = ( BXDM_Picture_FrameRateType_eFixed == pstPictureParameters->pstUnifiedPicture->stFrameRate.eType );

   /* SW7405-4378: return the unaltered width and height for use by the application. */
   hXvdCh->stPictureParameterInfo.uiCodedSourceWidth = pstPictureParameters->pstUnifiedPicture->stBufferInfo.stSource.uiWidth;
   hXvdCh->stPictureParameterInfo.uiCodedSourceHeight = pstPictureParameters->pstUnifiedPicture->stBufferInfo.stSource.uiHeight;
   
   /* Forward Picture Parameter info to the PictureParameter app callback */
   if ( NULL != callback->BXVD_P_pAppIntCallbackPtr )
   {
      callback->BXVD_P_pAppIntCallbackPtr(
               callback->pParm1,
               callback->parm2,
               &hXvdCh->stPictureParameterInfo);
   }

   return;
}

/***************************************************************************
BXVD_InstallInterruptCallback: Used to enable and install an application
                               callback for a channel relevant Interrupt.
****************************************************************************/
BERR_Code BXVD_InstallInterruptCallback(BXVD_ChannelHandle hXvdCh,
                                        BXVD_Interrupt     eInterrupt,
                                        BXVD_CallbackFunc  fCallBack,
                                        void               *pParm1,
                                        int                parm2)
{
   struct BXVD_P_InterruptCallbackInfo *callback;

   BDBG_ENTER(BXVD_InstallInterruptCallback);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   if (eInterrupt >= BXVD_Interrupt_eMaxInterrupts)
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   if ( eInterrupt == BXVD_Interrupt_ePictureDataReady)
   {
      callback = &hXvdCh->pXvd->stDeviceInterruptCallbackInfo[BXVD_DeviceInterrupt_ePictureDataReady0];
   }
   else
   {
      callback = &hXvdCh->stInterruptCallbackInfo[eInterrupt];
   }

   switch ( eInterrupt )
   {
      case BXVD_Interrupt_ePictureDataReady:
         BXDM_DisplayInterruptHandler_InstallCallback_PictureDataReadyInterrupt(
                  hXvdCh->pXvd->hXdmDih[BXVD_DisplayInterrupt_eZero],
                  (BXDM_DisplayInterruptHandler_PictureDataReady_isr) fCallBack,
                  pParm1,
                  parm2);
         break;
      default:
         break;
   }

   BKNI_EnterCriticalSection();
   switch ( eInterrupt )
   {
      case BXVD_Interrupt_ePtsStcOffset:
         BXDM_PictureProvider_Callback_Install_StcPtsOffset_isr(
                  hXvdCh->hPictureProvider,
                  (BXDM_PictureProvider_Callback_StcPtsOffset_isr) fCallBack,
                  pParm1,
                  parm2);
         break;
      case BXVD_Interrupt_eFirstPTSReady:
         BXDM_PictureProvider_Callback_Install_FirstPTSReady_isr(
                  hXvdCh->hPictureProvider,
                  BXVD_S_FirstPtsReady_XDMCompatibility_isr,
                  hXvdCh,
                  eInterrupt);

         BXDM_PictureProvider_Callback_Install_FirstCodedPTSReady_isr(
                  hXvdCh->hPictureProvider,
                  (BXDM_PictureProvider_Callback_FirstCodedPTSReady_isr) fCallBack,
                  pParm1,
                  parm2);

         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eFirstCodedPTSReady,
                  false
                  );
         break;
      case BXVD_Interrupt_eFirstPTSPassed:
         BXDM_PictureProvider_Callback_Install_FirstPTSPassed_isr(
                  hXvdCh->hPictureProvider,
                  (BXDM_PictureProvider_Callback_FirstPTSPassed_isr) fCallBack,
                  pParm1,
                  parm2);
         break;
      case BXVD_Interrupt_ePTSError:
         BXDM_PictureProvider_Callback_Install_PTSError_isr(
                  hXvdCh->hPictureProvider,
                  (BXDM_PictureProvider_Callback_PTSError_isr) fCallBack,
                  pParm1,
                  parm2);
         break;
      case BXVD_Interrupt_eIFrame:
         BXDM_PictureProvider_Callback_Install_IFrame_isr(
                  hXvdCh->hPictureProvider,
                  (BXDM_PictureProvider_Callback_IFrame_isr) fCallBack,
                  pParm1,
                  parm2);
         break;
      case BXVD_Interrupt_ePictureParameters:
         BXDM_PictureProvider_Callback_Install_PictureParameters_isr(
                  hXvdCh->hPictureProvider,
                  BXVD_S_PictureParameters_XDMCompatibility_isr,
                  hXvdCh,
                  eInterrupt);
         break;
      case BXVD_Interrupt_eTSMPassInASTMMode:
         BXDM_PictureProvider_Callback_Install_TSMPassInASTMMode_isr(
                  hXvdCh->hPictureProvider,
                  (BXDM_PictureProvider_Callback_TSMPassInASTMMode_isr) fCallBack,
                  pParm1,
                  parm2);
         break;
      case BXVD_Interrupt_eClipStart:
         BXDM_PictureProvider_Callback_Install_ClipStart_isr(
                  hXvdCh->hPictureProvider,
                  BXVD_S_ClipEvent_XDMCompatibility_isr,
                  hXvdCh,
                  eInterrupt);
         break;
      case BXVD_Interrupt_eClipStop:
         BXDM_PictureProvider_Callback_Install_ClipStop_isr(
                  hXvdCh->hPictureProvider,
                  BXVD_S_ClipEvent_XDMCompatibility_isr,
                  hXvdCh,
                  eInterrupt);
         break;
      case BXVD_Interrupt_ePictureMarker:
         BXDM_PictureProvider_Callback_Install_PictureMarker_isr(
                  hXvdCh->hPictureProvider,
                  BXVD_S_ClipEvent_XDMCompatibility_isr,
                  hXvdCh,
                  eInterrupt);
         break;
      case BXVD_Interrupt_eRequestSTC:
         BXDM_PictureProvider_Callback_Install_RequestSTC_isr(
                  hXvdCh->hPictureProvider,
                  BXVD_S_RequestSTC_XDMCompatibility_isr,
                  hXvdCh,
                  eInterrupt);
         break;
      case BXVD_Interrupt_ePPBParameters:
         BXDM_PictureProvider_Callback_Install_PictureUnderEvaluation_isr(
                  hXvdCh->hPictureProvider,
                  (BXDM_PictureProvider_Callback_PictureUnderEvaluation_isr) fCallBack,
                  pParm1,
                  parm2);
         break;
      case BXVD_Interrupt_eTSMResult:
         BXDM_PictureProvider_Callback_Install_TSMResult_isr(
                  hXvdCh->hPictureProvider,
                  (BXDM_PictureProvider_Callback_TSMResult_isr) fCallBack,
                  pParm1,
                  parm2);
         break;
      case BXVD_Interrupt_ePictureExtensionData:
         BXDM_PictureProvider_Callback_Install_PictureExtensionData_isr(
                  hXvdCh->hPictureProvider,
                  (BXDM_PictureProvider_Callback_PictureExtensionData_isr) fCallBack,
                  pParm1,
                  parm2);
         break;

      case BXVD_Interrupt_eDecodeError:
         BXDM_PictureProvider_Callback_Install_DecodeError_isr(
                  hXvdCh->hPictureProvider,
                  BXVD_S_DecodeError_XDMCompatibility_isr,
                  hXvdCh,
                  eInterrupt);
         break;

      case BXVD_Interrupt_eChunkDone:
         BXDM_PictureProvider_Callback_Install_ChunkDone_isr(
                  hXvdCh->hPictureProvider,
                  (BXDM_PictureProvider_Callback_ChunkDone_isr) fCallBack,
                  pParm1,
                  parm2);
         break;

      default:
         break;
   }
   callback->BXVD_P_pAppIntCallbackPtr = (BXVD_IntCallbackFunc)fCallBack;
   callback->pParm1 = pParm1;
   callback->parm2 = parm2;

   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_InstallInterruptCallback);

   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
BXVD_UnInstallInterruptCallback: Used to disable and un-install an application
                                 callback for a channel relevant interrupt.
****************************************************************************/
BERR_Code BXVD_UnInstallInterruptCallback(BXVD_ChannelHandle hXvdCh,
                                          BXVD_Interrupt     eInterrupt)
{
   struct BXVD_P_InterruptCallbackInfo *callback;

   BDBG_ENTER(BXVD_UnInstallInterruptCallback);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   if (eInterrupt >= BXVD_Interrupt_eMaxInterrupts)
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }


   if ( eInterrupt == BXVD_Interrupt_ePictureDataReady)
   {
      callback = &hXvdCh->pXvd->stDeviceInterruptCallbackInfo[BXVD_DeviceInterrupt_ePictureDataReady0];
   }
   else
   {
      callback = &hXvdCh->stInterruptCallbackInfo[eInterrupt];
   }

   BKNI_EnterCriticalSection();
   switch ( eInterrupt )
   {
      case BXVD_Interrupt_ePtsStcOffset:
         BXDM_PictureProvider_Callback_UnInstall_StcPtsOffset_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_ePictureDataReady:
         BXDM_DisplayInterruptHandler_UnInstallCallback_PictureDataReadyInterrupt(
                  hXvdCh->pXvd->hXdmDih[BXVD_DisplayInterrupt_eZero]);
         break;
      case BXVD_Interrupt_eFirstPTSReady:
         BXDM_PictureProvider_Callback_UnInstall_FirstPTSReady_isr(
                  hXvdCh->hPictureProvider);
         BXDM_PictureProvider_Callback_UnInstall_FirstCodedPTSReady_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_eFirstPTSPassed:
         BXDM_PictureProvider_Callback_UnInstall_FirstPTSPassed_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_ePTSError:
         BXDM_PictureProvider_Callback_UnInstall_PTSError_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_eIFrame:
         BXDM_PictureProvider_Callback_UnInstall_IFrame_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_ePictureParameters:
         BXDM_PictureProvider_Callback_UnInstall_PictureParameters_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_eTSMPassInASTMMode:
         BXDM_PictureProvider_Callback_UnInstall_TSMPassInASTMMode_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_eClipStart:
         BXDM_PictureProvider_Callback_UnInstall_ClipStart_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_eClipStop:
         BXDM_PictureProvider_Callback_UnInstall_ClipStop_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_ePictureMarker:
         BXDM_PictureProvider_Callback_UnInstall_PictureMarker_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_eRequestSTC:
         BXDM_PictureProvider_Callback_UnInstall_RequestSTC_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_ePPBParameters:
         BXDM_PictureProvider_Callback_UnInstall_PictureUnderEvaluation_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_eTSMResult:
         BXDM_PictureProvider_Callback_UnInstall_TSMResult_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_ePictureExtensionData:
         BXDM_PictureProvider_Callback_UnInstall_PictureExtensionData_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_eDecodeError:
         BXDM_PictureProvider_Callback_UnInstall_DecodeError_isr(
                  hXvdCh->hPictureProvider);
         break;
      case BXVD_Interrupt_eChunkDone:
         BXDM_PictureProvider_Callback_UnInstall_ChunkDone_isr(
                  hXvdCh->hPictureProvider);
         break;
      default:
         break;
   }

   callback->BXVD_P_pAppIntCallbackPtr = NULL;
   callback->pParm1 = NULL;
   callback->parm2 = (int) NULL;
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_UnInstallInterruptCallback);
   return BERR_TRACE(BERR_SUCCESS);
}


/***************************************************************************
 * BXVD_GetBufferConfig: returns the buffer requirements for CDB and ITB.
 ****************************************************************************/
BERR_Code BXVD_GetBufferConfig(BXVD_Handle       hXvd,
                               BAVC_CdbItbConfig *pCdbItbConfigInfo,
                               int32_t           *pPicBufLength)
{
   BERR_Code ret = BERR_SUCCESS;
   BXVD_DecodeMode eDecodeMode;

   BDBG_ENTER (BXVD_GetBufferConfig);

   /* Check input parameters */
   BDBG_ASSERT(hXvd);
   BDBG_ASSERT(pCdbItbConfigInfo);
   BDBG_ASSERT(pPicBufLength);

   if ((hXvd == NULL)||(pCdbItbConfigInfo == NULL)||(pPicBufLength == NULL))
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   BXVD_P_ParseDecodeMode( hXvd, hXvd->stSettings.pDecodeModes, &eDecodeMode);

   /* Assign the CDB & ITB info corresponding to the buffer config mode */
   *pCdbItbConfigInfo = sCdbItbCfg[eDecodeMode];

   /* Assign FW Picture buffer size */
   if (eDecodeMode == BXVD_DecodeMode_eCustom)
   {
      *pPicBufLength = 0;
   }
   else
   {
      *pPicBufLength = (hXvd->stFWMemCfg[eDecodeMode].general_memory_size + hXvd->stFWMemCfg[eDecodeMode].cabac_bin_size +
                        (hXvd->stFWMemCfg[eDecodeMode].video_block_size * hXvd->stFWMemCfg[eDecodeMode].video_block_count));
   }

   BDBG_LEAVE (BXVD_GetBufferConfig);
   return BERR_TRACE(ret);
}

/***************************************************************************
 * BXVD_GetChannelMemoryConfig: returns the FW Memory configuation needed 
 * for the specified channel settings. 
 ****************************************************************************/
BERR_Code BXVD_GetChannelMemoryConfig( BXVD_Handle                hXvd,           /* [in] XVD handle */
                                       const BXVD_ChannelSettings *pChSettings,   /* [in] channel settings */
                                       BXVD_FWMemConfig           *pFWMemConfig)  /* [out] memory configuration for this channel */
{
   BXVD_P_DecodeFWMemSize  stDecodeFWMemSize;

   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER (BXVD_GetChannelMemoryConfig);

   /* Check input parameters */
   BDBG_ASSERT(hXvd);
   BDBG_ASSERT(pChSettings);
   BDBG_ASSERT(pFWMemConfig);

   if (pChSettings->eChannelMode == BXVD_ChannelMode_eStill)
   {
      /* Get still picture decode FW memory sizes */
      rc = BXVD_P_GetStillDecodeFWMemSize(hXvd,
                                          pChSettings->eDecodeResolution,
                                          pChSettings->peVideoCmprStdList,
                                          pChSettings->uiVideoCmprCount,
                                          &stDecodeFWMemSize);
      if (rc != BERR_SUCCESS)
      {
         pFWMemConfig->uiGeneralHeapSize = 0;
         pFWMemConfig->uiCabacHeapSize = 0;
         pFWMemConfig->uiPictureHeapSize = 0;

         return BERR_TRACE(rc);
      }

      pFWMemConfig->uiGeneralHeapSize =  stDecodeFWMemSize.uiFWContextSize + 
         stDecodeFWMemSize.uiFWCabacWorklistSize + stDecodeFWMemSize.uiFWInnerLoopWorklistSize;

      pFWMemConfig->uiCabacHeapSize = stDecodeFWMemSize.uiFWCabacSize;

      pFWMemConfig->uiPictureHeapSize = stDecodeFWMemSize.uiFWPicBlockSize * stDecodeFWMemSize.uiFWPicBlockCount;

   }
   else
   {
      /* Get video picture decode FW memory sizes */
      rc = BXVD_P_GetDecodeFWMemSize(hXvd,
                                     pChSettings->eDecodeResolution,
                                     pChSettings->peVideoCmprStdList,
                                     pChSettings->uiVideoCmprCount,
                                     pChSettings,
                                     &stDecodeFWMemSize);

      if (rc != BERR_SUCCESS)
      {
         pFWMemConfig->uiGeneralHeapSize = 0;
         pFWMemConfig->uiCabacHeapSize = 0;
         pFWMemConfig->uiPictureHeapSize = 0;

         return BERR_TRACE(rc);
      }

      pFWMemConfig->uiGeneralHeapSize =  stDecodeFWMemSize.uiFWContextSize + stDecodeFWMemSize.uiFWCabacWorklistSize + 
         stDecodeFWMemSize.uiFWInnerLoopWorklistSize + stDecodeFWMemSize.uiFWDirectModeSize + stDecodeFWMemSize.uiFWInterLayerMVSize;
      pFWMemConfig->uiCabacHeapSize = stDecodeFWMemSize.uiFWCabacSize;

      pFWMemConfig->uiPictureHeapSize = stDecodeFWMemSize.uiFWInterLayerPicSize +
         (stDecodeFWMemSize.uiFWPicBlockSize * stDecodeFWMemSize.uiFWPicBlockCount);
   }

   BDBG_LEAVE (BXVD_GetChannelMemoryConfig);

   return rc;
}


/***************************************************************************
  BXVD_DecodeStillPicture: Decodes a still picture specified by RaveContext
  and type.
****************************************************************************/
BERR_Code BXVD_DecodeStillPicture(BXVD_ChannelHandle   hXvdCh,
                                  BXVD_DecodeStillMode ePictureType,
                                  BAVC_XptContextMap   *pContextMap)
{
   BXVD_Handle hXvd = NULL;
   BXVD_DecodeSettings stDecodeSettings;
   BERR_Code rc = BERR_SUCCESS;

   uint32_t uiCDBReadAddress = 0;
   uint32_t uiCDBValidAddress = 0;
   uint32_t uiCDBSize = 0;

   uint32_t i;

   BXVD_DecodeResolution eDecodeResolution;
   BXVD_P_PictureReleaseQueue *pPictureReleaseQueue = NULL;

   BDBG_ENTER(BXVD_DecodeStillPicture);
   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pContextMap);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   hXvd = hXvdCh->pXvd;

   /* Validate hXvdCh is a still picture channel */
   if (hXvdCh->sChSettings.eChannelMode != BXVD_ChannelMode_eStill)
   {
      /* Only allow still picture compatibility mode if the passed in
       * channel is #0 */
      if ((hXvdCh->ulChannelNum == 0) && (hXvd->bStillPictureCompatibilityMode))
      {
         /* We only support MPEG_SD and MPEG_HD still modes in compatibility mode */
         if (ePictureType == BXVD_DecodeModeStill_eMPEG_SD)
         {
            eDecodeResolution = BXVD_DecodeResolution_eSD;
         }
         else if (ePictureType == BXVD_DecodeModeStill_eMPEG_HD)
         {
            eDecodeResolution = BXVD_DecodeResolution_eHD;
         }
         else
         {
            BXVD_DBG_ERR(hXvdCh, ("Still Picture Compatibility Mode: ePictureType parameter out of range"));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
         }

         if (hXvd->bStillChannelAllocated)
         {
            /* A still channel was already created, so use that instead */
            hXvdCh = hXvd->ahChannel[hXvd->uiStillChannelNum];
            BDBG_ASSERT(hXvdCh);
            BXVD_DBG_WRN(hXvdCh, ("Still Picture Compatibility Mode: Using pre-allocated still picture channel [%d]",
                                       hXvdCh->ulChannelNum));

            if (hXvdCh->bDecoderChannelOpened)
            {
               /* Close the channel if the channel has already been opened in
                * still picture compatibility mode */
               rc = BERR_TRACE(BXVD_P_HostCmdSendDecChannelClose(hXvd, hXvdCh));
               if (rc != BERR_SUCCESS)
               {
                  return BERR_TRACE(rc);
               }
            }

            if (!hXvdCh->bDecoderChannelOpened)
            {
               /* Channel is being opened or reopened, don't release previous still picture PPB */
               hXvdCh->bStillPictureToRelease = false;

               uiCDBReadAddress = BREG_Read32(hXvd->hReg,
                                              pContextMap->CDB_Read);

               uiCDBValidAddress = BREG_Read32(hXvd->hReg,
                                               pContextMap->CDB_Valid);

               /* Make sure the CDB Read Address is 256 byte aligned because
                * cabac bin must be 256 byte aligned */
               BDBG_ASSERT((uiCDBReadAddress & 0xff)   == 0);

               /* Make sure valid address is larger than read address */
               BDBG_ASSERT(uiCDBValidAddress > uiCDBReadAddress);

               uiCDBSize = uiCDBValidAddress - uiCDBReadAddress;

               /* Hack to account for AVD FW expecting 128-byte
                * aligned cabac size */

               /* Round CDB size up to 128 byte boundary */
               uiCDBSize += 127;
               uiCDBSize &= ~7;
               uiCDBSize += 256; /* This is hack to fix a cabac hw bug on some platforms */

               BXVD_DBG_MSG(hXvdCh, ("Still Picture Compatibility Mode: cabac base/size = %x/%d",
                                          uiCDBReadAddress,
                                          uiCDBSize));

               hXvdCh->stDecodeFWBaseAddrs.uiFWCabacBase = uiCDBReadAddress;
               hXvdCh->stDecodeFWMemSize.uiFWCabacSize = uiCDBSize;

               rc = BXVD_P_HostCmdSendDecChannelOpen((BXVD_Handle)hXvd,
                                                     hXvdCh,
                                                     true,
                                                     eDecodeResolution,
                                                     &(hXvdCh->stDecodeFWMemSize),
                                                     &(hXvdCh->stDecodeFWBaseAddrs));

               if (rc != BERR_SUCCESS)
               {
                  return BERR_TRACE(rc);
               }
            }
         }
         else
         {
            BXVD_DBG_ERR(hXvdCh, ("Still Picture Compatibility Mode: Still channel has not been pre-allocated"));
            return BERR_TRACE(BERR_UNKNOWN);
         }
      }
      else
      {
         BXVD_DBG_ERR(hXvdCh, ("BXVD_DecodeStillPicture failed, channel[%d] is not a still decode channel",
                                    hXvdCh->ulChannelNum));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }

#if BXVD_P_FW_HIM_API
   /* Return previous still picture in release queue (if any) */
   if (hXvdCh->bStillPictureToRelease)
   {
      uint32_t  uiReleaseWriteOffset;
      uint32_t  uiQueueSize = sizeof(BXVD_P_PictureReleaseQueue);

      BKNI_EnterCriticalSection(); 

      BXVD_P_RELEASE_QUEUE_GET_ADDR( hXvdCh, pPictureReleaseQueue );

      BXVD_P_RELEASE_QUEUE_GET_SHADOW_WRITE_OFFSET( hXvdCh, uiReleaseWriteOffset );

      pPictureReleaseQueue->display_elements[uiReleaseWriteOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE] = (BXVD_P_PPB *) hXvdCh->uiStillDisplayElementOffset;

      BERR_TRACE(BMEM_Heap_FlushCache(hXvdCh->hSystemHeap, pPictureReleaseQueue, uiQueueSize));

      BXVD_P_INCREMENT_2BASED_OFFSET( uiReleaseWriteOffset, 1 );

      BXVD_P_RELEASE_QUEUE_SET_SHADOW_WRITE_OFFSET( hXvdCh, uiReleaseWriteOffset );

      BKNI_LeaveCriticalSection(); 

      hXvdCh->bStillPictureToRelease = false;
      hXvdCh->uiStillDisplayElementOffset = 0;
   }
#endif

   /* Setup decode settings struct */
   BKNI_Memset(&stDecodeSettings,
               0,
               sizeof(BXVD_DecodeSettings));

   stDecodeSettings.pContextMap = pContextMap;
   switch (ePictureType)
   {
      case BXVD_DecodeModeStill_eMPEG_SD:
      case BXVD_DecodeModeStill_eMPEG_HD:
         stDecodeSettings.eVideoCmprStd = BAVC_VideoCompressionStd_eMPEG2;
         break;

      case BXVD_DecodeModeStill_eAVC_SD:
      case BXVD_DecodeModeStill_eAVC_HD:
         stDecodeSettings.eVideoCmprStd = BAVC_VideoCompressionStd_eH264;
         break;
      case BXVD_DecodeModeStill_eVC1_SD:
      case BXVD_DecodeModeStill_eVC1_HD:
         stDecodeSettings.eVideoCmprStd = BAVC_VideoCompressionStd_eVC1;
         break;

      case BXVD_DecodeModeStill_eMPEG4Part2_SD:
      case BXVD_DecodeModeStill_eMPEG4Part2_HD:
         stDecodeSettings.eVideoCmprStd = BAVC_VideoCompressionStd_eMPEG4Part2;
         break;

      case BXVD_DecodeModeStill_eVP8_SD:
      case BXVD_DecodeModeStill_eVP8_HD:
         stDecodeSettings.eVideoCmprStd = BAVC_VideoCompressionStd_eVP8;
         break;

      default:
         BXVD_DBG_ERR(hXvdCh, ("Invalid decode still mode specified: 0x%x (%d)",
                                    ePictureType, ePictureType));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Validate picture type */
   /* Make sure protocol is within channel's capabilities */
   for (i = 0; i < hXvdCh->sChSettings.uiVideoCmprCount; i++)
   {
      if (stDecodeSettings.eVideoCmprStd == hXvdCh->sChSettings.peVideoCmprStdList[i]) break;
   }
   if (i == hXvdCh->sChSettings.uiVideoCmprCount)
   {
      BXVD_DBG_ERR(hXvdCh, ("Still channel not capable of specified decode still mode (protocol): 0x%x (%d)",
                                 ePictureType, ePictureType));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Make sure resolution is within channel's capabilities */
   switch(hXvdCh->sChSettings.eDecodeResolution)
   {
      case BXVD_DecodeResolution_eHD:
         switch(ePictureType)
         {
            case BXVD_DecodeModeStill_eMPEG_SD:
            case BXVD_DecodeModeStill_eMPEG_HD:
            case BXVD_DecodeModeStill_eAVC_SD:
            case BXVD_DecodeModeStill_eAVC_HD:
            case BXVD_DecodeModeStill_eVC1_SD:
            case BXVD_DecodeModeStill_eVC1_HD:
            case BXVD_DecodeModeStill_eMPEG4Part2_SD:
            case BXVD_DecodeModeStill_eMPEG4Part2_HD:
            case BXVD_DecodeModeStill_eVP8_SD:
            case BXVD_DecodeModeStill_eVP8_HD:
               break;

            default:
               BXVD_DBG_ERR(hXvdCh, ("Still channel not capable of specified decode still mode (max resolution: HD): 0x%x (%d)",
                                          ePictureType, ePictureType));
               return BERR_TRACE(BERR_INVALID_PARAMETER);
         }
         break;

      case BXVD_DecodeResolution_eSD:
         switch(ePictureType)
         {
            case BXVD_DecodeModeStill_eMPEG_SD:
            case BXVD_DecodeModeStill_eAVC_SD:
            case BXVD_DecodeModeStill_eVC1_SD:
            case BXVD_DecodeModeStill_eMPEG4Part2_SD:
            case BXVD_DecodeModeStill_eVP8_SD:
               break;

            default:
               BXVD_DBG_ERR(hXvdCh, ("Still channel not capable of specified decode still mode (max resolution: SD): 0x%x (%d)",
                                          ePictureType, ePictureType));
               return BERR_TRACE(BERR_INVALID_PARAMETER);
         }
         break;

      default:
         BXVD_DBG_ERR(hXvdCh, ("Still channel not capable of specified decode still mode (resolution): 0x%x (%d)",
                                    ePictureType, ePictureType));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* Disable interrupt to prevent race condition */
   if (hXvd->stDecoderContext.pCbAVC_StillPicRdy_ISR)
   {
      rc = BINT_DisableCallback(hXvd->stDecoderContext.pCbAVC_StillPicRdy_ISR);

      if (rc != BERR_SUCCESS)
      {
         return BERR_TRACE(rc);
      }
   }

   /* Call StartDecode with still context */
   rc = BERR_TRACE(BXVD_StartDecode(
                      hXvdCh,
                      &stDecodeSettings));

   if (rc != BERR_SUCCESS )
   {
      return BERR_TRACE(rc);
   }

#if !BXVD_P_FW_HIM_API
   /* Return previous still picture in release queue (if any) */
   if (hXvdCh->bStillPictureToRelease)
   {
      uint32_t       uiReleaseWriteOffset;

      BXVD_P_RELEASE_QUEUE_GET_ADDR( hXvdCh, pPictureReleaseQueue );
      BXVD_P_RELEASE_QUEUE_GET_WRITE_OFFSET( hXvdCh, uiReleaseWriteOffset );

      pPictureReleaseQueue->display_elements[uiReleaseWriteOffset - BXVD_P_INITIAL_OFFSET_DISPLAY_QUEUE] = (BXVD_P_PPB *) hXvdCh->uiStillDisplayElementOffset;

      BXVD_P_INCREMENT_2BASED_OFFSET( uiReleaseWriteOffset, 1 );

      BXVD_P_RELEASE_QUEUE_SET_WRITE_OFFSET( hXvdCh, uiReleaseWriteOffset );

      hXvdCh->bStillPictureToRelease = false;
      hXvdCh->uiStillDisplayElementOffset = 0;
   }
#endif

   /* Re-enable interrupt */
   if (hXvd->stDecoderContext.pCbAVC_StillPicRdy_ISR)
   {
      rc = BINT_EnableCallback(hXvd->stDecoderContext.pCbAVC_StillPicRdy_ISR);
   }

   /* The decoded picture will be retrieved by to the
    * BXVD_DeviceInterrupt_eDecodedStillBufferReady interrupt
    * handler */

   BDBG_LEAVE(BXVD_DecodeStillPicture);
   return BERR_TRACE(rc);
}

/***************************************************************************
Summary: This routine is used to reset the still picture decoder state.
****************************************************************************/
BERR_Code BXVD_DecodeStillPictureReset(BXVD_ChannelHandle  hXvdCh)
{
   BXVD_Handle hXvd = NULL;
   BERR_Code rc = BERR_SUCCESS;


   BDBG_ENTER(BXVD_DecodeStillPictureReset);
   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   hXvd = hXvdCh->pXvd;

   /* Validate hXvdCh is a still picture channel */
   if (hXvdCh->sChSettings.eChannelMode == BXVD_ChannelMode_eStill)
   {
      rc = BXVD_StopDecode(hXvdCh);

   }

   /* Handle still picture compatibility mode */
   else if ((hXvdCh->ulChannelNum == 0) && hXvd->bStillPictureCompatibilityMode && hXvd->bStillChannelAllocated)
   {
      /* A compatibility still channel is being used */
      hXvdCh = hXvd->ahChannel[hXvd->uiStillChannelNum];
      BDBG_ASSERT(hXvdCh);

      rc =  BXVD_StopDecode(hXvdCh);
   }

   else
   {
      BXVD_DBG_ERR(hXvdCh, ("Channel specified not a still docode channel: %d", hXvdCh->ulChannelNum));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }
   return rc;
}

/***************************************************************************
  BXVD_SetSkipPictureModeDecode: Sets the skip picture mode (I, IP or IPB)
****************************************************************************/
BERR_Code BXVD_SetSkipPictureModeDecode(BXVD_ChannelHandle hXvdCh,
                                        BXVD_SkipMode      eSkipMode)
{
   BAVC_XptContextMap  XptContextMap;
   BAVC_XptContextMap  aXptContextMap_Extended[BXVD_NUM_EXT_RAVE_CONTEXT];

   uint32_t i;

   BERR_Code eStatus = BERR_SUCCESS;
   BXDM_PictureProvider_PreserveStateSettings stPreserveStateSettings;

   BDBG_ENTER(BXVD_SetSkipPictureModeDecode);
   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(eSkipMode <= BXVD_SkipMode_eDecode_Ref_Only);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetSkipPictureModeDecode(%d -> %d)", hXvdCh->eCurrentSkipMode, eSkipMode));

   if (hXvdCh->eCurrentSkipMode != eSkipMode)
   {
      hXvdCh->eCurrentSkipMode = eSkipMode;

      /*
       * If Host Sparse mode is currently enabled and Decoder is active,
       * the decoder must be stopped and started to change skip mode.
       */

      if ((hXvdCh->stDecoderContext.bHostSparseMode == true) && (hXvdCh->eDecoderState == BXVD_P_DecoderState_eActive))
      {
         BXVD_DBG_MSG(hXvdCh, ("BXVD_SetSkipPictureMode: Sparse mode enabled, Stopping and starting decoder"));

         /* Don't reset decoder settings when stopping decode for a sparse mode speed transition */
         hXvdCh->bPreserveState = true;

         /* Force channelChanleMode to hold last picture */
         BERR_TRACE(BXDM_PictureProvider_GetDefaultPreserveStateSettings(
                     hXvdCh->hPictureProvider,
                     &stPreserveStateSettings
                     ));

         BKNI_EnterCriticalSection();

         stPreserveStateSettings.bDisplay = true;
         stPreserveStateSettings.bCounters = true;

         BERR_TRACE(BXDM_PictureProvider_SetPreserveStateSettings_isr(
                  hXvdCh->hPictureProvider,
                  &stPreserveStateSettings
                  ));

         BKNI_LeaveCriticalSection();

         BXVD_StopDecode(hXvdCh);

         /* Decoder counters should not be cleared */
         hXvdCh->bPreserveCounters = true;

         /* Reset XPT Rave CDB read register address */
         XptContextMap.CDB_Read = hXvdCh->ulXptCDB_Read;
         hXvdCh->sDecodeSettings.pContextMap = &XptContextMap;

         for (i = 0; i < hXvdCh->sDecodeSettings.uiContextMapExtNum; i++)
         {
            hXvdCh->sDecodeSettings.aContextMapExtended[i] = &aXptContextMap_Extended[i];
            aXptContextMap_Extended[i].CDB_Read = hXvdCh->aulXptCDB_Read_Extended[i];
         }

         eStatus = BERR_TRACE(BXVD_StartDecode(hXvdCh, &hXvdCh->sDecodeSettings));

         hXvdCh->bPreserveState = false;
      }
      else
      {
         if (hXvdCh->eDecoderState == BXVD_P_DecoderState_eActive)
         {
            eStatus = BXVD_P_HostCmdSetSkipPictureMode(hXvdCh->pXvd,
                                                       hXvdCh->ulChannelNum,
                                                       eSkipMode);
         }
      }
   }

   BDBG_LEAVE(BXVD_SetSkipPictureModeDecode);
   return BERR_TRACE(eStatus);
}

/***************************************************************************
  BXVD_GetSkipPictureModeConfig: Gets the current skip picture mode
****************************************************************************/
BERR_Code BXVD_GetSkipPictureModeConfig(BXVD_ChannelHandle hXvdCh,
                                        BXVD_SkipMode      *peSkipMode)
{
   BERR_Code eStatus = BERR_SUCCESS;

   BDBG_ENTER(BXVD_GetSkipPictureModeConfig);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   *peSkipMode = hXvdCh->eCurrentSkipMode;

   BDBG_LEAVE(BXVD_GetSkipPictureModeConfig);
   return BERR_TRACE(eStatus);
}

/***************************************************************************
BXVD_EnableInterrupt_isr: Enable or disable the specifed interrupt. ISR version
***************************************************************************/
BERR_Code BXVD_EnableInterrupt_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Interrupt eInterrupt,
   BXVD_InterruptEnable eEnable
   )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER(BXVD_EnableInterrupt_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* Sanity check arguments */
   if (eEnable != BXVD_InterruptEnable_eDisable &&
       eEnable != BXVD_InterruptEnable_eEnable)
   {
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   switch (eInterrupt)
   {
      case BXVD_Interrupt_eVideoInstructionChecker:
         BXVD_DBG_ERR(hXvdCh, ("Enable/Disable interrupt not supported for VideoIntructionChecker"));
         rc = BERR_INVALID_PARAMETER;
         break;
      case BXVD_Interrupt_ePtsStcOffset:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eStcPtsOffset,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_ePictureDataReady:
         BXVD_DBG_ERR(hXvdCh, ("Enable/Disable interrupt not supported for PictureDataReady"));
         rc = BERR_INVALID_PARAMETER;
         break;
      case BXVD_Interrupt_eUserData:
         BXVD_DBG_ERR(hXvdCh, ("Enable/Disable interrupt not supported for UserData"));
         rc = BERR_INVALID_PARAMETER;
         break;
      case BXVD_Interrupt_ePicDepthLowerThreshold:
         BXVD_DBG_ERR(hXvdCh, ("Enable/Disable interrupt not supported for PicDepthLowerThreshold"));
         rc = BERR_INVALID_PARAMETER;
         break;
      case BXVD_Interrupt_ePicDepthHigherThreshold:
         BXVD_DBG_ERR(hXvdCh, ("Enable/Disable interrupt not supported for PicDepthHigherThreshold"));
         rc = BERR_INVALID_PARAMETER;
         break;
      case BXVD_Interrupt_eFirstPTSReady:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eFirstPTSReady,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_eFirstPTSPassed:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eFirstPTSPassed,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_ePTSError:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_ePTSError,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_ePauseUntoPTS:
         BXVD_DBG_WRN(hXvdCh, ("BXVD_Interrupt_ePauseUntoPTS is DEPRECATED"));
         break;
      case BXVD_Interrupt_eDisplayUntoPTS:
         BXVD_DBG_WRN(hXvdCh, ("BXVD_Interrupt_ePauseUntoPTS is DEPRECATED"));
         break;
      case BXVD_Interrupt_ePTS1Match:
         BXVD_DBG_WRN(hXvdCh, ("BXVD_Interrupt_ePTS1Match is DEPRECATED"));
         break;
      case BXVD_Interrupt_ePTS2Match:
         BXVD_DBG_WRN(hXvdCh, ("BXVD_Interrupt_ePTS2Match is DEPRECATED"));
         break;
      case BXVD_Interrupt_eIFrame:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eIFrame,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_ePictureParameters:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_ePictureParameters,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_eTSMPassInASTMMode:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eTSMPassInASTMMode,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_eClipStart:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eClipStart,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_eClipStop:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eClipStop,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_ePictureMarker:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_ePictureMarker,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_eMarker:
         hXvdCh->stCallbackReq.bMarker = eEnable;
         break;
      case BXVD_Interrupt_eRequestSTC:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eRequestSTC,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_ePPBReceived:
         hXvdCh->stCallbackReq.bPPBReceived = eEnable;
         break;
      case BXVD_Interrupt_ePPBParameters:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_ePictureUnderEvaluation,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_eDecodeError:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eDecodeError,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_eTSMResult:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eTSMResult,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_ePictureExtensionData:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_ePictureExtensionData,
                  eEnable
                  );
         break;
      case BXVD_Interrupt_eChunkDone:
         BXDM_PictureProvider_Callback_SetEnable_isr(
                  hXvdCh->hPictureProvider,
                  BXDM_PictureProvider_Callback_eChunkDone,
                  eEnable
                  );
         break;
      default:
         rc = BERR_INVALID_PARAMETER;
         break;
   }

   BDBG_LEAVE(BXVD_EnableInterrupt_isr);

   return BERR_TRACE(rc);
}

/***************************************************************************
BXVD_EnableInterrupt: Enable or disable the specifed interrupt
***************************************************************************/
BERR_Code BXVD_EnableInterrupt(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Interrupt eInterrupt,
   BXVD_InterruptEnable eEnable
   )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER(BXVD_EnableInterrupt);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   BKNI_EnterCriticalSection();
   rc = BXVD_EnableInterrupt_isr(hXvdCh, eInterrupt, eEnable);
   BKNI_LeaveCriticalSection();


   BDBG_LEAVE(BXVD_EnableInterrupt);
   return BERR_TRACE(rc);
}

/***************************************************************************
Summary:
        Link two decoders picture procssing

Description:
        Routine used to link the primary decoder picture processing
with the processing of the secondary decoders pictures. Two decoders
are needed to decode 12 CIF video streams which will be displayed on
a single display device.
***************************************************************************/
void BXVD_LinkDecoders(BXVD_Handle hXvd_Primary,
                       BXVD_Handle hXvd_Secondary,
                       BXVD_DisplayInterrupt eDisplayInterrupt)
{
   BDBG_ASSERT(hXvd_Primary);
   BDBG_ASSERT(hXvd_Secondary);

   /* Check handle type for correctness */
   if (hXvd_Primary->eHandleType != BXVD_P_HandleType_XvdMain ||
       hXvd_Secondary->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return;
   }

   hXvd_Primary->hXvd_Secondary = hXvd_Secondary;
   hXvd_Secondary->hLinkedDecoderXdmDih = hXvd_Primary->hXdmDih[eDisplayInterrupt];
   hXvd_Primary->Secondary_eDisplayInterrupt = eDisplayInterrupt;
}


/***************************************************************************
Summary:
        Unlink two decoders picture procssing

Description:
        Routine used to unlink the primary decoder picture processing
with the processing of the secondary decoders pictures.
***************************************************************************/
void BXVD_UnlinkDecoders(BXVD_Handle hXvd_Primary)
{
   BDBG_ASSERT(hXvd_Primary);

   /* Check handle type for correctness */
   if (hXvd_Primary->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return;
   }

   hXvd_Primary->hXvd_Secondary = (BXVD_Handle) NULL;
}


/************************/
/* Display Manager APIs */
/************************/

/*********************************/
/* Display Manager APIs - Status */
/*********************************/

/***************************************************************************
BXVD_GetPTS: API used to get the running PTS.
****************************************************************************/
BERR_Code BXVD_GetPTS
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PTSInfo       *pPTSInfo
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetPTS);

   BKNI_EnterCriticalSection();
   rc = BXVD_GetPTS_isr(
      hXvdCh,
      pPTSInfo
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetPTS);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetPTS_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PTSInfo       *pPTSInfo
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_PTSInfo stPTSInfo;
   BDBG_ENTER(BXVD_GetPTS_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pPTSInfo);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /*
    * Copy the "stCurrentPTSInfo" structure.
    */

   rc = BXDM_PictureProvider_GetCurrentPTSInfo_isr(
            hXvdCh->hPictureProvider,
            &stPTSInfo
            );

   *pPTSInfo = stPTSInfo;

   BDBG_LEAVE(BXVD_GetPTS_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
  BXVD_GetLastCodedPTS: API used to get the last coded PTS.
****************************************************************************/
BERR_Code BXVD_GetLastCodedPTS
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PTSInfo       *pPTSInfo
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetLastCodedPTS);

   BKNI_EnterCriticalSection();
   rc = BXVD_GetLastCodedPTS_isr(
      hXvdCh,
      pPTSInfo
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetLastCodedPTS);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetLastCodedPTS_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PTSInfo       *pPTSInfo
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_PTSInfo stPTSInfo;

   BDBG_ENTER(BXVD_GetLastCodedPTS_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pPTSInfo);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetLastCodedPTSInfo_isr(
            hXvdCh->hPictureProvider,
            &stPTSInfo
            );

   *pPTSInfo = stPTSInfo;

   BDBG_LEAVE(BXVD_GetLastCodedPTS_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
  BXVD_GetNextPTS: API used to get the next PTS.
****************************************************************************/
BERR_Code BXVD_GetNextPTS
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_PTSInfo *pPTSInfo /* [out] PTS Info is returned*/
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetNextPTS);

   BKNI_EnterCriticalSection();
   rc = BXVD_GetNextPTS_isr(
      hXvdCh,
      pPTSInfo
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetNextPTS);
   return rc;
}

BERR_Code BXVD_GetNextPTS_isr
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_PTSInfo *pPTSInfo /* [out] PTS Info is returned*/
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_PTSInfo stPTSInfo;

   BDBG_ENTER(BXVD_GetNextPTS_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pPTSInfo);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetNextPTSInfo_isr(
               hXvdCh->hPictureProvider,
               &stPTSInfo
               );

   *pPTSInfo = stPTSInfo;

   BDBG_LEAVE(BXVD_GetNextPTS_isr);

   return rc;
}

/***************************************************************************
  BXVD_GetIPictureFoundState: Retrieve I picture found status
****************************************************************************/
BERR_Code BXVD_GetIPictureFoundStatus
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbIPictureFound
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetIPictureFoundStatus);

   rc = BXVD_GetIPictureFoundStatus_isr(
      hXvdCh,
      pbIPictureFound
      );

   BDBG_LEAVE(BXVD_GetIPictureFoundStatus);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetIPictureFoundStatus_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbIPictureFound
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_GetIPictureFoundStatus_isr);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetIPictureFoundStatus_isr(
                     hXvdCh->hPictureProvider,
                     pbIPictureFound
                     );

   BDBG_LEAVE(BXVD_GetIPictureFoundStatus_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
  BXVD_GetPictureTag: Get the current picture tag from DM.
***************************************************************************/
BERR_Code BXVD_GetPictureTag
(
   BXVD_ChannelHandle    hXvdCh,
   unsigned long        *pulPictureTag
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetPictureTag);

   rc = BXVD_GetPictureTag_isr(
      hXvdCh,
      pulPictureTag
      );

   BDBG_LEAVE(BXVD_GetPictureTag);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetPictureTag_isr
(
   BXVD_ChannelHandle    hXvdCh,
   unsigned long        *pulPictureTag
   )
{
   BERR_Code rc;
   uint32_t uiPictureTag;

   BDBG_ENTER(BXVD_GetPictureTag_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pulPictureTag);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* Return current picture tag */
   rc = BXDM_PictureProvider_GetPictureTag_isr(
                     hXvdCh->hPictureProvider,
                     &uiPictureTag
                     );

   *pulPictureTag = uiPictureTag;

   BDBG_LEAVE(BXVD_GetPictureTag_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
  BXVD_GetGopTimeCode: Get the GOP timecode from DM and decode into HH:MM:SS:FF
***************************************************************************/
BERR_Code BXVD_GetGopTimeCode
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_GopTimeCode *pGopTimeCode
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetGopTimeCode);

   rc = BXVD_GetGopTimeCode_isr(
      hXvdCh,
      pGopTimeCode
      );

   BDBG_LEAVE(BXVD_GetGopTimeCode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetGopTimeCode_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_GopTimeCode *pGopTimeCode
   )
{
   BERR_Code rc;
   BXDM_Picture_GopTimeCode stTimeCode;
   BDBG_ENTER(BXVD_GetGopTimeCode_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pGopTimeCode);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetCurrentTimeCode_isr(
            hXvdCh->hPictureProvider,
            &stTimeCode
            );

   pGopTimeCode->ulTimeCodeHours = stTimeCode.uiHours;
   pGopTimeCode->ulTimeCodeMinutes = stTimeCode.uiMinutes;
   pGopTimeCode->ulTimeCodeSeconds = stTimeCode.uiSeconds;
   pGopTimeCode->ulTimeCodePictures = stTimeCode.uiPictures;
   pGopTimeCode->bTimeCodeValid = stTimeCode.bValid;

   BDBG_LEAVE(BXVD_GetGopTimeCode_isr);
   return BERR_TRACE(rc);
}

/**********************************/
/* Display Manager APIs - Display */
/**********************************/

/***************************************************************************
BXVD_EnableMute: The Application can override the decoder mute with this API.
***************************************************************************/
BERR_Code BXVD_EnableMute
(
   BXVD_ChannelHandle hXvdCh,
   bool               bEnable
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_EnableMute);

   BKNI_EnterCriticalSection();
   rc = BXVD_EnableMute_isr(
      hXvdCh,
      bEnable
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_EnableMute);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_EnableMute_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool               bEnable
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_EnableMute_isr);
   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* Save user request */
   rc = BXDM_PictureProvider_SetMuteMode_isr(
            hXvdCh->hPictureProvider,
            bEnable
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_EnableMute(%d)", bEnable));

   BDBG_LEAVE(BXVD_EnableMute_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
BXVD_SetDisplayFieldMode: API used to set the display field mode.
****************************************************************************/
BERR_Code BXVD_SetDisplayFieldMode
(
   BXVD_ChannelHandle    hXvdCh,
   BXVD_DisplayFieldType eDisplayFieldType
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetDisplayFieldMode);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetDisplayFieldMode_isr(
      hXvdCh,
      eDisplayFieldType
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetDisplayFieldMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetDisplayFieldMode_isr
(
   BXVD_ChannelHandle    hXvdCh,
   BXVD_DisplayFieldType eDisplayFieldType
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_DisplayFieldMode eXDMDisplayFieldMode;
   BDBG_ENTER(BXVD_SetDisplayFieldMode_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   switch ( eDisplayFieldType )
   {
      case BXVD_DisplayFieldType_eBothField:
         eXDMDisplayFieldMode = BXDM_PictureProvider_DisplayFieldMode_eBothField;
         break;

      case BXVD_DisplayFieldType_eTopFieldOnly:
         eXDMDisplayFieldMode = BXDM_PictureProvider_DisplayFieldMode_eTopFieldOnly;
         break;

      case BXVD_DisplayFieldType_eBottomFieldOnly:
         eXDMDisplayFieldMode = BXDM_PictureProvider_DisplayFieldMode_eBottomFieldOnly;
         break;

      case BXVD_DisplayFieldType_eSingleField:
         eXDMDisplayFieldMode = BXDM_PictureProvider_DisplayFieldMode_eSingleField;
         break;

      case BXVD_DisplayFieldType_eAuto:
         eXDMDisplayFieldMode = BXDM_PictureProvider_DisplayFieldMode_eAuto;
         break;

      default:
      BXVD_DBG_ERR(hXvdCh, ("Invalid display field mode: 0x%x (%d)",
                                  eDisplayFieldType, eDisplayFieldType));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   rc = BXDM_PictureProvider_SetDisplayFieldMode_isr(
            hXvdCh->hPictureProvider,
            eXDMDisplayFieldMode);

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetDisplayFieldMode(%d)", eDisplayFieldType));

   BDBG_LEAVE(BXVD_SetDisplayFieldMode_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
BXVD_GetDisplayFieldMode: Gets the video display mode status.
****************************************************************************/
BERR_Code BXVD_GetDisplayFieldMode
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_DisplayFieldType *peDisplayFieldType
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetDisplayFieldMode);

   rc = BXVD_GetDisplayFieldMode_isr(
      hXvdCh,
      peDisplayFieldType
      );

   BDBG_LEAVE(BXVD_GetDisplayFieldMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetDisplayFieldMode_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_DisplayFieldType *peDisplayFieldType
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_DisplayFieldMode eXDMDisplayFieldMode;

   BDBG_ENTER(BXVD_GetDisplayFieldMode_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(peDisplayFieldType);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetDisplayFieldMode_isr(
            hXvdCh->hPictureProvider,
            &eXDMDisplayFieldMode);

   if ( BERR_SUCCESS == rc )
   {
      switch ( eXDMDisplayFieldMode )
      {
         case BXDM_PictureProvider_DisplayFieldMode_eBothField:
            *peDisplayFieldType = BXVD_DisplayFieldType_eBothField;
            break;

         case BXDM_PictureProvider_DisplayFieldMode_eTopFieldOnly:
            *peDisplayFieldType = BXVD_DisplayFieldType_eTopFieldOnly;
            break;

         case BXDM_PictureProvider_DisplayFieldMode_eBottomFieldOnly:
            *peDisplayFieldType = BXVD_DisplayFieldType_eBottomFieldOnly;
            break;

         case BXDM_PictureProvider_DisplayFieldMode_eSingleField:
            *peDisplayFieldType = BXVD_DisplayFieldType_eSingleField;
            break;

         case BXDM_PictureProvider_DisplayFieldMode_eAuto:
            *peDisplayFieldType = BXVD_DisplayFieldType_eAuto;
            break;

         default:
            BXVD_DBG_ERR(hXvdCh, ("Unknown XDM display field mode: 0x%x (%d)",
                     eXDMDisplayFieldMode, eXDMDisplayFieldMode));

            return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }

   BDBG_LEAVE(BXVD_GetDisplayFieldMode_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
  BXVD_SetChannelChangeMode: Tell DM how to handle channel changes.
****************************************************************************/
BERR_Code BXVD_SetChannelChangeMode
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_ChannelChangeMode eChChangeMode
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetChannelChangeMode);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetChannelChangeMode_isr(
      hXvdCh,
      eChChangeMode
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetChannelChangeMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetChannelChangeMode_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_ChannelChangeMode eChChangeMode
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_ChannelChangeSettings stNewChannelChangeSettings;

   BXDM_DisplayInterruptHandler_Handle hXdmDihCurrent;

   bool bSquirrelAwayTheSettings;

   BDBG_ENTER(BXVD_SetChannelChangeMode_isr);
   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetChannelChangeSettings_isr(
            hXvdCh->hPictureProvider,
            &stNewChannelChangeSettings);

   if ( BERR_SUCCESS == rc )
   {
      switch ( eChChangeMode )
      {
         case BXVD_ChannelChangeMode_eMute:
            stNewChannelChangeSettings.bHoldLastPicture = false;
            stNewChannelChangeSettings.bFirstPicturePreview = false;
            break;

         case BXVD_ChannelChangeMode_eLastFramePreviousChannel:
            stNewChannelChangeSettings.bHoldLastPicture = true;
            stNewChannelChangeSettings.bFirstPicturePreview = false;
            break;

         case BXVD_ChannelChangeMode_eMuteWithFirstPicturePreview:
            stNewChannelChangeSettings.bHoldLastPicture = false;
            stNewChannelChangeSettings.bFirstPicturePreview = true;
            break;

         case BXVD_ChannelChangeMode_eLastFramePreviousWithFirstPicturePreview:
            stNewChannelChangeSettings.bHoldLastPicture = true;
            stNewChannelChangeSettings.bFirstPicturePreview = true;
            break;

         default:
            BXVD_DBG_ERR(hXvdCh, ("Invalid channel change parameter (%d)",
                     eChChangeMode));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
      }

      /* SW7425-1064: when the XMO is installed and active, the channel change mode is force
       * to NOT hold last picture.  After StopDecode is called, the channel change mode is 
       * restored to the original value.  So if the XMO is currently active, save these
       * new settings in the XMO structure.  They will be applied to XDM in BXVD_StopDecode.
       */
      bSquirrelAwayTheSettings =  ( BXVD_P_DecoderState_eActive == hXvdCh->eDecoderState );
      bSquirrelAwayTheSettings &= ( BXVD_P_ChannelType_eBase == hXvdCh->eChannelType );
      bSquirrelAwayTheSettings &= ( NULL != hXvdCh->hXmo ); 

      if ( true == bSquirrelAwayTheSettings )
      {
         BXDM_PictureProvider_XMO_SaveChannelChangeSettings_isr( 
                        hXvdCh->hXmo, 
                        &stNewChannelChangeSettings );
      }
      else
      {
         rc = BXDM_PictureProvider_SetChannelChangeSettings_isr(
                  hXvdCh->hPictureProvider,
                  &stNewChannelChangeSettings );
      }

      if ((eChChangeMode == BXVD_ChannelChangeMode_eMute) &&
          (hXvdCh->eDecoderState == BXVD_P_DecoderState_eNotActive))
      {
         /* Channel change mode is now eMute, the channel is stopped, 
          * disable picture provider. */
         BXDM_PictureProvider_GetDIH( hXvdCh->hPictureProvider, &hXdmDihCurrent );

         if ( NULL != hXdmDihCurrent )
         {

            BERR_TRACE(BXDM_DisplayInterruptHandler_SetPictureProviderMode_isr( 
                          hXdmDihCurrent,
                          BXDM_PictureProvider_GetPicture_isr,
                          hXvdCh->hPictureProvider,
                          BXDM_DisplayInterruptHandler_PictureProviderMode_eDisabled
                          ));
         }   
      }

      BXVD_DBG_MSG(hXvdCh, ("BXVD_SetChannelChangeMode(%d)", eChChangeMode));
   }

   BDBG_LEAVE(BXVD_SetChannelChangeMode_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
  BXVD_GetChannelChangeMode: Retrieve current channel change mode
****************************************************************************/
BERR_Code BXVD_GetChannelChangeMode
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_ChannelChangeMode *peChChangeMode
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetChannelChangeMode);

   rc = BXVD_GetChannelChangeMode_isr(
      hXvdCh,
      peChChangeMode
      );

   BDBG_LEAVE(BXVD_GetChannelChangeMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetChannelChangeMode_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_ChannelChangeMode *peChChangeMode
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_ChannelChangeSettings eCurrentChannelChangeSettings;

   BDBG_ENTER(BXVD_GetChannelChangeMode_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(peChChangeMode);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetChannelChangeSettings_isr(
            hXvdCh->hPictureProvider,
            &eCurrentChannelChangeSettings);

   if ( BERR_SUCCESS == rc )
   {
      if ( ( false == eCurrentChannelChangeSettings.bHoldLastPicture )
        && ( false == eCurrentChannelChangeSettings.bFirstPicturePreview ) )
      {
         *peChChangeMode = BXVD_ChannelChangeMode_eMute;
      } else if ( ( true == eCurrentChannelChangeSettings.bHoldLastPicture )
               && ( false == eCurrentChannelChangeSettings.bFirstPicturePreview ) )
      {
         *peChChangeMode = BXVD_ChannelChangeMode_eLastFramePreviousChannel;
      } else if ( ( false == eCurrentChannelChangeSettings.bHoldLastPicture )
               && ( true == eCurrentChannelChangeSettings.bFirstPicturePreview ) )
      {
         *peChChangeMode = BXVD_ChannelChangeMode_eMuteWithFirstPicturePreview;
      } else
      {
         *peChChangeMode = BXVD_ChannelChangeMode_eLastFramePreviousWithFirstPicturePreview;
      }
   }

   BDBG_LEAVE(BXVD_GetChannelChangeMode_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
  BXVD_SetInterpolationModeForStillContent: This API sets the SPIM for the
  video decoder.
****************************************************************************/
BERR_Code BXVD_SetInterpolationModeForStillContent
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_StillContentInterpolationMode eNewInterpolation
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetInterpolationModeForStillContent);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetInterpolationModeForStillContent_isr(
      hXvdCh,
      eNewInterpolation
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetInterpolationModeForStillContent);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetInterpolationModeForStillContent_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_StillContentInterpolationMode eNewInterpolation
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_SourceFormatOverride eSourceFormatOverride;
   BDBG_ENTER(BXVD_SetInterpolationModeForStillContent_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   hXvdCh->eSavedSPIM = eNewInterpolation;

   switch ( eNewInterpolation )
   {
      case BXVD_StillContentInterpolationMode_eSingleField:
         eSourceFormatOverride = BXDM_PictureProvider_SourceFormatOverride_eInterlaced;
         break;

      case BXVD_StillContentInterpolationMode_eBothField:
      case BXVD_StillContentInterpolationMode_eFrame:
         eSourceFormatOverride = BXDM_PictureProvider_SourceFormatOverride_eProgressive;
       break;

      case BXVD_StillContentInterpolationMode_eDefault:
         eSourceFormatOverride = BXDM_PictureProvider_SourceFormatOverride_eDefault;
         break;

      default:
         BXVD_DBG_ERR(hXvdCh, ("Invalid SPIM Mode (%d)",
                  eNewInterpolation));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   rc = BXDM_PictureProvider_SetSourceFormatOverride_isr(
            hXvdCh->hPictureProvider,
            eSourceFormatOverride
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetInterpolationModeForStillContent(%d)", eNewInterpolation));

   BDBG_LEAVE(BXVD_SetInterpolationModeForStillContent_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
  BXVD_GetInterpolationModeForStillContent: This function gets the still
  content interpolation mode(SPIM).
****************************************************************************/
BERR_Code BXVD_GetInterpolationModeForStillContent
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_StillContentInterpolationMode *peStillContentIntrpMode
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetInterpolationModeForStillContent);

   rc = BXVD_GetInterpolationModeForStillContent_isr(
      hXvdCh,
      peStillContentIntrpMode
      );

   BDBG_LEAVE(BXVD_GetInterpolationModeForStillContent);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetInterpolationModeForStillContent_isr
(
   BXVD_ChannelHandle   hXvdCh,
   BXVD_StillContentInterpolationMode *peStillContentIntrpMode
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_SourceFormatOverride eSourceFormatOverride;

   BDBG_ENTER(BXVD_GetInterpolationModeForStillContent_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(peStillContentIntrpMode);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetSourceFormatOverride_isr(
            hXvdCh->hPictureProvider,
            &eSourceFormatOverride
            );

   if ( BERR_SUCCESS == rc )
   {
      switch ( eSourceFormatOverride )
      {
         case BXDM_PictureProvider_SourceFormatOverride_eInterlaced:
            *peStillContentIntrpMode = BXVD_StillContentInterpolationMode_eSingleField;
            break;

         case BXDM_PictureProvider_SourceFormatOverride_eProgressive:
            *peStillContentIntrpMode = hXvdCh->eSavedSPIM;
            break;

         case BXDM_PictureProvider_SourceFormatOverride_eDefault:
            *peStillContentIntrpMode = BXVD_StillContentInterpolationMode_eDefault;
            break;

         default:
            BXVD_DBG_ERR(hXvdCh, ("Unknown XDM SPIM Mode (%d)",
                     eSourceFormatOverride));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }

   BDBG_LEAVE(BXVD_GetInterpolationModeForStillContent_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
  BXVD_SetInterpolationModeForMovingContent: This API sets the MPIM for
  the video decoder.
****************************************************************************/
BERR_Code BXVD_SetInterpolationModeForMovingContent
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_MovingContentInterpolationMode eNewInterpolation
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetInterpolationModeForMovingContent);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetInterpolationModeForMovingContent_isr(
      hXvdCh,
      eNewInterpolation
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetInterpolationModeForMovingContent);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetInterpolationModeForMovingContent_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_MovingContentInterpolationMode eNewInterpolation
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_ScanModeOverride eXDMScanModeOverride;
   BDBG_ENTER(BXVD_SetInterpolationModeForMovingContent_isr);
   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(eNewInterpolation <= BXVD_MovingContentInterpolationMode_eProgressiveScanout);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   switch ( eNewInterpolation )
   {
      case BXVD_MovingContentInterpolationMode_eInterlacedScanout:
         eXDMScanModeOverride = BXDM_PictureProvider_ScanModeOverride_eInterlaced;
         break;

      case BXVD_MovingContentInterpolationMode_eProgressiveScanout:
         eXDMScanModeOverride = BXDM_PictureProvider_ScanModeOverride_eProgressive;
         break;

      case BXVD_MovingContentInterpolationMode_eDefault:
         eXDMScanModeOverride = BXDM_PictureProvider_ScanModeOverride_eDefault;
         break;

      default:
         BXVD_DBG_ERR(hXvdCh, ("Invalid MPIM Mode (%d)",
                  eNewInterpolation));
         return BERR_TRACE(BERR_INVALID_PARAMETER);

   }

   rc = BXDM_PictureProvider_SetScanModeOverride_isr(
            hXvdCh->hPictureProvider,
            eXDMScanModeOverride
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetInterpolationModeForMovingContent(%d)", eNewInterpolation));

   BDBG_LEAVE(BXVD_SetInterpolationModeForMovingContent_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
  BXVD_GetInterpolationModeForMovingContent: This function gets interpolation
  mode for moving content.
****************************************************************************/
BERR_Code BXVD_GetInterpolationModeForMovingContent
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_MovingContentInterpolationMode *peNewInterpolation
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetInterpolationModeForMovingContent);

   rc = BXVD_GetInterpolationModeForMovingContent_isr(
      hXvdCh,
      peNewInterpolation
      );

   BDBG_LEAVE(BXVD_GetInterpolationModeForMovingContent);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetInterpolationModeForMovingContent_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_MovingContentInterpolationMode *peNewInterpolation
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_ScanModeOverride eScanModeOverride;

   BDBG_ENTER(BXVD_GetInterpolationModeForMovingContent_isr);
   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(peNewInterpolation);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetScanModeOverride_isr(
            hXvdCh->hPictureProvider,
            &eScanModeOverride
            );

   if ( BERR_SUCCESS == rc )
   {
      switch ( eScanModeOverride )
      {

         case BXDM_PictureProvider_ScanModeOverride_eInterlaced:
            *peNewInterpolation = BXVD_MovingContentInterpolationMode_eInterlacedScanout;
            break;

         case BXDM_PictureProvider_ScanModeOverride_eProgressive:
            *peNewInterpolation = BXVD_MovingContentInterpolationMode_eProgressiveScanout;
            break;

         case BXDM_PictureProvider_ScanModeOverride_eDefault:
            *peNewInterpolation = BXVD_MovingContentInterpolationMode_eDefault;
            break;

         default:
            BXVD_DBG_ERR(hXvdCh, ("Unknown XDM MPIM Mode (%d)",
                     eScanModeOverride));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }

   BDBG_LEAVE(BXVD_GetInterpolationModeForMovingContent_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
    BXVD_SetMonitorRefreshRate: Used to set the monitor refresh rate
****************************************************************************/
BERR_Code BXVD_SetMonitorRefreshRate
(
   BXVD_ChannelHandle hXvdCh,
   uint32_t ui32MonitorRefreshRate
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetMonitorRefreshRate);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetMonitorRefreshRate_isr(
      hXvdCh,
      ui32MonitorRefreshRate
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetMonitorRefreshRate);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetMonitorRefreshRate_isr
(
   BXVD_ChannelHandle hXvdCh,
   uint32_t ui32MonitorRefreshRate
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_MonitorRefreshRate eXDMMonitorRefreshRate;
   BDBG_ENTER(BXVD_SetMonitorRefreshRate_isr);
   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   switch ( ui32MonitorRefreshRate )
   {
      case BXVD_MONITOR_REFRESH_RATE_50Hz:
         eXDMMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_e50Hz;
         break;

      case BXVD_MONITOR_REFRESH_RATE_59_94Hz:
         eXDMMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_e59_94Hz;
         break;

      case BXVD_MONITOR_REFRESH_RATE_60Hz:
         eXDMMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_e60Hz;
         break;

      case BXVD_MONITOR_REFRESH_RATE_23_976Hz:
         eXDMMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_e23_976Hz;
         break;

      case BXVD_MONITOR_REFRESH_RATE_24Hz:
         eXDMMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_e24Hz;
         break;

      case BXVD_MONITOR_REFRESH_RATE_25Hz:
         eXDMMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_e25Hz;
         break;

      case BXVD_MONITOR_REFRESH_RATE_30Hz:
         eXDMMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_e30Hz;
         break;

      case BXVD_MONITOR_REFRESH_RATE_48Hz:
         eXDMMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_e48Hz;
         break;

      case BXVD_MONITOR_REFRESH_RATE_29_97Hz:
         eXDMMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_e29_97Hz;
         break;

      case BXVD_MONITOR_REFRESH_RATE_INVALID:
         eXDMMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_eUnknown;
         break;

      default:
         eXDMMonitorRefreshRate = BXDM_PictureProvider_MonitorRefreshRate_eUnknown;
         BXVD_DBG_ERR(hXvdCh, ("Invalid monitor refresh rate specified (defaulting to \"Unknown\"): %d", ui32MonitorRefreshRate));
         break;
   }

   rc = BXDM_PictureProvider_SetMonitorRefreshRate_isr(
            hXvdCh->hPictureProvider,
            eXDMMonitorRefreshRate
            );

   hXvdCh->sChSettings.ui32MonitorRefreshRate = ui32MonitorRefreshRate;

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetMonitorRefreshRate(%d)", ui32MonitorRefreshRate));

   BDBG_LEAVE(BXVD_SetMonitorRefreshRate_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
    BXVD_GetMonitorRefreshRate: Used to get the current value of the monitor
  refresh rate
****************************************************************************/
BERR_Code BXVD_GetMonitorRefreshRate
(
   BXVD_ChannelHandle hXvdCh,
   uint32_t *pui32MonitorRefreshRate
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetMonitorRefreshRate);

   rc = BXVD_GetMonitorRefreshRate_isr(
      hXvdCh,
      pui32MonitorRefreshRate
      );

   BDBG_LEAVE(BXVD_GetMonitorRefreshRate);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetMonitorRefreshRate_isr
(
   BXVD_ChannelHandle hXvdCh,
   uint32_t *pui32MonitorRefreshRate
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_MonitorRefreshRate eXDMMonitorRefreshRate;

   BDBG_ENTER(BXVD_GetMonitorRefreshRate_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pui32MonitorRefreshRate);

   /* Check for NULL handle */
   if (hXvdCh == NULL)
   {
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }      

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetMonitorRefreshRate_isr(
            hXvdCh->hPictureProvider,
            &eXDMMonitorRefreshRate
            );

   if ( BERR_SUCCESS == rc )
   {
      switch ( eXDMMonitorRefreshRate )
      {
         case BXDM_PictureProvider_MonitorRefreshRate_e50Hz:
            *pui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_50Hz;
            break;

         case BXDM_PictureProvider_MonitorRefreshRate_e59_94Hz:
            *pui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_59_94Hz;
            break;

         case BXDM_PictureProvider_MonitorRefreshRate_e60Hz:
            *pui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_60Hz;
            break;

         case BXDM_PictureProvider_MonitorRefreshRate_e23_976Hz:
            *pui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_23_976Hz;
            break;

         case BXDM_PictureProvider_MonitorRefreshRate_e24Hz:
            *pui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_24Hz;
            break;

         case BXDM_PictureProvider_MonitorRefreshRate_e25Hz:
            *pui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_25Hz;
            break;

         case BXDM_PictureProvider_MonitorRefreshRate_e30Hz:
            *pui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_30Hz;
            break;

         case BXDM_PictureProvider_MonitorRefreshRate_e48Hz:
            *pui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_48Hz;
            break;

         case BXDM_PictureProvider_MonitorRefreshRate_e29_97Hz:
            *pui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_29_97Hz;
            break;

         case BXDM_PictureProvider_MonitorRefreshRate_eUnknown:
            *pui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_INVALID;
            break;

         default:
            *pui32MonitorRefreshRate = BXVD_MONITOR_REFRESH_RATE_INVALID;
            BXVD_DBG_ERR(hXvdCh, ("Unknown XDM monitor refresh rate specified: %d", eXDMMonitorRefreshRate));
            rc = BERR_UNKNOWN;
            break;
      }
   }

   BDBG_LEAVE(BXVD_GetMonitorRefreshRate_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
  BXVD_EnableVideoFreeze: Used to freeze video while the decoder continues
  to run
****************************************************************************/
BERR_Code BXVD_EnableVideoFreeze
(
   BXVD_ChannelHandle hXvdCh
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_EnableVideoFreeze);

   BKNI_EnterCriticalSection();
   rc = BXVD_EnableVideoFreeze_isr(
      hXvdCh
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_EnableVideoFreeze);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_EnableVideoFreeze_isr
(
   BXVD_ChannelHandle hXvdCh
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_EnableVideoFreeze_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_SetFreeze_isr(
            hXvdCh->hPictureProvider,
            true);

   BXVD_DBG_MSG(hXvdCh, ("BXVD_EnableVideoFreeze()"));

   BDBG_LEAVE(BXVD_EnableVideoFreeze_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
  BXVD_DisableVideoFreeze: Used to re-enable previously frozen video
****************************************************************************/
BERR_Code BXVD_DisableVideoFreeze
(
   BXVD_ChannelHandle hXvdCh
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_DisableVideoFreeze);

   BKNI_EnterCriticalSection();
   rc = BXVD_DisableVideoFreeze_isr(
      hXvdCh
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_DisableVideoFreeze);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_DisableVideoFreeze_isr
(
   BXVD_ChannelHandle hXvdCh
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_DisaableVideoFreeze_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_SetFreeze_isr(
            hXvdCh->hPictureProvider,
            false);

   BXVD_DBG_MSG(hXvdCh, ("BXVD_DisableVideoFreeze()"));

   BDBG_LEAVE(BXVD_DisableVideoFreeze_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
  BXVD_GetVideoFreezeState: Used to get the video freeze state
****************************************************************************/
BERR_Code BXVD_GetVideoFreezeState
(
   BXVD_ChannelHandle hXvdCh,
   bool               *bVFState
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetVideoFreezeState);

   rc = BXVD_GetVideoFreezeState_isr(
      hXvdCh,
      bVFState
      );

   BDBG_LEAVE(BXVD_GetVideoFreezeState);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetVideoFreezeState_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool               *bVFState
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_GetVideoFreezeState_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(bVFState);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetFreeze_isr(
            hXvdCh->hPictureProvider,
            bVFState);

   BDBG_LEAVE(BXVD_GetVideoFreezeState_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
****************************************************************************/
BERR_Code BXVD_Set1080pScanMode
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_1080pScanMode e1080pScanMode /* [in] The new 1080p scan mode */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_Set1080pScanMode);

   BKNI_EnterCriticalSection();
   rc = BXVD_Set1080pScanMode_isr(
      hXvdCh,
      e1080pScanMode
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_Set1080pScanMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_Set1080pScanMode_isr
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_1080pScanMode e1080pScanMode /* [in] The new 1080p scan mode */
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_1080pScanMode eXDM1080pScanMode;

   BDBG_ENTER(BXVD_Set1080pScanMode_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   switch ( e1080pScanMode )
   {
      case BXVD_1080pScanMode_eDefault:
         eXDM1080pScanMode = BXDM_PictureProvider_1080pScanMode_eDefault;
         break;

      case BXVD_1080pScanMode_eAdvanced:
         eXDM1080pScanMode = BXDM_PictureProvider_1080pScanMode_eAdvanced;
         break;

      default:
      BXVD_DBG_ERR(hXvdCh, ("Invalid 1080p scan mode specified: %d", e1080pScanMode));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   rc = BXDM_PictureProvider_Set1080pScanMode_isr(
            hXvdCh->hPictureProvider,
            eXDM1080pScanMode
            );

   hXvdCh->sChSettings.e1080pScanMode = e1080pScanMode;
   BXVD_DBG_MSG(hXvdCh, ("BXVD_Set1080pScanMode(%d)", e1080pScanMode));

   BDBG_LEAVE(BXVD_Set1080pScanMode_isr);
   return BERR_SUCCESS;
}

/***************************************************************************
****************************************************************************/
BERR_Code BXVD_Get1080pScanMode
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_1080pScanMode *pe1080pScanMode /* [out] The current 1080p scan mode */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_Get1080pScanMode);

   rc = BXVD_Get1080pScanMode_isr(
      hXvdCh,
      pe1080pScanMode
      );

   BDBG_LEAVE(BXVD_Get1080pScanMode);

   return BERR_TRACE(rc);
}

BERR_Code BXVD_Get1080pScanMode_isr
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_1080pScanMode *pe1080pScanMode /* [out] The current 1080p scan mode */
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_1080pScanMode eXDM1080pScanMode;

   BDBG_ENTER(BXVD_Get1080pScanMode_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pe1080pScanMode);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_Get1080pScanMode_isr(
            hXvdCh->hPictureProvider,
            &eXDM1080pScanMode
            );

   if ( BERR_SUCCESS == rc )
   {
      switch ( eXDM1080pScanMode )
      {
         case BXDM_PictureProvider_1080pScanMode_eDefault:
            *pe1080pScanMode = BXVD_1080pScanMode_eDefault;
            break;

         case BXDM_PictureProvider_1080pScanMode_eAdvanced:
            *pe1080pScanMode = BXVD_1080pScanMode_eAdvanced;
            break;

         default:
            BXVD_DBG_ERR(hXvdCh, ("Unknown XDM 1080p scan mode: %d", eXDM1080pScanMode));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }

   BDBG_LEAVE(BXVD_Get1080pScanMode_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
****************************************************************************/
BERR_Code BXVD_Set240iScanMode
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_240iScanMode e240iScanMode /* [in] The new 240i scan mode */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_Set240iScanMode);

   BKNI_EnterCriticalSection();
   rc = BXVD_Set240iScanMode_isr(
      hXvdCh,
      e240iScanMode
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_Set240iScanMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_Set240iScanMode_isr
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_240iScanMode e240iScanMode /* [in] The new 240i scan mode */
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_240iScanMode eXDM240iScanMode;
   BDBG_ENTER(BXVD_Set240iScanMode_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   switch ( e240iScanMode )
   {
      case BXVD_240iScanMode_eForceProgressive:
         eXDM240iScanMode = BXDM_PictureProvider_240iScanMode_eForceProgressive;
         break;

      case BXVD_240iScanMode_eUseEncodedFormat:
         eXDM240iScanMode = BXDM_PictureProvider_240iScanMode_eUseEncodedFormat;
         break;

      default:
      BXVD_DBG_ERR(hXvdCh, ("Invalid 240i scan mode specified: %d", e240iScanMode));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   rc = BXDM_PictureProvider_Set240iScanMode_isr(
            hXvdCh->hPictureProvider,
            eXDM240iScanMode
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_Set240iScanMode(%d)", e240iScanMode));

   BDBG_LEAVE(BXVD_Set240iScanMode_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
****************************************************************************/
BERR_Code BXVD_Get240iScanMode
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_240iScanMode *pe240iScanMode /* [out] The current 240i scan mode */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_Get240iScanMode);

   rc = BXVD_Get240iScanMode_isr(
      hXvdCh,
      pe240iScanMode
      );

   BDBG_LEAVE(BXVD_Get240iScanMode);

   return BERR_TRACE(rc);
}

BERR_Code BXVD_Get240iScanMode_isr
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_240iScanMode *pe240iScanMode /* [out] The current 240i scan mode */
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_240iScanMode eXDM240iScanMode;

   BDBG_ENTER(BXVD_Get240iScanMode_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pe240iScanMode);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_Get240iScanMode_isr(
            hXvdCh->hPictureProvider,
            &eXDM240iScanMode
            );

   if ( BERR_SUCCESS == rc )
   {
      switch ( eXDM240iScanMode )
      {
         case BXDM_PictureProvider_240iScanMode_eForceProgressive:
            *pe240iScanMode = BXVD_240iScanMode_eForceProgressive;
            break;

           /* Assume non-AVC 240i/288i is coded correctly */
         case BXDM_PictureProvider_240iScanMode_eUseEncodedFormat:
            *pe240iScanMode = BXVD_240iScanMode_eUseEncodedFormat;
            break;

         default:
            BXVD_DBG_ERR(hXvdCh, ("Unknown XDM 240i scan mode: %d", eXDM240iScanMode));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }

   BDBG_LEAVE(BXVD_Get240iScanMode_isr);
   return BERR_TRACE( rc );
}


/***************************************************************************
****************************************************************************/
BERR_Code BXVD_SetPictureDropMode
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_PictureDropMode ePictureDropMode /* [in] the picture drop mode */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetPictureDropMode);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetPictureDropMode_isr(
      hXvdCh,
      ePictureDropMode
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetPictureDropMode);
   return BERR_TRACE(rc);
}

/***************************************************************************
****************************************************************************/
BERR_Code BXVD_SetPictureDropMode_isr
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_PictureDropMode ePictureDropMode /* [in] the picture drop mode */
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_PictureDropMode eXDMPictureDropMode;
   BDBG_ENTER(BXVD_SetPictureDropMode_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   switch ( ePictureDropMode )
   {
      case BXVD_PictureDropMode_eField:
         eXDMPictureDropMode = BXDM_PictureProvider_PictureDropMode_eField;
         break;

      case BXVD_PictureDropMode_eFrame:
         eXDMPictureDropMode = BXDM_PictureProvider_PictureDropMode_eFrame;
         break;

      default:
         BXVD_DBG_ERR(hXvdCh, ("Invalid picture drop mode specified: %d", ePictureDropMode));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   rc = BXDM_PictureProvider_SetPictureDropMode_isr(
            hXvdCh->hPictureProvider,
            eXDMPictureDropMode
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetPictureDropMode(%d)", ePictureDropMode));

   BDBG_LEAVE(BXVD_SetPictureDropMode_isr);

   return BERR_TRACE( rc );
}

/***************************************************************************
****************************************************************************/
BERR_Code BXVD_GetPictureDropMode
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_PictureDropMode *pePictureDropMode /* [out] the current picture drop mode */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetPictureDropMode);

   rc = BXVD_GetPictureDropMode_isr(
      hXvdCh,
      pePictureDropMode
      );

   BDBG_LEAVE(BXVD_GetPictureDropMode);
   return BERR_TRACE(rc);
}

/***************************************************************************
****************************************************************************/
BERR_Code BXVD_GetPictureDropMode_isr(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   BXVD_PictureDropMode *pePictureDropMode /* [out] the current picture drop mode */
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_PictureDropMode eXDMPictureDropMode;

   BDBG_ENTER(BXVD_GetPictureDropMode_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pePictureDropMode);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetPictureDropMode_isr(
            hXvdCh->hPictureProvider,
            &eXDMPictureDropMode
            );

   if ( BERR_SUCCESS == rc )
   {
      switch ( eXDMPictureDropMode )
      {
         case BXDM_PictureProvider_PictureDropMode_eField:
            *pePictureDropMode = BXVD_PictureDropMode_eField;
            break;

         case BXDM_PictureProvider_PictureDropMode_eFrame:
            *pePictureDropMode = BXVD_PictureDropMode_eFrame;
            break;

         default:
            BXVD_DBG_ERR(hXvdCh, ("Unknown XDM picture drop mode: %d", eXDMPictureDropMode));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }

   BDBG_LEAVE(BXVD_GetPictureDropMode_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************

Summary:
   SW7405-4703: API to Set Horizontal Overscan calculation mode

****************************************************************************/
BERR_Code BXVD_SetHorizontalOverscanMode(
   BXVD_ChannelHandle     hXvdCh,                  /* [in] XVD Channel handle */
   BXVD_HorizontalOverscanMode  eHorizOverscanMode /* [in] Horizontal Overscan mode  */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetHorizontalOverscanMode);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetHorizontalOverscanMode_isr(
      hXvdCh,
      eHorizOverscanMode
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetHorizontalOverscanMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetHorizontalOverscanMode_isr(
   BXVD_ChannelHandle     hXvdCh,                  /* [in] XVD Channel handle */
   BXVD_HorizontalOverscanMode  eHorizOverscanMode /* [in] Horizontal Overscan mode  */
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_HorizontalOverscanMode eXDMHorizOverscanMode;
   BDBG_ENTER(BXVD_SetPictureDropMode_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   switch ( eHorizOverscanMode )
   {
      case BXVD_HorizontalOverscanMode_eAuto:
         eXDMHorizOverscanMode = BXDM_PictureProvider_HorizontalOverscanMode_eAuto;
         break;

      case BXVD_HorizontalOverscanMode_eDisable:
         eXDMHorizOverscanMode = BXDM_PictureProvider_HorizontalOverscanMode_eDisable;
         break;

      default:
         BXVD_DBG_ERR(hXvdCh, ("Invalid horizontal overscan mode specified: %d", eHorizOverscanMode));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   rc = BXDM_PictureProvider_SetHorizontalOverscanMode_isr(
            hXvdCh->hPictureProvider,
            eXDMHorizOverscanMode
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetHorizontalOverscanMode_isr(%d)", eXDMHorizOverscanMode));

   BDBG_LEAVE(BXVD_SetHorizontalOverscanMode_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************

Summary:
   SW7405-4703: API to Get current Horizontal Overscan mode

****************************************************************************/

BERR_Code BXVD_GetHorizontalOverscanMode
(
   BXVD_ChannelHandle           hXvdCh,               /* [in] XVD Channel handle */
   BXVD_HorizontalOverscanMode  *peHorizOverscanMode  /* [out] Horizontal Overscan mode  */
)
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetHorizontalOverscanMode);

   rc = BXVD_GetHorizontalOverscanMode_isr(
      hXvdCh,
      peHorizOverscanMode
      );

   BDBG_LEAVE(BXVD_GetHorizontalOverscanMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetHorizontalOverscanMode_isr
(
   BXVD_ChannelHandle           hXvdCh,               /* [in] XVD Channel handle */
   BXVD_HorizontalOverscanMode  *peHorizOverscanMode  /* [out] Horizontal Overscan mode  */
)
{
   BERR_Code rc;
   BXDM_PictureProvider_HorizontalOverscanMode eXDMHorizOverscanMode;

   BDBG_ENTER(BXVD_GetHorizontalOverscanMode_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(peHorizOverscanMode);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetHorizontalOverscanMode_isr(
            hXvdCh->hPictureProvider,
            &eXDMHorizOverscanMode
            );

   if ( BERR_SUCCESS == rc )
   {
      switch ( eXDMHorizOverscanMode )
      {
         case BXDM_PictureProvider_HorizontalOverscanMode_eAuto:
            *peHorizOverscanMode = BXVD_HorizontalOverscanMode_eAuto;
            break;

         case BXDM_PictureProvider_HorizontalOverscanMode_eDisable:
            *peHorizOverscanMode = BXVD_HorizontalOverscanMode_eDisable;
            break;

         default:
            BXVD_DBG_ERR(hXvdCh, ("Unknown XDM Horizontal Overscan mode: %d", eXDMHorizOverscanMode));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }

   BDBG_LEAVE(BXVD_GetHorizontalOverscanMode_isr);
   return BERR_TRACE( rc );
}

/******************************/
/* Display Manager APIs - TSM */
/******************************/

/***************************************************************************
BXVD_SetDisplayOffset: API used to set the display offset.
****************************************************************************/
BERR_Code BXVD_SetDisplayOffset
(
   BXVD_ChannelHandle hXvdCh,
   long               lDisplayOffsetValue
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetDisplayOffset);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetDisplayOffset_isr(
      hXvdCh,
      lDisplayOffsetValue
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetDisplayOffset);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetDisplayOffset_isr
(
   BXVD_ChannelHandle hXvdCh,
   long               lDisplayOffsetValue
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_SetDisplayOffset_isr);
   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* Set the display offset value */
   rc = BXDM_PictureProvider_SetPTSOffset_isr(
            hXvdCh->hPictureProvider,
            (uint32_t) lDisplayOffsetValue
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetDisplayOffset(%d)", lDisplayOffsetValue));

   BDBG_LEAVE(BXVD_SetDisplayOffset_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
BXVD_GetDisplayOffset: API used to get the display offset.
****************************************************************************/
BERR_Code BXVD_GetDisplayOffset
(
   BXVD_ChannelHandle hXvdCh,
   long               *plDisplayOffsetValue
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetDisplayOffset);

   rc = BXVD_GetDisplayOffset_isr(
      hXvdCh,
      plDisplayOffsetValue);

   BDBG_LEAVE(BXVD_GetDisplayOffset);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetDisplayOffset_isr
(
   BXVD_ChannelHandle hXvdCh,
   long               *plDisplayOffsetValue
   )
{
   BERR_Code rc;
   uint32_t uiPTSOffset;

   BDBG_ENTER(BXVD_GetDisplayOffset_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(plDisplayOffsetValue);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* get the display offset value */
   rc = BXDM_PictureProvider_GetPTSOffset_isr(
            hXvdCh->hPictureProvider,
            &uiPTSOffset
            );

   *plDisplayOffsetValue = uiPTSOffset;

   BDBG_LEAVE(BXVD_GetDisplayOffset_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
BXVD_SetVideoDisplayMode: Used to set the display mode.
****************************************************************************/
BERR_Code BXVD_SetVideoDisplayMode
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_DisplayMode   eDisplayMode
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetVideoDisplayMode);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetVideoDisplayMode_isr(
      hXvdCh,
      eDisplayMode
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetVideoDisplayMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetVideoDisplayMode_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_DisplayMode   eDisplayMode
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_DisplayMode eXDMDisplayMode;
   BDBG_ENTER(BXVD_SetVideoDisplayMode_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   switch ( eDisplayMode )
   {
      case BXVD_DisplayMode_eTSMMode:
         eXDMDisplayMode = BXDM_PictureProvider_DisplayMode_eTSM;
         break;

      case BXVD_DisplayMode_eVSYNCMode:
         eXDMDisplayMode = BXDM_PictureProvider_DisplayMode_eVirtualTSM;
         break;

      default:
      BXVD_DBG_ERR(hXvdCh, ("Invalid video display mode: 0x%x (%d)",
                                 eDisplayMode, eDisplayMode));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* If DQT is enabled, force VSYNC mode. */

   if ( true == hXvdCh->stDecoderContext.bReversePlayback )
   {
      eXDMDisplayMode = BXDM_PictureProvider_DisplayMode_eVirtualTSM;
   }


   /* Set the video display mode */
   rc = BXDM_PictureProvider_SetDisplayMode_isr(
            hXvdCh->hPictureProvider,
            eXDMDisplayMode
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetVideoDisplayMode(%d)", eDisplayMode));

   BDBG_LEAVE(BXVD_SetVideoDisplayMode_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
BXVD_GetVideoDisplayMode: Used to get the display mode.
****************************************************************************/
BERR_Code BXVD_GetVideoDisplayMode
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_DisplayMode   *peDisplayMode
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetVideoDisplayMode);

   rc = BXVD_GetVideoDisplayMode_isr(
      hXvdCh,
      peDisplayMode
      );

   BDBG_LEAVE(BXVD_GetVideoDisplayMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetVideoDisplayMode_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_DisplayMode   *peDisplayMode
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_DisplayMode eXDMDisplayMode;
   BDBG_ENTER(BXVD_GetVideoDisplayMode_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(peDisplayMode);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* Get the video display mode */
   rc = BXDM_PictureProvider_GetDisplayMode_isr(
            hXvdCh->hPictureProvider,
            &eXDMDisplayMode
            );

   if ( BERR_SUCCESS == rc )
   {
      switch ( eXDMDisplayMode )
      {
         case BXDM_PictureProvider_DisplayMode_eTSM:
            *peDisplayMode = BXVD_DisplayMode_eTSMMode;
            break;

         case BXDM_PictureProvider_DisplayMode_eVirtualTSM:
            *peDisplayMode = BXVD_DisplayMode_eVSYNCMode;
            break;

         default:
            BXVD_DBG_ERR(hXvdCh, ("Unknown XDM video display mode: 0x%x (%d)",
                                       eXDMDisplayMode, eXDMDisplayMode));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }

   BDBG_LEAVE(BXVD_GetVideoDisplayMode_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
  BXVD_GetDisplayThresholds: API used to get display threshold values
****************************************************************************/
BERR_Code BXVD_GetDisplayThresholds
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_DisplayThresholds *pDispThresholds
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetDisplayThresholds);

   BKNI_EnterCriticalSection();
   rc = BXVD_GetDisplayThresholds_isr(
      hXvdCh,
      pDispThresholds
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetDisplayThresholds);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetDisplayThresholds_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_DisplayThresholds *pDispThresholds
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_TSMThresholdSettings eCurrentTSMThresholdSettings;

   BDBG_ENTER(BXVD_GetDisplayThresholds_isr);

   /* Check for validity of input ptrs */
   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pDispThresholds);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetTSMThresholdSettings_isr(
            hXvdCh->hPictureProvider,
            &eCurrentTSMThresholdSettings
            );

   pDispThresholds->ui32DiscardThreshold =
            eCurrentTSMThresholdSettings.uiTooEarlyThreshold;

   pDispThresholds->ui32VeryLateThreshold =
            eCurrentTSMThresholdSettings.uiTooLateThreshold;

   BDBG_LEAVE(BXVD_GetDisplayThresholds_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
  BXVD_SetDiscardThreshold: API used to supply the decoder with display
  discard threshold.
****************************************************************************/
BERR_Code BXVD_SetDiscardThreshold
(
   BXVD_ChannelHandle hXvdCh,
   uint32_t           ui32DiscardThreshold
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetDiscardThreshold);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetDiscardThreshold_isr(
      hXvdCh,
      ui32DiscardThreshold
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetDiscardThreshold);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetDiscardThreshold_isr
(
   BXVD_ChannelHandle hXvdCh,
   uint32_t           ui32DiscardThreshold
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_TSMThresholdSettings eCurrentTSMThresholdSettings;

   BDBG_ENTER(BXVD_SetDiscardThreshold_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetTSMThresholdSettings_isr(
            hXvdCh->hPictureProvider,
            &eCurrentTSMThresholdSettings
            );

   if ( BERR_SUCCESS == rc )
   {
      eCurrentTSMThresholdSettings.uiTooEarlyThreshold = ui32DiscardThreshold;

      rc = BXDM_PictureProvider_SetTSMThresholdSettings_isr(
               hXvdCh->hPictureProvider,
               &eCurrentTSMThresholdSettings
               );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetDiscardThreshold(%d)", ui32DiscardThreshold));
   }

   BDBG_LEAVE(BXVD_SetDiscardThreshold_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
  BXVD_SetVeryLateThreshold:    API used to supply the decoder with display
  very late threshold.
****************************************************************************/
BERR_Code BXVD_SetVeryLateThreshold
(
   BXVD_ChannelHandle hXvdCh,
   uint32_t           ui32VeryLateThreshold
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetVeryLateThreshold);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetVeryLateThreshold_isr(
      hXvdCh,
      ui32VeryLateThreshold
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetVeryLateThreshold);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetVeryLateThreshold_isr
(
   BXVD_ChannelHandle hXvdCh,
   uint32_t           ui32VeryLateThreshold
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_TSMThresholdSettings eCurrentTSMThresholdSettings;
   BDBG_ENTER(BXVD_SetVeryLateThreshold_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetTSMThresholdSettings_isr(
            hXvdCh->hPictureProvider,
            &eCurrentTSMThresholdSettings
            );

   if ( BERR_SUCCESS == rc )
   {
      eCurrentTSMThresholdSettings.uiTooLateThreshold = ui32VeryLateThreshold;

      rc = BXDM_PictureProvider_SetTSMThresholdSettings_isr(
               hXvdCh->hPictureProvider,
               &eCurrentTSMThresholdSettings
               );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetVeryLateThreshold(%d)", ui32VeryLateThreshold));
   }

   BDBG_LEAVE(BXVD_SetVeryLateThreshold_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
BXVD_SetSTCInvalidFlag: Used to set the state of the STC invalid flag
****************************************************************************/
BERR_Code BXVD_SetSTCInvalidFlag
(
   BXVD_ChannelHandle hXvdCh,
   bool bStcInvalidFlag
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_SetSTCInvalidFlag);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetSTCInvalidFlag_isr(
      hXvdCh,
      bStcInvalidFlag
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetSTCInvalidFlag);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetSTCInvalidFlag_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool bStcInvalidFlag
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_SetSTCInvalidFlag_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_SetSTCValid_isr(
            hXvdCh->hPictureProvider,
            !bStcInvalidFlag);

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetSTCInvalidFlag(%d)", bStcInvalidFlag));

   BDBG_LEAVE(BXVD_SetSTCInvalidFlag_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
BXVD_GetSTCInvalidFlag: Used to get the current state of the STC invalid flag
****************************************************************************/
BERR_Code BXVD_GetSTCInvalidFlag
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbStcInvalidFlag
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetStcInvalidFlag);

   rc = BXVD_GetSTCInvalidFlag_isr(
      hXvdCh,
      pbStcInvalidFlag
      );

   BDBG_LEAVE(BXVD_GetStcInvalidFlag);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetSTCInvalidFlag_isr
(
   BXVD_ChannelHandle hXvdCh,
   bool *pbStcInvalidFlag
   )
{
   BERR_Code rc;
   bool bCurrentSTCValid;
   BDBG_ENTER(BXVD_GetStcInvalidFlag_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pbStcInvalidFlag);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetSTCValid_isr(
            hXvdCh->hPictureProvider,
            &bCurrentSTCValid);

   *pbStcInvalidFlag = !bCurrentSTCValid;

   BDBG_LEAVE(BXVD_GetStcInvalidFlag_isr);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
BXVD_SetPtsStcDiffThreshold: Set the Pts/Stc difference threshold
****************************************************************************/
BERR_Code BXVD_SetPtsStcDiffThreshold
(
   BXVD_ChannelHandle hXvdCh,
   long uiPtsStcDiffThreshold
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetPtsStcDiffThreshold);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetPtsStcDiffThreshold_isr(
      hXvdCh,
      uiPtsStcDiffThreshold
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetPtsStcDiffThreshold);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetPtsStcDiffThreshold_isr
(
   BXVD_ChannelHandle hXvdCh,
   long uiPtsStcDiffThreshold
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_TSMThresholdSettings eCurrentTSMThresholdSettings;

   BDBG_ENTER(BXVD_SetPtsStcDiffThreshold_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetTSMThresholdSettings_isr(
            hXvdCh->hPictureProvider,
            &eCurrentTSMThresholdSettings
            );

   if ( BERR_SUCCESS == rc )
   {
      eCurrentTSMThresholdSettings.uiDeltaStcPtsDiffThreshold = uiPtsStcDiffThreshold;

      rc = BXDM_PictureProvider_SetTSMThresholdSettings_isr(
               hXvdCh->hPictureProvider,
               &eCurrentTSMThresholdSettings
               );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetPtsStcDiffThreshold(%d)", uiPtsStcDiffThreshold));
   }

   BDBG_LEAVE(BXVD_SetPtsStcDiffThreshold_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
BXVD_GetPtsStcDiffThreshold: Get the Pts/Stc difference threshold
****************************************************************************/
BERR_Code BXVD_GetPtsStcDiffThreshold
(
   BXVD_ChannelHandle hXvdCh,
   long *puiPtsStcDiffThreshold
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetPtsStcDiffThreshold);

   rc = BXVD_GetPtsStcDiffThreshold_isr(
      hXvdCh,
      puiPtsStcDiffThreshold
      );

   BDBG_LEAVE(BXVD_GetPtsStcDiffThreshold);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetPtsStcDiffThreshold_isr
(
   BXVD_ChannelHandle hXvdCh,
   long *puiPtsStcDiffThreshold
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_TSMThresholdSettings eCurrentTSMThresholdSettings;

   BDBG_ENTER(BXVD_GetPtsStcDiffThreshold_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(puiPtsStcDiffThreshold);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetTSMThresholdSettings_isr(
            hXvdCh->hPictureProvider,
            &eCurrentTSMThresholdSettings
            );

   *puiPtsStcDiffThreshold = eCurrentTSMThresholdSettings.uiDeltaStcPtsDiffThreshold;

   BDBG_LEAVE(BXVD_GetPtsStcDiffThreshold_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
   BXVD_SetSTCSource: Set the STC source
****************************************************************************/
BERR_Code BXVD_SetSTCSource
(
   BXVD_ChannelHandle hXvdCh,  /* [in] The XVD Channel handle */
   BXVD_STC eSTC               /* [in] STC Time base */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetSTCSource);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetSTCSource_isr(
      hXvdCh,
      eSTC
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetSTCSource);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetSTCSource_isr(
   BXVD_ChannelHandle hXvdCh,  /* [in] The XVD Channel handle */
   BXVD_STC eSTC               /* [in] STC Time base */
   )
{
   BERR_Code rc;
   uint32_t uiXDMSTCIndex;

   BDBG_ENTER(BXVD_SetSTCSource_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   if (eSTC >= BXVD_P_STC_MAX)
   {
      BXVD_DBG_ERR(hXvdCh, ("Invalid STC: 0x%x (%d)",
                            eSTC, eSTC));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   switch ( eSTC )
   {
      case BXVD_STC_eZero:
         uiXDMSTCIndex = 0;
         break;

      case BXVD_STC_eOne:
         uiXDMSTCIndex = 1;
         break;

#if (BXVD_P_STC_MAX > 2)

      case BXVD_STC_eTwo:
         uiXDMSTCIndex = 2;
         break;

      case BXVD_STC_eThree:
         uiXDMSTCIndex = 3;
         break;

      case BXVD_STC_eFour:
         uiXDMSTCIndex = 4;
         break;

      case BXVD_STC_eFive:
         uiXDMSTCIndex = 5;
         break;

      case BXVD_STC_eSix:
         uiXDMSTCIndex = 6;
         break;

      case BXVD_STC_eSeven:
         uiXDMSTCIndex = 7;
         break;
#endif
      default:
         BXVD_DBG_ERR(hXvdCh, ("Invalid STC: 0x%x (%d)",
                                    eSTC, eSTC));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   rc = BXDM_PictureProvider_SetSTCIndex_isr(
            hXvdCh->hPictureProvider,
            uiXDMSTCIndex
            );

   hXvdCh->sDecodeSettings.eSTC = eSTC;
   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetSTCSource(%d)", eSTC));

   BDBG_LEAVE(BXVD_SetSTCSource_isr);
   return BERR_TRACE( rc );
}


/***************************************************************************
   BXVD_GetSTCSource: Get the STC source
****************************************************************************/
BERR_Code BXVD_GetSTCSource
(
   BXVD_ChannelHandle hXvdCh,  /* [in] The XVD Channel handle */
   BXVD_STC *peSTC             /* [out] STC Time base */
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_GetSTCSource);

   rc = BXVD_GetSTCSource_isr(
      hXvdCh,
      peSTC
      );

   BDBG_LEAVE(BXVD_GetSTCSource);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetSTCSource_isr
(
   BXVD_ChannelHandle hXvdCh,  /* [in] The XVD Channel handle */
   BXVD_STC *peSTC             /* [out] STC Time base */
   )
{
   BERR_Code rc;
   uint32_t uiXDMSTCIndex;

   BDBG_ENTER(BXVD_GetSTCSource_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(peSTC);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetSTCIndex_isr(
            hXvdCh->hPictureProvider,
            &uiXDMSTCIndex
            );

   if ( BERR_SUCCESS == rc )
   {
      switch ( uiXDMSTCIndex )
      {
         case 0:
            *peSTC = BXVD_STC_eZero;
            break;

         case 1:
            *peSTC = BXVD_STC_eOne;
            break;

         case 2:
            *peSTC = BXVD_STC_eTwo;
            break;

         case 3:
            *peSTC = BXVD_STC_eThree;
            break;

         case 4:
            *peSTC = BXVD_STC_eFour;
            break;

         case 5:
            *peSTC = BXVD_STC_eFive;
            break;

         case 6:
            *peSTC = BXVD_STC_eSix;
            break;

         case 7:
            *peSTC = BXVD_STC_eSeven;
            break;

         default:
            BXVD_DBG_ERR(hXvdCh, ("Unknown XDM STC Source: 0x%x (%d)",
                     uiXDMSTCIndex, uiXDMSTCIndex));

            return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }

   BDBG_LEAVE(BXVD_GetSTCSource_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
   BXVD_GetSTCValue: Get's the STC snapshot
****************************************************************************/
BERR_Code BXVD_GetSTCValue
(
   BXVD_Handle hXvd,                         /* [in] The XVD handle */
   BXVD_DisplayInterrupt eDisplayInterrupt,  /* [in] Display Interrupt number */
   BXVD_STC eSTC,                            /* [in] STC Time base */
   BXVD_STCInfo *pSTCInfo                    /* [out] STC info */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetSTCValue);

   BKNI_EnterCriticalSection();
   rc = BXVD_GetSTCValue_isr(
      hXvd,
      eDisplayInterrupt,
      eSTC,
      pSTCInfo
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetSTCValue);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetSTCValue_isr
(
   BXVD_Handle hXvd,                         /* [in] The XVD handle */
   BXVD_DisplayInterrupt eDisplayInterrupt,  /* [in] Display Interrupt number */
   BXVD_STC eSTC,                            /* [in] STC Time base */
   BXVD_STCInfo *pSTCInfo                    /* [out] STC info */
   )
{
   BERR_Code rc;
   BXDM_DisplayInterruptInfo stDisplayInterruptInfo;

   BDBG_ENTER(BXVD_GetSTCValue_isr);

   BDBG_ASSERT(hXvd);
   BDBG_ASSERT(pSTCInfo);

   /* Check handle type for correctness */
   if (hXvd->eHandleType != BXVD_P_HandleType_XvdMain)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   if ( eDisplayInterrupt >= BXVD_DisplayInterrupt_eMax )
   {
         BXVD_DBG_ERR(hXvd, ("Invalid Display Interrupt requested: 0x%x (%d)",
                                  eDisplayInterrupt));
      return BERR_TRACE(BERR_NOT_SUPPORTED);
   }

   rc = BXVD_DisplayInterruptProvider_GetDisplayInterruptInfo_isr(
            hXvd->hXvdDipCh[eDisplayInterrupt],
                                                              &stDisplayInterruptInfo );

   if ( BERR_SUCCESS == rc )
   {
   pSTCInfo->eInterruptPolarity = stDisplayInterruptInfo.eInterruptPolarity;

   if ( ( eSTC >= stDisplayInterruptInfo.uiSTCCount )
        || ( false == stDisplayInterruptInfo.astSTC[eSTC].bValid ) )
   {
         BXVD_DBG_ERR(hXvd, ("Invalid STC requested: 0x%x (%d)",
                                  eSTC));
         return BERR_TRACE(BERR_NOT_SUPPORTED);
   }

   pSTCInfo->ui32STCValue = stDisplayInterruptInfo.astSTC[eSTC].uiValue;
   }

   BDBG_LEAVE(BXVD_GetSTCValue_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
 ***************************************************************************/
BERR_Code BXVD_SetClipTime
(
   BXVD_ChannelHandle hXvdCh,  /* [in] The XVD Channel handle */
   BXVD_ClipTimeType eClipTimeType, /* [in] Clip Time Type */
   uint32_t    ui32StartTime, /* [in] Start display from the specified time/PTS */
   uint32_t    ui32StopTime /* [in] Stop display at the specified time/PTS */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetClipTime);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetClipTime_isr(
      hXvdCh,
      eClipTimeType,
      ui32StartTime,
      ui32StopTime
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetClipTime);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetClipTime_isr
(
   BXVD_ChannelHandle hXvdCh,  /* [in] The XVD Channel handle */
   BXVD_ClipTimeType eClipTimeType, /* [in] Clip Time Type */
   uint32_t    ui32StartTime, /* [in] Start display from the specified time/PTS */
   uint32_t    ui32StopTime /* [in] Stop display at the specified time/PTS */
   )
{
   BERR_Code rc;
   BXDM_PictureProvider_ClipTimeSettings stNewClipTimeSettings;

   BDBG_ENTER(BXVD_SetClipTime_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   BKNI_Memset( &stNewClipTimeSettings, 0, sizeof( BXDM_PictureProvider_ClipTimeSettings) );

   stNewClipTimeSettings.uiStart = ui32StartTime;
   stNewClipTimeSettings.uiStop = ui32StopTime;
   stNewClipTimeSettings.bValid = true;
   stNewClipTimeSettings.eType = eClipTimeType;

   rc = BXDM_PictureProvider_SetClipTimeSettings_isr(
            hXvdCh->hPictureProvider,
            &stNewClipTimeSettings
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetClipTime(%d,%u,%u)", eClipTimeType, ui32StartTime, ui32StopTime));

   BDBG_LEAVE(BXVD_SetClipTime_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
****************************************************************************/
BERR_Code BXVD_SetTSMWaitForValidSTC
(
   BXVD_ChannelHandle hXvdCh /* [in] The XVD Channel handle */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetTSMWaitForValidSTC);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetTSMWaitForValidSTC_isr(
      hXvdCh
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetTSMWaitForValidSTC);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetTSMWaitForValidSTC_isr
(
   BXVD_ChannelHandle hXvdCh /* [in] The XVD Channel handle */
   )
{
   BERR_Code rc = BERR_SUCCESS;

   BDBG_ENTER(BXVD_SetTSMWaitForValidSTC_isr);

   /* Check to make sure the RequestSTC callback is installed */
   if (hXvdCh->stInterruptCallbackInfo[BXVD_Interrupt_eRequestSTC].BXVD_P_pAppIntCallbackPtr == NULL)
   {
      BXVD_DBG_ERR(hXvdCh, ("BXVD_Interrupt_eRequestSTC callback must be installed"));
      return BERR_TRACE(BERR_NOT_INITIALIZED);
   }

   /* Enable the RequestSTC callback */
   rc = BXVD_EnableInterrupt_isr(hXvdCh,
                                 BXVD_Interrupt_eRequestSTC,
                                 BXVD_InterruptEnable_eEnable);
   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   /* Invalidate STC */
   rc = BXVD_SetSTCInvalidFlag_isr(hXvdCh,
                                   true);
   if (rc != BERR_SUCCESS)
   {
      return BERR_TRACE(rc);
   }

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetTSMWaitForValidSTC()"));

   BDBG_LEAVE(BXVD_SetTSMWaitForValidSTC_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
   BXVD_SetSwPcrOffset
****************************************************************************/
BERR_Code BXVD_SetSwPcrOffset
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   uint32_t uiSwPcrOffset     /* [in] software PCR offset */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetSwPcrOffset);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetSwPcrOffset_isr(
      hXvdCh,
      uiSwPcrOffset
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetSwPcrOffset);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetSwPcrOffset_isr
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   uint32_t uiSwPcrOffset     /* [in] software PCR offset */
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_SetSwPcrOffset_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_SetSoftwarePCROffset_isr(
            hXvdCh->hPictureProvider,
            uiSwPcrOffset
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetSwPcrOffset_isr(%d)", uiSwPcrOffset));

   BDBG_LEAVE(BXVD_SetSwPcrOffset_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
   BXVD_GetSwPcrOffset
****************************************************************************/
BERR_Code BXVD_GetSwPcrOffset
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   uint32_t * puiSwPcrOffset  /* [out] software PCR offset */
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetSwPcrOffset);

   rc = BXVD_GetSwPcrOffset_isr(
      hXvdCh,
      puiSwPcrOffset
      );

   BDBG_LEAVE(BXVD_GetSwPcrOffset);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetSwPcrOffset_isr
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   uint32_t * puiSwPcrOffset  /* [out] software PCR offset */
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_GetSwPcrOffset_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(puiSwPcrOffset);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetSoftwarePCROffset_isr(
            hXvdCh->hPictureProvider,
            puiSwPcrOffset
            );

   BDBG_LEAVE(BXVD_GetSwPcrOffset_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
   BXVD_SetHwPcrOffsetEnable
****************************************************************************/
BERR_Code BXVD_SetHwPcrOffsetEnable
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   bool bHwPcrOffsetEnable    /* [in] hardware PCR offset enable flag */
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_SetHwPcrOffsetEnable);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetHwPcrOffsetEnable_isr(
      hXvdCh,
      bHwPcrOffsetEnable
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetHwPcrOffsetEnable);
   return BERR_TRACE(rc);
}


BERR_Code BXVD_SetHwPcrOffsetEnable_isr
(
   BXVD_ChannelHandle hXvdCh, /* [in] The XVD Channel handle */
   bool bHwPcrOffsetEnable    /* [in] hardware PCR offset enable flag */
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_SetHwPcrOffsetEnable_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_SetHardwarePCROffsetMode_isr(
            hXvdCh->hPictureProvider,
            bHwPcrOffsetEnable
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetHwPcrOffsetEnable_isr(%d)", bHwPcrOffsetEnable));

   BDBG_LEAVE(BXVD_SetHwPcrOffsetEnable_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
   BXVD_GetHwPcrOffsetEnable
****************************************************************************/
BERR_Code BXVD_GetHwPcrOffsetEnable
(
   BXVD_ChannelHandle hXvdCh,    /* [in] The XVD Channel handle */
   bool * pbHwPcrOffsetEnable    /* [out] hardware PCR offset enable flag */
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_GetHwPcrOffsetEnable);

   rc = BXVD_GetHwPcrOffsetEnable_isr(
      hXvdCh,
      pbHwPcrOffsetEnable
      );

   BDBG_LEAVE(BXVD_GetHwPcrOffsetEnable);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetHwPcrOffsetEnable_isr
(
   BXVD_ChannelHandle hXvdCh,    /* [in] The XVD Channel handle */
   bool * pbHwPcrOffsetEnable    /* [out] hardware PCR offset enable flag */
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_GetHwPcrOffsetEnable_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pbHwPcrOffsetEnable);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetHardwarePCROffsetMode_isr(
            hXvdCh->hPictureProvider,
            pbHwPcrOffsetEnable
            );

   BDBG_LEAVE(BXVD_GetHwPcrOffsetEnable_isr);
   return BERR_TRACE( rc );
}

/***************************************************************************
   BXVD_SetVsyncPlaybackRate
   DEPRECATED: SW7400-2870: use BXVD_SetPlaybackRate(isr)
****************************************************************************/

BERR_Code BXVD_SetVsyncPlaybackRate(
   BXVD_ChannelHandle hXvdCh,
   uint32_t uiVsyncPlaybackRate
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_SetVsyncPlaybackRate);

   BKNI_EnterCriticalSection();

   rc = BXVD_SetVsyncPlaybackRate_isr( hXvdCh, uiVsyncPlaybackRate );

   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetVsyncPlaybackRate);

   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetVsyncPlaybackRate_isr(
   BXVD_ChannelHandle hXvdCh,
   uint32_t uiVsyncPlaybackRate
   )
{
   BERR_Code rc=BERR_SUCCESS;
   BXDM_Picture_Rate stPlaybackRate;
   BDBG_ENTER(BXVD_SetVsyncPlaybackRate_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* SW7400-2870: 
    * XDM uses a single variable to store the vsync playback rate.  This value is set 
    * both by this API and BXVD_PVR_EnablePause; the pause state has precedence.  
    * As a result, if the system is paused when this API is called, the new playback 
    * rate will be saved but not applied until the system is un-paused.
    */

   /* "uiVsyncPlaybackRate" is a percentage . */
   BKNI_Memset(&stPlaybackRate, 0, sizeof( BXDM_Picture_Rate ) );

   stPlaybackRate.uiNumerator = uiVsyncPlaybackRate;
   stPlaybackRate.uiDenominator = 100;

   /* Only apply the new playback rate if the system in NOT paused. */
   if ( false == hXvdCh->bPauseActive )
   {
      rc = BXDM_PictureProvider_SetPlaybackRate_isr(
            hXvdCh->hPictureProvider,
            &stPlaybackRate
            );
   }

   hXvdCh->stSavedPlaybackRate = stPlaybackRate;
   hXvdCh->bSavedPlaybackRateValid = true;

   BDBG_LEAVE(BXVD_SetVsyncPlaybackRate_isr);

   return BERR_TRACE( rc );
}

/***************************************************************************
   BXVD_GetVsyncPlaybackRate
   SW7400-2870:
   DEPRECATED: use BXVD_GetPlaybackRate(isr)
****************************************************************************/

BERR_Code BXVD_GetVsyncPlaybackRate(
   BXVD_ChannelHandle hXvdCh,
   uint32_t * puiVsyncPlaybackRate
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_GetVsyncPlaybackRate);

   BKNI_EnterCriticalSection();

   rc = BXVD_GetVsyncPlaybackRate_isr( hXvdCh, puiVsyncPlaybackRate );

   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetVsyncPlaybackRate);

   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetVsyncPlaybackRate_isr(
   BXVD_ChannelHandle hXvdCh,
   uint32_t * puiVsyncPlaybackRate
   )
{
   BERR_Code rc=BERR_SUCCESS;
   BXDM_Picture_Rate stCurrentPlaybackRate;
   BDBG_ENTER(BXVD_GetVsyncPlaybackRate_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(puiVsyncPlaybackRate);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   /* SW7400-2870: if not paused by a call to BXVD_PVR_EnablePause, retrieve 
    * the playback rate from XDM.  If paused, return the valued saved in the 
    * channel structure.
    */
   if ( false == hXvdCh->bPauseActive )
   {
      rc = BXDM_PictureProvider_GetPlaybackRate_isr(
                  hXvdCh->hPictureProvider,
                  &stCurrentPlaybackRate
                  );

      if ( BERR_SUCCESS != rc )
      {
         *puiVsyncPlaybackRate = 0;
         BXVD_DBG_WRN(hXvdCh, ("BXVD_GetVsyncPlaybackRate_isr() BXDM_PictureProvider_GetPlaybackRate_isr returned %d", rc ));
         return BERR_TRACE( rc );
      }
   }
   else
   {
      if ( true == hXvdCh->bSavedPlaybackRateValid )
      {
         stCurrentPlaybackRate = hXvdCh->stSavedPlaybackRate;
      }
      else
      {
         /* It should be impossible to hit this case.  If "bPauseActive" is true,
          * there should always be a saved playback rate.  Add this warning in the
          * event a logic bug creeps in.
          */
         stCurrentPlaybackRate.uiNumerator = 1;
         stCurrentPlaybackRate.uiDenominator = 1;
         BXVD_DBG_WRN(hXvdCh, ("BXVD_GetVsyncPlaybackRate_isr() bPauseActive but no saved playback rate."));
      }
   }

   /* Prevent a divide by '0'.*/
   if ( 0 == stCurrentPlaybackRate.uiDenominator )
   {
      *puiVsyncPlaybackRate = 0;
      BDBG_WRN(("BXVD_GetVsyncPlaybackRate_isr() uiDenominator == 0!"));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* "*puiVsyncPlaybackRate" is a percentage, multiply the numerator/demonintor ratio
    * by 100 to return the appropriate value.
    */
   *puiVsyncPlaybackRate = (stCurrentPlaybackRate.uiNumerator * 100) / stCurrentPlaybackRate.uiDenominator;

   BDBG_LEAVE(BXVD_GetVsyncPlaybackRate_isr);

   return BERR_TRACE( rc );
}

/***************************************************************************
   SW7400-2870: 
   BXVD_SetPlaybackRate
   see comment in bxvd.h
****************************************************************************/

BERR_Code BXVD_SetPlaybackRate(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PlaybackRateSettings stPlaybackRateSettings
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_SetPlaybackRate);

   BKNI_EnterCriticalSection();

   rc = BXVD_SetPlaybackRate_isr( hXvdCh, stPlaybackRateSettings );

   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetPlaybackRate);

   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetPlaybackRate_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PlaybackRateSettings stPlaybackRateSettings
   )
{
   BERR_Code rc=BERR_SUCCESS;
   BXDM_Picture_Rate stXDMPlaybackRate;

   BDBG_ENTER(BXVD_SetPlaybackRate_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }


   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetPlaybackRate_isr(%d/%d)", 
            stPlaybackRateSettings.uiNumerator, 
            stPlaybackRateSettings.uiDenominator));

   /* Prevent a divide by '0'.*/
   if ( 0 == stPlaybackRateSettings.uiDenominator )
   {
      BDBG_WRN(("BXVD_SetPlaybackRate_isr90 uiDenominator == 0!"));
      return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   /* XDM uses a single variable to store the vsync playback rate.  This value is set 
    * both by this API and BXVD_PVR_EnablePause; the pause state has precedence.  
    * As a result, if the system is paused when this API is called, the new playback 
    * rate will be saved but not applied until the system is un-paused.
    */
   BKNI_Memset(&stXDMPlaybackRate, 0, sizeof( BXDM_Picture_Rate ) );

   stXDMPlaybackRate.uiNumerator    = stPlaybackRateSettings.uiNumerator;
   stXDMPlaybackRate.uiDenominator  = stPlaybackRateSettings.uiDenominator;

   /* Only apply the new playback rate if the system in NOT paused. */
   if ( false == hXvdCh->bPauseActive )
   {
      rc = BXDM_PictureProvider_SetPlaybackRate_isr(
                  hXvdCh->hPictureProvider,
                  &stXDMPlaybackRate
                  );
   }       

   hXvdCh->stSavedPlaybackRate = stXDMPlaybackRate;
   hXvdCh->bSavedPlaybackRateValid = true;

   BDBG_LEAVE(BXVD_SetPlaybackRate_isr);

   return BERR_TRACE( rc );
}

/***************************************************************************
   BXVD_GetPlaybackRate
   see comment in bxvd.h
****************************************************************************/

BERR_Code BXVD_GetPlaybackRate(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PlaybackRateSettings * pstPlaybackRateSettings
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_GetPlaybackRate);

   BKNI_EnterCriticalSection();

   rc = BXVD_GetPlaybackRate_isr( hXvdCh, pstPlaybackRateSettings );

   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetPlaybackRate);

   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetPlaybackRate_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PlaybackRateSettings * pstPlaybackRateSettings
   )
{
   BERR_Code rc=BERR_SUCCESS;
   BXDM_Picture_Rate stCurrentPlaybackRate;
   BDBG_ENTER(BXVD_GetPlaybackRate_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pstPlaybackRateSettings);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   if ( false == hXvdCh->bPauseActive )
   {
      rc = BXDM_PictureProvider_GetPlaybackRate_isr(
                  hXvdCh->hPictureProvider,
                  &stCurrentPlaybackRate
                  );

      if ( BERR_SUCCESS != rc )
      {
         pstPlaybackRateSettings->uiNumerator = 1;
         pstPlaybackRateSettings->uiDenominator = 1;
         BXVD_DBG_WRN(hXvdCh, ("BXVD_GetPlaybackRate_isr() BXDM_PictureProvider_GetPlaybackRate_isr returned %d", rc ));
         return BERR_TRACE( rc );
      }
   }
   else
   {
      if ( true == hXvdCh->bSavedPlaybackRateValid )
      {
         stCurrentPlaybackRate = hXvdCh->stSavedPlaybackRate;
      }
      else
      {
         /* It should be impossible to hit this case.  If "bPauseActive" is true,
          * there should always be a saved playback rate.  Add this warning in the
          * event a logic bug creeps in.
          */
         stCurrentPlaybackRate.uiNumerator = 1;
         stCurrentPlaybackRate.uiDenominator = 1;
         BXVD_DBG_WRN(hXvdCh, ("BXVD_GetPlaybackRate_isr() bPauseActive but no saved playback rate."));
      }
   }
   
   pstPlaybackRateSettings->uiNumerator = stCurrentPlaybackRate.uiNumerator;
   pstPlaybackRateSettings->uiDenominator = stCurrentPlaybackRate.uiDenominator;

   BDBG_LEAVE(BXVD_GetPlaybackRate_isr);

   return BERR_TRACE( rc );
}


/***************************************************************************
   BXVD_GetPPBParameterInfo
****************************************************************************/

BERR_Code BXVD_GetPPBParameterInfo(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PPBParameterInfo *pPPBParameterInfo
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_GetPPBParameterInfo);

   BKNI_EnterCriticalSection();
   rc = BXVD_GetPPBParameterInfo_isr( hXvdCh, pPPBParameterInfo );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetPPBParameterInfo);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetPPBParameterInfo_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_PPBParameterInfo *pPPBParameterInfo
   )
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetPPBParameterInfo_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pPPBParameterInfo);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetCurrentPicture_isr(
            hXvdCh->hPictureProvider,
            pPPBParameterInfo
            );

   BDBG_LEAVE(BXVD_GetPPBParameterInfo_isr);
   return BERR_TRACE( rc );
}

BERR_Code BXVD_GetPPBParameterQueueInfo(
   BXVD_ChannelHandle hXvdCh,
   const BXVD_PPBParameterInfo* apstPPBParameterInfo[],
   uint32_t uiPPBParameterInfoCount,
   uint32_t *puiValidPPBParameterInfoCount
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_GetPPBParameterQueueInfo);

   BKNI_EnterCriticalSection();
   rc = BXVD_GetPPBParameterQueueInfo_isr( hXvdCh,
                                           apstPPBParameterInfo,
                                           uiPPBParameterInfoCount,
                                           puiValidPPBParameterInfoCount );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetPPBParameterQueueInfo);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetPPBParameterQueueInfo_isr(
   BXVD_ChannelHandle hXvdCh,
   const BXVD_PPBParameterInfo* apstPPBParameterInfo[],
   uint32_t uiPPBParameterInfoCount,
   uint32_t *puiValidPPBParameterInfoCount
   )
{
   BERR_Code rc = BERR_UNKNOWN;

   BDBG_ENTER(BXVD_GetPPBParameterQueueInfo_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(apstPPBParameterInfo);
   BDBG_ASSERT(puiValidPPBParameterInfoCount);
   BDBG_ASSERT(uiPPBParameterInfoCount > 0);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXVD_Decoder_GetPPBParameterQueueInfo_isr(
            hXvdCh,
            apstPPBParameterInfo,
            uiPPBParameterInfoCount,
            puiValidPPBParameterInfoCount);

   BDBG_LEAVE(BXVD_GetPPBParameterQueueInfo_isr);

   return BERR_TRACE(rc);
}

/***************************************************************************
    BXVD_SetErrorHandlingMode: Used to set the error handling mode.
****************************************************************************/
BERR_Code BXVD_SetErrorHandlingMode
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Picture_ErrorHandling eErrorMode
)
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_SetErrorHandlingMode);

   BKNI_EnterCriticalSection();
   rc = BXVD_SetErrorHandlingMode_isr(
      hXvdCh,
      eErrorMode
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetErrorHandlingMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetErrorHandlingMode_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Picture_ErrorHandling eErrorMode
)
{
   BERR_Code rc;
   BXDM_PictureProvider_ErrorHandlingMode eXDMErrorHandlingMode;

   BDBG_ENTER(BXVD_SetErrorHandlingMode_isr);
   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   switch ( eErrorMode )
   {
      case BXVD_ErrorHandling_eOff:
         eXDMErrorHandlingMode = BXDM_PictureProvider_ErrorHandlingMode_eOff;
         break;

      case BXVD_ErrorHandling_ePicture:
         eXDMErrorHandlingMode = BXDM_PictureProvider_ErrorHandlingMode_ePicture;
         break;

      case BXVD_ErrorHandling_ePrognostic:
         eXDMErrorHandlingMode = BXDM_PictureProvider_ErrorHandlingMode_ePrognostic;
         break;

      default:
         BXVD_DBG_ERR(hXvdCh, ("Invalid Error Handling Mode: 0x%x (%d)",
                  eErrorMode, eErrorMode));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
   }

   rc = BXDM_PictureProvider_SetErrorHandlingMode_isr(
            hXvdCh->hPictureProvider,
            eXDMErrorHandlingMode
            );

   hXvdCh->sDecodeSettings.eErrorHandling = eErrorMode;
   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetErrorHandlingMode(%d)", eErrorMode));

   BDBG_LEAVE(BXVD_SetErrorHandlingMode_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
    BXVD_GetErrorHandlingMode: Used to get the error handling mode.
****************************************************************************/
BERR_Code BXVD_GetErrorHandlingMode
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Picture_ErrorHandling * peErrorMode
)
{
   BERR_Code rc;

   BDBG_ENTER(BXVD_GetErrorHandlingMode);

   BKNI_EnterCriticalSection();
   rc = BXVD_GetErrorHandlingMode_isr(
      hXvdCh,
      peErrorMode
      );
   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetErrorHandlingMode);
   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetErrorHandlingMode_isr
(
   BXVD_ChannelHandle hXvdCh,
   BXVD_Picture_ErrorHandling * peErrorMode
)
{
   BERR_Code rc;
   BXDM_PictureProvider_ErrorHandlingMode eXDMErrorHandlingMode;

   BDBG_ENTER(BXVD_GetErrorHandlingMode_isr);
   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(peErrorMode);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetErrorHandlingMode_isr(
            hXvdCh->hPictureProvider,
            &eXDMErrorHandlingMode
            );

   if ( BERR_SUCCESS == rc )
   {
      switch ( eXDMErrorHandlingMode )
      {
         case BXDM_PictureProvider_ErrorHandlingMode_eOff:
            *peErrorMode = BXVD_ErrorHandling_eOff;
            break;

         case BXDM_PictureProvider_ErrorHandlingMode_ePicture:
            *peErrorMode = BXVD_ErrorHandling_ePicture;
            break;

         case BXDM_PictureProvider_ErrorHandlingMode_ePrognostic:
            *peErrorMode = BXVD_ErrorHandling_ePrognostic;
            break;

         default:
            BXVD_DBG_ERR(hXvdCh, ("Unknown XDM Error Handling Mode: 0x%x (%d)",
                     eXDMErrorHandlingMode, eXDMErrorHandlingMode));

            return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
   }

   BXVD_DBG_MSG(hXvdCh, ("BXVD_GetErrorHandlingMode(%d)", hXvdCh->sDecodeSettings.eErrorHandling ));

   BDBG_LEAVE(BXVD_GetErrorHandlingMode_isr);
   return BERR_TRACE(rc);
}

/***************************************************************************
    BXVD_[Set/Get]JitterToleranceImprovementEnable: Used to enable/disable
    Jitter Tolerance Improvement logic in XVD DM
****************************************************************************/

BERR_Code BXVD_SetJitterToleranceImprovementEnable(
   BXVD_ChannelHandle hXvdCh,
   bool bEnable
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_SetJitterToleranceImprovementEnable );

   BKNI_EnterCriticalSection();
   rc = BXVD_SetJitterToleranceImprovementEnable_isr(
      hXvdCh,
      bEnable
      );
   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_SetJitterToleranceImprovementEnable );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_SetJitterToleranceImprovementEnable_isr(
   BXVD_ChannelHandle hXvdCh,
   bool bEnable
   )
{
   BERR_Code rc;
   BDBG_ENTER( BXVD_SetJitterToleranceImprovementEnable_isr );
   
   BDBG_ASSERT( hXvdCh );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_SetJitterToleranceImprovementMode_isr(
            hXvdCh->hPictureProvider,
            bEnable
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetJitterToleranceImprovementEnable(%d)", bEnable));

   BDBG_LEAVE( BXVD_SetJitterToleranceImprovementEnable_isr );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_GetJitterToleranceImprovementEnable(
   BXVD_ChannelHandle hXvdCh,
   bool *pbEnable
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_GetJitterToleranceImprovementEnable );

   BKNI_EnterCriticalSection();
   rc = BXVD_GetJitterToleranceImprovementEnable_isr(
      hXvdCh,
      pbEnable
      );
   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_GetJitterToleranceImprovementEnable );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_GetJitterToleranceImprovementEnable_isr(
   BXVD_ChannelHandle hXvdCh,
   bool *pbEnable
   )
{
   BERR_Code rc;
   BDBG_ENTER( BXVD_GetJitterToleranceImprovementEnable_isr );
   
   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pbEnable );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_GetJitterToleranceImprovementMode_isr(
            hXvdCh->hPictureProvider,
            pbEnable
            );

   BDBG_LEAVE( BXVD_GetJitterToleranceImprovementEnable_isr );

   return BERR_TRACE( rc );
}

/***************************************************************************
    BXVD_[Set/Get]MPEGPulldownOverride:
    Set the pulldown override mode for MPEG content.
****************************************************************************/

BERR_Code BXVD_SetMPEGPulldownOverride(
   BXVD_ChannelHandle hXvdCh,
   BXVD_MPEGPulldownOverride ePulldownOverride
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_SetMPEGPulldownOverride );

   BKNI_EnterCriticalSection();

   rc = BXVD_SetMPEGPulldownOverride_isr( hXvdCh, ePulldownOverride );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_SetMPEGPulldownOverride );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_SetMPEGPulldownOverride_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_MPEGPulldownOverride ePulldownOverride
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_SetMPEGPulldownOverride_isr );
   
   BDBG_ASSERT( hXvdCh );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   hXvdCh->stDecoderContext.ePulldownOverride = ePulldownOverride;

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetMPEGPulldownOverride(%d)", ePulldownOverride));

   BDBG_LEAVE( BXVD_SetMPEGPulldownOverride_isr );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_GetMPEGPulldownOverride(
   BXVD_ChannelHandle hXvdCh,
   BXVD_MPEGPulldownOverride * pePulldownOverride
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_GetMPEGPulldownOverride );

   BKNI_EnterCriticalSection();

   rc = BXVD_GetMPEGPulldownOverride_isr( hXvdCh, pePulldownOverride );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_GetMPEGPulldownOverride );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_GetMPEGPulldownOverride_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_MPEGPulldownOverride * pePulldownOverride
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_GetMPEGPulldownOverride_isr );
   
   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pePulldownOverride );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   *pePulldownOverride = hXvdCh->stDecoderContext.ePulldownOverride;

   BDBG_LEAVE( BXVD_GetMPEGPulldownOverride_isr );

   return BERR_TRACE( rc );
}

/***************************************************************************

   SW7405-4117: deinterlacer max height is used in conjuction with 
   BXVD_DisplayFieldType_eAuto to choose either eSingleField or eBothField 
   based on the steam height during slow motion (and preroll).

****************************************************************************/

BERR_Code BXVD_SetDeinterlacerMaxHeight(
   BXVD_ChannelHandle hXvdCh,
   uint32_t uiMaxHeight
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_SetDeinterlacerMaxHeight );

   BKNI_EnterCriticalSection();

   rc = BXVD_SetDeinterlacerMaxHeight_isr( hXvdCh, uiMaxHeight );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_SetDeinterlacerMaxHeight );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_SetDeinterlacerMaxHeight_isr(
   BXVD_ChannelHandle hXvdCh,
   uint32_t uiMaxHeight
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_SetDeinterlacerMaxHeight_isr );
   
   BDBG_ASSERT( hXvdCh );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_SetDeinterlacerMaxHeight(
            hXvdCh->hPictureProvider,
            uiMaxHeight
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetDeinterlacerMaxHeight(%d)", uiMaxHeight));

   BDBG_LEAVE( BXVD_SetDeinterlacerMaxHeight_isr );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_GetDeinterlacerMaxHeight(
   BXVD_ChannelHandle hXvdCh,
   uint32_t * puiMaxHeight
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_GetDeinterlacerMaxHeight );

   BKNI_EnterCriticalSection();

   rc = BXVD_GetDeinterlacerMaxHeight_isr( hXvdCh, puiMaxHeight );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_GetDeinterlacerMaxHeight );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_GetDeinterlacerMaxHeight_isr(
   BXVD_ChannelHandle hXvdCh,
   uint32_t * puiMaxHeight
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_GetDeinterlacerMaxHeight_isr );
   
   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( puiMaxHeight );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_GetDeinterlacerMaxHeight(
            hXvdCh->hPictureProvider,
            puiMaxHeight
            );

   BDBG_LEAVE( BXVD_GetDeinterlacerMaxHeight_isr );

   return BERR_TRACE( rc );
}

/***************************************************************************
   SWDEPRECATED-1003:
   BXVD_SetFrameRateOverride
   see comment in bxvd.h
****************************************************************************/

BERR_Code BXVD_SetFrameRateOverride(
   BXVD_ChannelHandle hXvdCh,
   BXVD_FrameRateOverride * pstFrameRateOverrideSettings
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_SetFrameRateOverride);

   BKNI_EnterCriticalSection();

   rc = BXVD_SetFrameRateOverride_isr( hXvdCh, pstFrameRateOverrideSettings );

   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_SetFrameRateOverride);

   return BERR_TRACE(rc);
}

BERR_Code BXVD_SetFrameRateOverride_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_FrameRateOverride * pstFrameRateOverrideSettings
   )
{
   BERR_Code rc=BERR_SUCCESS;
   BXDM_Picture_FrameRateOverride  stFrameRateOverride;

   BDBG_ENTER(BXVD_SetFrameRateOverride_isr);

   BDBG_ASSERT(hXvdCh);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetFrameRateOverride_isr() bValid:%d rate: %d/%d", 
            pstFrameRateOverrideSettings->bValid, 
            pstFrameRateOverrideSettings->stRate.uiNumerator,
            pstFrameRateOverrideSettings->stRate.uiDenominator
            ));

   BKNI_Memset( &stFrameRateOverride, 0, sizeof( BXDM_Picture_FrameRateOverride ));

   if ( true == pstFrameRateOverrideSettings->bValid )
   {
      if ( 0 == pstFrameRateOverrideSettings->stRate.uiNumerator
            || 0 == pstFrameRateOverrideSettings->stRate.uiDenominator
         )
      {
         /* Set the override to invalid if the specified rate is unknown.  */
         stFrameRateOverride.bValid = false;
         stFrameRateOverride.stRate.uiNumerator = 1;
         stFrameRateOverride.stRate.uiDenominator = 1;

         BDBG_ERR(("BXVD_SetFrameRateOverride_isr() invalid rate: %d/%d",
                        pstFrameRateOverrideSettings->stRate.uiNumerator,
                        pstFrameRateOverrideSettings->stRate.uiDenominator));
         return BERR_TRACE(BERR_INVALID_PARAMETER);
      }
      else
      {
         stFrameRateOverride.bValid = true;
         stFrameRateOverride.stRate.uiNumerator = pstFrameRateOverrideSettings->stRate.uiNumerator;
         stFrameRateOverride.stRate.uiDenominator = pstFrameRateOverrideSettings->stRate.uiDenominator;
         stFrameRateOverride.bTreatAsSingleElement = pstFrameRateOverrideSettings->bTreatAsSingleElement;
      }
   }
   else
   {
      stFrameRateOverride.bValid = false;
      stFrameRateOverride.stRate.uiNumerator = 1;
      stFrameRateOverride.stRate.uiDenominator = 1;
   }

   rc = BXDM_PictureProvider_SetFrameRateOverride_isr(
                  hXvdCh->hPictureProvider,
                  &stFrameRateOverride
                  );

   BDBG_LEAVE(BXVD_SetFrameRateOverride_isr);

   return BERR_TRACE( rc );
}

/***************************************************************************
   BXVD_GetFrameRateOverride
   see comment in bxvd.h
****************************************************************************/

BERR_Code BXVD_GetFrameRateOverride(
   BXVD_ChannelHandle hXvdCh,
   BXVD_FrameRateOverride * pstFrameRateOverrideSettings
   )
{
   BERR_Code rc;
   BDBG_ENTER(BXVD_GetFrameRateOverride);

   BKNI_EnterCriticalSection();

   rc = BXVD_GetFrameRateOverride_isr( hXvdCh, pstFrameRateOverrideSettings );

   BKNI_LeaveCriticalSection();

   BDBG_LEAVE(BXVD_GetFrameRateOverride);

   return BERR_TRACE(rc);
}

BERR_Code BXVD_GetFrameRateOverride_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_FrameRateOverride * pstFrameRateOverrideSettings
   )
{
   BERR_Code rc=BERR_SUCCESS;
   BXDM_Picture_FrameRateOverride stCurrentFrameRateOverride;
   BDBG_ENTER(BXVD_GetFrameRateOverride_isr);

   BDBG_ASSERT(hXvdCh);
   BDBG_ASSERT(pstFrameRateOverrideSettings);

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }

   rc = BXDM_PictureProvider_GetFrameRateOverride_isr(
                  hXvdCh->hPictureProvider,
                  &stCurrentFrameRateOverride
                  );

   pstFrameRateOverrideSettings->bValid               = stCurrentFrameRateOverride.bValid;
   pstFrameRateOverrideSettings->stRate.uiNumerator   = stCurrentFrameRateOverride.stRate.uiNumerator;
   pstFrameRateOverrideSettings->stRate.uiDenominator = stCurrentFrameRateOverride.stRate.uiDenominator;
   pstFrameRateOverrideSettings->bTreatAsSingleElement = stCurrentFrameRateOverride.bTreatAsSingleElement;

   BDBG_LEAVE(BXVD_GetFrameRateOverride_isr);

   return BERR_TRACE( rc );
}

/***************************************************************************
   SW7422-72: API's for specifying the 3D orientation of pictures
   see comment in bxvd.h
****************************************************************************/

BERR_Code BXVD_Set3D(
   BXVD_ChannelHandle hXvdCh,
   const BXVD_3DSetting * pst3DSettings
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_Set3D );

   BKNI_EnterCriticalSection();

   rc = BXVD_Set3D_isr( hXvdCh, pst3DSettings );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_Set3D );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_Set3D_isr(
   BXVD_ChannelHandle hXvdCh,
   const BXVD_3DSetting * pst3DSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_Set3D_isr );
   
   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pst3DSettings );


   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_Set3D_isr(
            hXvdCh->hPictureProvider,
            pst3DSettings
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_Set3D(%d)", pst3DSettings));

   BDBG_LEAVE( BXVD_Set3D_isr );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_Get3D(
   BXVD_ChannelHandle hXvdCh,
   BXVD_3DSetting * pst3DSettings
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_Get3D );

   BKNI_EnterCriticalSection();

   rc = BXVD_Get3D_isr( hXvdCh, pst3DSettings );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_Get3D );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_Get3D_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_3DSetting * pst3DSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_Get3D_isr );
   
   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pst3DSettings );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_Get3D_isr(
            hXvdCh->hPictureProvider,
            pst3DSettings
            );

   BDBG_LEAVE( BXVD_Get3D_isr );

   return BERR_TRACE( rc );
}


BERR_Code BXVD_GetDefault3D(
   BXVD_ChannelHandle hXvdCh,
   BXVD_3DSetting * pst3DSettings
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_GetDefault3D );
   
   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pst3DSettings );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_GetDefault3D(
            hXvdCh->hPictureProvider,
            pst3DSettings
            );

#if BXVD_LEGACY_MVC_SUPPORT
      /* SW7422-72: the original MVC behavior on the DVD chips was to set "pNext" in the 
       * MFD structure of frame 0 to point to frame 1.  When running on these older chip,
       * "bSetNextPointer" will convey to XDM that both "pEhanced" and "pNext" should be set.
       */
      pst3DSettings->bSetNextPointer = true;
#endif


   BDBG_LEAVE( BXVD_GetDefault3D );

   return BERR_TRACE( rc );
}

/***************************************************************************
   SW7425-1264: support for a synthesized STC; can create a clock that 
   run backwards.
   see comment in bxvd.h
****************************************************************************/

BERR_Code BXVD_SetClockOverride(
   BXVD_ChannelHandle hXvdCh,
   const BXVD_ClockOverride * pstClockOverride
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_SetClockOverride );

   BKNI_EnterCriticalSection();

   rc = BXVD_SetClockOverride_isr( hXvdCh, pstClockOverride );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_SetClockOverride );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_SetClockOverride_isr(
   BXVD_ChannelHandle hXvdCh,
   const BXVD_ClockOverride * pstClockOverride
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_SetClockOverride_isr );
   
   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pstClockOverride );


   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_SetClockOverride_isr(
            hXvdCh->hPictureProvider,
            pstClockOverride
            );

   BXVD_DBG_MSG(hXvdCh, ("BXVD_SetClockOverride:: Enable:%d  initialValue:%d load:%d stcDelta:%d)",
                  pstClockOverride->bEnableClockOverride,
                  pstClockOverride->uiStcValue,
                  pstClockOverride->bLoadSwStc,
                  pstClockOverride->iStcDelta
                  ));


   BDBG_LEAVE( BXVD_SetClockOverride_isr );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_GetClockOverride(
   BXVD_ChannelHandle hXvdCh,
   BXVD_ClockOverride * pstClockOverride
   )
{
   BERR_Code rc;

   BDBG_ENTER( BXVD_GetClockOverride );

   BKNI_EnterCriticalSection();

   rc = BXVD_GetClockOverride_isr( hXvdCh, pstClockOverride );

   BKNI_LeaveCriticalSection();
   
   BDBG_LEAVE( BXVD_GetClockOverride );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_GetClockOverride_isr(
   BXVD_ChannelHandle hXvdCh,
   BXVD_ClockOverride * pstClockOverride
   )
{
   BERR_Code rc = BERR_SUCCESS;
   BDBG_ENTER( BXVD_GetClockOverride_isr );
   
   BDBG_ASSERT( hXvdCh );
   BDBG_ASSERT( pstClockOverride );

   /* Check handle type for correctness */
   if (hXvdCh->eHandleType != BXVD_P_HandleType_XvdChannel)
   {
      BDBG_ERR(("Invalid handle type passed to function"));
      return BERR_TRACE(BXVD_ERR_INVALID_HANDLE);
   }
   
   rc = BXDM_PictureProvider_GetClockOverride_isr(
            hXvdCh->hPictureProvider,
            pstClockOverride
            );

   BDBG_LEAVE( BXVD_GetClockOverride_isr );

   return BERR_TRACE( rc );
}

/***************************************************************************
   SW7425-1064: support for linking channels
   see comment in bxvd.h
****************************************************************************/

static BERR_Code BXVD_S_InstallXmo(
   BXVD_ChannelHandle hXvdChBase
   )
{
   BERR_Code rc=BERR_SUCCESS;
   BXVD_ChannelHandle hXvdChEnhanced;
   bool bInstallXmo;

   BDBG_ENTER( BXVD_S_InstallXmo );

   BDBG_ASSERT( hXvdChBase );

   /* Verify that the second channel is linked to the first. */
   hXvdChEnhanced = hXvdChBase->hXvdChLinked;

   if ( NULL == hXvdChEnhanced )
   {
      BXVD_DBG_ERR( hXvdChBase, ("BXVD_S_InstallXmo:: handle for the enhanced channel is NULL"));
      rc = BERR_INVALID_PARAMETER;
      goto Done;
   }

   /* If the XMO has been allocated and both channels are of the 
    * correct type, link them to the XMO. 
    */
   bInstallXmo = ( NULL != hXvdChBase->hXmo );
   bInstallXmo &= ( BXVD_P_ChannelType_eBase == hXvdChBase->eChannelType );
   bInstallXmo &= ( BXVD_P_ChannelType_eEnhanced == hXvdChEnhanced->eChannelType );

   if ( true == bInstallXmo )
   {
      BXDM_Decoder_Interface stDecoderInterface;
      void *pPrivateContext;

      /* Get the base decoder interface. */
      BXVD_Decoder_GetDMInterface( 
               hXvdChBase, 
               &stDecoderInterface, 
               &pPrivateContext 
               );

      /* Register the base decoder interface with XMO. */
      BXDM_PictureProvider_XMO_SetDecoderInterface( 
               hXvdChBase->hXmo, 
               BXDM_PP_XMO_Base_Index, 
               &stDecoderInterface,
               pPrivateContext 
               );

      /* Get the enhanced decoder interface. */
      BXVD_Decoder_GetDMInterface( 
               hXvdChEnhanced, 
               &stDecoderInterface, 
               &pPrivateContext 
               );

      /* Register the enhanced decoder interface with XMO. */
      BXDM_PictureProvider_XMO_SetDecoderInterface( 
               hXvdChBase->hXmo, 
               BXDM_PP_XMO_Enhanced_Index, 
               &stDecoderInterface,
               pPrivateContext 
               );

      /* Get the XMO decoder interface. */
      BXDM_PictureProvider_XMO_GetDMInterface( 
               hXvdChBase->hXmo, 
               &stDecoderInterface, 
               &pPrivateContext 
               );

      /* Bind XMO to XDM (the Picture Provider) 
       * TODO: does BXDM_PictureProvider_SetDecoderInterface_isr
       * need to be an ISR call?
       */
      BKNI_EnterCriticalSection();

      BXDM_PictureProvider_SetDecoderInterface_isr( 
               hXvdChBase->hPictureProvider, 
               &stDecoderInterface, 
               pPrivateContext 
               );

      BKNI_LeaveCriticalSection();
   

   }     /* end of if ( true == bInstallXmo ) */
   else
   {
      BXVD_DBG_ERR( hXvdChBase, ("BXVD_S_InstallXmo:: install failed: hXmo: %08x  base type:%d  enhanced type:%d",
                                     hXvdChBase->hXmo,
                                     hXvdChBase->eChannelType,
                                     hXvdChEnhanced->eChannelType
                                     ));
      rc = BERR_INVALID_PARAMETER;
   }

Done:

   BDBG_LEAVE( BXVD_S_InstallXmo );

   return BERR_TRACE( rc );
}


static BERR_Code BXVD_S_UninstallXmo(
   BXVD_ChannelHandle hXvdChBase
   )
{
   BERR_Code rc=BERR_SUCCESS;
   BXVD_ChannelHandle hXvdChEnhanced;
   bool bInstallXmo;

   BDBG_ENTER( BXVD_S_UninstallXmo );

   BDBG_ASSERT( hXvdChBase );

   /* Verify that the second channel is linked to the first. */
   hXvdChEnhanced = hXvdChBase->hXvdChLinked;

   if ( NULL == hXvdChEnhanced )
   {
      BXVD_DBG_ERR( hXvdChBase, ("BXVD_S_UninstallXmo:: handle for the enhanced channel is NULL"));
      rc = BERR_INVALID_PARAMETER;
      goto Done;
   }

   /* If the XMO has been allocated and both channels are of the 
    * correct type, link them to the XMO. 
    */
   bInstallXmo = ( NULL != hXvdChBase->hXmo );
   bInstallXmo &= ( BXVD_P_ChannelType_eBase == hXvdChBase->eChannelType );
   bInstallXmo &= ( BXVD_P_ChannelType_eEnhanced == hXvdChEnhanced->eChannelType );

   if ( true == bInstallXmo )
   {
      BXDM_Decoder_Interface stDecoderInterface;
      void *pPrivateContext;

      /* Get the base decoder interface. */
      BXVD_Decoder_GetDMInterface( 
               hXvdChBase, 
               &stDecoderInterface, 
               &pPrivateContext 
               );

      /* Bind XMO to XDM (the Picture Provider) 
       * TODO: does BXDM_PictureProvider_SetDecoderInterface_isr
       * need to be an ISR call?
       */
      BKNI_EnterCriticalSection();

      BXDM_PictureProvider_SetDecoderInterface_isr( 
               hXvdChBase->hPictureProvider, 
               &stDecoderInterface, 
               pPrivateContext 
               );

      BKNI_LeaveCriticalSection();
   

   }     /* end of if ( true == bInstallXmo ) */
   else
   {
      BXVD_DBG_ERR( hXvdChBase, ("BXVD_S_UninstallXmo:: install failed: hXmo: %08x  base type:%d  enhanced type:%d",
                                     hXvdChBase->hXmo,
                                     hXvdChBase->eChannelType,
                                     hXvdChEnhanced->eChannelType
                                     ));
      rc = BERR_INVALID_PARAMETER;
   }

Done:

   BDBG_LEAVE( BXVD_S_UninstallXmo );

   return BERR_TRACE( rc );
}

/***************************************************************************
Summary:


Description:
***************************************************************************/

BERR_Code BXVD_LinkChannels(
   BXVD_ChannelHandle hXvdChBase,
   BXVD_ChannelHandle hXvdChEnhanced
   )
{
   BERR_Code rc=BERR_SUCCESS;

   BDBG_ENTER( BXVD_LinkChannels );

   BDBG_ASSERT(hXvdChBase);
   BDBG_ASSERT(hXvdChEnhanced);

   /* Check base handle type for correctness */
   if ( hXvdChBase->eHandleType != BXVD_P_HandleType_XvdChannel )
   {
      BXVD_DBG_ERR( hXvdChBase, ("BXVD_LinkChannels:: Invalid base handle type"));
      rc = BXVD_ERR_INVALID_HANDLE;
      goto Done;
   }

   /* Check enhanced handle type for correctness */
   if ( hXvdChEnhanced->eHandleType != BXVD_P_HandleType_XvdChannel )
   {
      BXVD_DBG_ERR( hXvdChEnhanced, ("BXVD_LinkChannels:: Invalid enhanced handle type"));
      rc = BXVD_ERR_INVALID_HANDLE;
      goto Done;
   }

   /* Linking/unlinking can only occur when the deocders are stopped. */
   if ( hXvdChBase->eDecoderState == BXVD_P_DecoderState_eActive )
   {
      BXVD_DBG_ERR(hXvdChBase, ("BXVD_LinkChannels:: base decoder is active, it must be stopped."));
      rc = BXVD_ERR_DECODER_ACTIVE;
      goto Done;
   }

   if ( hXvdChEnhanced->eDecoderState == BXVD_P_DecoderState_eActive )
   {
      BXVD_DBG_ERR(hXvdChEnhanced, ("BXVD_LinkChannels:: enhanced decoder is active, it must be stopped."));
      rc = BXVD_ERR_DECODER_ACTIVE;
      goto Done;
   }

   /* The streams being decoded on separate decoders, this is not currently supported. */
   if ( hXvdChBase->pXvd != hXvdChEnhanced->pXvd )
   {
      BXVD_DBG_ERR( hXvdChBase, ("BXVD_LinkChannels:: linking channels on separate decoders is not supported."));
      rc = BERR_INVALID_PARAMETER;
      goto Done;
   }

   /* Link each channel to the other and set the channel type. */

   hXvdChBase->hXvdChLinked   = hXvdChEnhanced;
   hXvdChBase->eChannelType   = BXVD_P_ChannelType_eBase;

   hXvdChEnhanced->hXvdChLinked  = hXvdChBase;
   hXvdChEnhanced->eChannelType  = BXVD_P_ChannelType_eEnhanced;


   /* Create the XMO filter. */
   rc = BXDM_PictureProvider_XMO_Create( &(hXvdChBase->hXmo) );

   if ( BERR_SUCCESS != rc )
   {
      BXVD_DBG_ERR( hXvdChBase, ("BXVD_LinkChannels:: BXDM_PictureProvider_XMO_Create failed"));
      rc = BERR_OUT_OF_SYSTEM_MEMORY;
      goto Done;
   }

   /* Install the XMO filter. */
   rc = BXVD_S_InstallXmo( hXvdChBase );

   if ( BERR_SUCCESS != rc )
   {
      BXVD_DBG_ERR( hXvdChBase, ("BXVD_LinkChannels:: BXVD_S_InstallXmo failed"));
      rc = BERR_INVALID_PARAMETER;
   }

Done:

   BDBG_LEAVE( BXVD_LinkChannels );

   return  BERR_TRACE(rc);

}


/***************************************************************************
Summary:

Description:
***************************************************************************/
BERR_Code BXVD_UnlinkChannels(
   BXVD_ChannelHandle hXvdChBase,
   BXVD_ChannelHandle hXvdChEnhanced
   )
{
   BERR_Code rc=BERR_SUCCESS;

   BDBG_ENTER( BXVD_UnlinkChannels );

   BDBG_ASSERT(hXvdChBase);
   BDBG_ASSERT(hXvdChEnhanced);

   /* Check base handle type for correctness */
   if ( hXvdChBase->eHandleType != BXVD_P_HandleType_XvdChannel )
   {
      BXVD_DBG_ERR( hXvdChBase, ("BXVD_UnlinkChannels:: Invalid base handle type"));
      rc = BXVD_ERR_INVALID_HANDLE;
      goto Done;
   }

   /* Check enhanced handle type for correctness */
   if ( hXvdChEnhanced->eHandleType != BXVD_P_HandleType_XvdChannel )
   {
      BXVD_DBG_ERR( hXvdChEnhanced, ("BXVD_UnlinkChannels:: Invalid enhanced handle type"));
      rc = BXVD_ERR_INVALID_HANDLE;
      goto Done;
   }

   /* Linking/unlinking can only occur when the deocders are stopped. */
   if ( hXvdChBase->eDecoderState == BXVD_P_DecoderState_eActive )
   {
      BXVD_DBG_ERR(hXvdChBase, ("BXVD_UnlinkChannels:: base decoder is active, it must be stopped."));
      rc = BXVD_ERR_DECODER_ACTIVE;
      goto Done;
   }

   if ( hXvdChEnhanced->eDecoderState == BXVD_P_DecoderState_eActive )
   {
      BXVD_DBG_ERR(hXvdChEnhanced, ("BXVD_UnlinkChannels:: enhanced decoder is active, it must be stopped."));
      rc = BXVD_ERR_DECODER_ACTIVE;
      goto Done;
   }

   /* The streams being decoded on separate decoders, this is not currently supported. */
   if ( hXvdChBase->pXvd != hXvdChEnhanced->pXvd )
   {
      BXVD_DBG_ERR( hXvdChBase, ("BXVD_UnlinkChannels:: linking channels on separate decoders is not supported."));
      rc = BERR_INVALID_PARAMETER;
      goto Done;
   }

   /* Uninstall the filter. */
   BXVD_S_UninstallXmo( hXvdChBase );

   /* Free the memory and reset the channel state. */

   rc = BXDM_PictureProvider_XMO_Destroy( hXvdChBase->hXmo );
   hXvdChBase->hXmo = NULL;

   hXvdChBase->hXvdChLinked   = NULL;
   hXvdChBase->eChannelType   = BXVD_P_ChannelType_eStandard;

   hXvdChEnhanced->hXvdChLinked  = NULL;
   hXvdChEnhanced->eChannelType  = BXVD_P_ChannelType_eStandard;

Done:

   BDBG_LEAVE( BXVD_UnlinkChannels );

   return  BERR_TRACE(rc);
}

/***************************************************************************
   SW7425-1064: support for managing XMO (XDM Merge Objec)
   see comment in bxvd.h
****************************************************************************/
#if 0
BERR_Code BXVD_CreateXmo(
   BXVD_ChannelHandle hXvdCh
   )
{
   BERR_Code rc=BERR_SUCCESS;

   BDBG_ENTER( BXVD_CreateXmo );

   rc = BXDM_PictureProvider_XMO_Create( &(hXvdCh->hXmo) );

   BDBG_LEAVE( BXVD_CreateXmo );

   return BERR_TRACE( rc );
}

BERR_Code BXVD_DestroyXmo(
   BXVD_ChannelHandle hXvdCh
   )
{
   BERR_Code rc=BERR_SUCCESS;

   BDBG_ENTER( BXVD_DestroyXmo );

   if ( NULL != hXvdCh->hXmo
        && BXVD_P_ChannelType_eBase == hXvdCh->eChannelType
      )
   {
      rc = BXDM_PictureProvider_XMO_Destroy( hXvdCh->hXmo );
      hXvdCh->hXmo = NULL;
   }
   
   BDBG_LEAVE( BXVD_DestroyXmo );

   return BERR_TRACE( rc );
}
#endif

/*******************/
/* Deprecated APIs */
/*******************/

/***************************************************************************
  BXVD_SetTSMPassWindow: Sets the TSM Pass Display Threshold in number of
  decodable units of display decided by the Correct Display Algorithm
****************************************************************************/
BERR_Code BXVD_SetTSMPassWindow (BXVD_ChannelHandle hXvdCh,
                                 uint32_t ui32PassFractNumerator,
                                 uint32_t ui32PassFractDenominator)
{
   BDBG_ENTER(BXVD_SetTSMPassWindow);

   /* Check the arguments */
   BSTD_UNUSED(hXvdCh);
   BSTD_UNUSED(ui32PassFractNumerator);
   BSTD_UNUSED(ui32PassFractDenominator);

   BXVD_DBG_ERR(hXvdCh, ("BXVD_SetTSMPassWindow() is DEPRECATED"));

   BDBG_LEAVE(BXVD_SetTSMPassWindow);
   return BERR_TRACE(BERR_SUCCESS) ;
}

/***************************************************************************
  BXVD_DisplayUntoPTS: Decodes and displays until the specified PTS is reached.
****************************************************************************/
BERR_Code BXVD_DisplayUntoPTS(BXVD_ChannelHandle hXvdCh,
                              bool       bEnable,
                              uint32_t   ui32PTS,
                              uint32_t   ui32ThreshLoopAroundCntr)
{
   BDBG_ENTER(BXVD_DisplayUntoPTS);

   /* Check the arguments */
   BSTD_UNUSED(hXvdCh);
   BSTD_UNUSED(bEnable);
   BSTD_UNUSED(ui32PTS);
   BSTD_UNUSED(ui32ThreshLoopAroundCntr);

   BXVD_DBG_ERR(hXvdCh, ("BXVD_DisplayUntoPTS() is DEPRECATED"));

   BDBG_LEAVE(BXVD_DisplayUntoPTS);
   return BERR_TRACE(BERR_SUCCESS) ;
}

/***************************************************************************
BXVD_PauseUntoPTS: Decodes and displays until the specified PTS is reached.
****************************************************************************/
BERR_Code BXVD_PauseUntoPTS(BXVD_ChannelHandle hXvdCh,
                            bool        bEnable,
                            uint32_t    ui32PTS,
                            uint32_t    ui32ThreshLoopAroundCntr)
{
   BDBG_ENTER(BXVD_PauseUntoPTS);

   BSTD_UNUSED(hXvdCh);
   BSTD_UNUSED(bEnable);
   BSTD_UNUSED(ui32PTS);
   BSTD_UNUSED(ui32ThreshLoopAroundCntr);

   BXVD_DBG_ERR(hXvdCh, ("BXVD_PauseUntoPTS() is DEPRECATED"));

   BDBG_LEAVE(BXVD_PauseUntoPTS);
   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
****************************************************************************/
BERR_Code BXVD_SetPulldownMode(
   BXVD_ChannelHandle hXvdCh,      /* [in] The XVD Channel handle */
   BXVD_PulldownMode ePulldownMode /* [in] Pulldown state */
   )
{
   BDBG_ENTER(BXVD_SetPulldownMode);

   BSTD_UNUSED(hXvdCh);
   BSTD_UNUSED(ePulldownMode);

   BXVD_DBG_ERR(hXvdCh, ("BXVD_SetPulldownMode() is DEPRECATED"));

   BDBG_LEAVE(BXVD_SetPulldownMode);

   return BERR_TRACE(BERR_SUCCESS);
}

/***************************************************************************
****************************************************************************/
BERR_Code BXVD_GetPulldownMode(
   BXVD_ChannelHandle hXvdCh,          /* [in] The XVD Channel handle */
   BXVD_PulldownMode *pePulldownMode   /* [in] Pulldown state */
   )
{
   BDBG_ENTER(BXVD_GetPulldownMode);

   BSTD_UNUSED(hXvdCh);
   BSTD_UNUSED(pePulldownMode);

   BXVD_DBG_ERR(hXvdCh, ("BXVD_GetPulldownMode() is DEPRECATED"));

   return BERR_TRACE(BERR_SUCCESS);
}
