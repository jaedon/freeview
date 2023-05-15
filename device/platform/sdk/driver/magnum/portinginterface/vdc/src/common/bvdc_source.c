/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_source.c $
 * $brcm_Revision: Hydra_Software_Devel/491 $
 * $brcm_Date: 10/25/12 2:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_source.c $
 * 
 * Hydra_Software_Devel/511   7/23/13 2:07p yuxiaz
 * SW7425-4955: Fixed BVN error when input vertical size is odd in mosaic
 * mode.
 * 
 * Hydra_Software_Devel/510   7/16/13 6:17p vanessah
 * SW7425-4963: fix the mute color 3D programming
 * 
 * Hydra_Software_Devel/509   7/11/13 1:45p yuxiaz
 * SW7425-4955: Distorted mosaic windows under certain condition
 * 
 * Hydra_Software_Devel/505   6/4/13 2:14p yuxiaz
 * SW7425-4785: Fixed possible out of boundary for BAVC_MVD_Field array in
 * mosaic mode.
 * 
 * Hydra_Software_Devel/504   5/30/13 11:46a yuxiaz
 * SW7425-4842: Mosaic on HD/SD simul display doesn't work.
 * 
 * Hydra_Software_Devel/503   5/9/13 5:07p yuxiaz
 * SW7425-4785: Clear rect is not shown for all mosaic windows. Optimize
 * mosaic zorder code.
 * 
 * Hydra_Software_Devel/502   5/8/13 11:47a yuxiaz
 * SW7425-4736: Fixed kernel mode crash on 7468 and 7208.
 * 
 * Hydra_Software_Devel/501   5/6/13 4:00p yuxiaz
 * SW7425-4736: Need to copy pNext when first entry in linked list is
 * muted.
 * 
 * Hydra_Software_Devel/500   5/3/13 1:38p yuxiaz
 * SW7425-4736: Need to copy entire BAVC_MFD_Picture linked list for
 * mosaic mode.
 * 
 * Hydra_Software_Devel/491   10/25/12 2:29p pntruong
 * SW7425-4108: MJPEG decode displays garbage.  Need to program the
 * surface address for the callback path (xdm->vdc) as well.
 *
 * Hydra_Software_Devel/490   9/24/12 6:22p syang
 * SW7425-3977: add aspect-ratio handling for 3D video src and display
 *
 * Hydra_Software_Devel/489   9/10/12 11:59a syang
 * SW7425-2172: allow gfx MPEG src to InstallPictureCallback
 *
 * Hydra_Software_Devel/487   9/6/12 2:43p syang
 * SW7425-2172: refine SetMuteMode error check for gfx to MFD/VFD
 *
 * Hydra_Software_Devel/486   8/16/12 2:41p yuxiaz
 * SW7425-3619: Add const to read only settings.
 *
 * Hydra_Software_Devel/485   8/13/12 3:08p yuxiaz
 * SW7425-3626, SW7425-3619: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 *
 * Hydra_Software_Devel/484   8/9/12 11:19a syang
 * SW7425-2172: add SetSurface support for MFD gfx feeding
 *
 * Hydra_Software_Devel/483   8/7/12 12:54p yuxiaz
 * SW7405-3984: bLast and bChannelChange are valid when bMute is set.
 *
 * Hydra_Software_Devel/482   6/26/12 5:24p yuxiaz
 * SW7405-3984: Handle dummy picture from XVD: use previous picture except
 * eInterruptPolarity,
 * bIgnorePicture, and bStallStc fields.
 *
 * Hydra_Software_Devel/481   6/14/12 3:43p syang
 * SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
 * prepare to add SetSurface support to MFD
 *
 * Hydra_Software_Devel/480   5/23/12 6:47p syang
 * SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
 * use gfxsurface sub-module
 *
 * Hydra_Software_Devel/479   5/15/12 3:00p syang
 * SW7425-2093:don't free scratch register until BVDC_Close
 *
 * Hydra_Software_Devel/478   5/11/12 4:04p syang
 * SW7425-2093: free scratch registers when gfx/vfd src handle is
 * destroyed by user
 *
 * Hydra_Software_Devel/477   5/9/12 10:53a syang
 * SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
 * error fix with GetSurface; use shared gfxsurface to manage gfx surface
 *
 * Hydra_Software_Devel/476   4/12/12 4:13p vanessah
 * SW7425-2831: use the source for the field inversion handling in
 * ignorepicture
 *
 * Hydra_Software_Devel/475   3/30/12 5:29p pntruong
 * SW7435-80: Reduced BVDC_Open-time allocations from BMEM based on
 * maximum usage.
 *
 * Hydra_Software_Devel/474   3/8/12 11:51a yuxiaz
 * SW7425-2546: Fixed warning msg.
 *
 * Hydra_Software_Devel/473   3/1/12 1:42p yuxiaz
 * SW7425-2418: Set hHeap for graphics surface.
 *
 * Hydra_Software_Devel/472   3/1/12 11:46a yuxiaz
 * SW7425-2526, SW7425-1182: Added runtime query capabilities for source
 * in VDC.
 *
 * Hydra_Software_Devel/471   2/29/12 2:54p yuxiaz
 * SW7425-2418: Clean up code for mfd feeds from gfx surface
 *
 * Hydra_Software_Devel/470   2/16/12 3:01p vanessah
 * SW7425-2327: add Bar data handling
 *
 * Hydra_Software_Devel/469   2/16/12 1:16p pntruong
 * SW7231-587: Correctly propagate the original pts value to get buffer.
 *
 * Hydra_Software_Devel/468   2/13/12 2:42p yuxiaz
 * SW7425-2181: Fixed MPEG data validate when
 * pChrominanceFrameBufferAddress is NULL.
 *
 * Hydra_Software_Devel/467   2/8/12 1:40p yuxiaz
 * SW7405-5577: loss of video on composite on mosaic teardown: remove
 * unnecessary source slot clean up.
 *
 * Hydra_Software_Devel/466   2/7/12 5:14p yuxiaz
 * SW7425-2181: Add basic VDC support to interface with output from SID
 * for MJPEG usage mode.
 *
 * Hydra_Software_Devel/465   12/8/11 4:41p yuxiaz
 * SW7425-967: Bring up MRE on 7425 B1.
 *
 * Hydra_Software_Devel/464   12/7/11 5:17p pntruong
 * SW7208-175: Allowed more valid resolution supported by decoder and not
 * just resticted to 1080p.
 *
 * Hydra_Software_Devel/463   12/1/11 4:33p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/SW7425-968/1   11/16/11 3:12p yuxiaz
 * SW7425-968: Added left_R calculation in VDC rects.
 *
 * Hydra_Software_Devel/462   10/27/11 5:31p albertl
 * SW7405-4434: Fixed to correctly copy content of pCurDirty to pOldDirty
 * rather than copy pointer values.
 *
 * Hydra_Software_Devel/461   10/26/11 2:56p pntruong
 * SW7400-3008, SW7405-4434: Used correct dirtybit context.
 *
 * Hydra_Software_Devel/460   10/7/11 5:49p vanessah
 * SW7425-1417: add host_arm for STG_1
 *
 * Hydra_Software_Devel/459   9/27/11 5:21p yuxiaz
 * SW7425-967: Bring up MRE on 7425 B0.
 *
 * Hydra_Software_Devel/458   9/14/11 5:14p yuxiaz
 * SW7425-1276: Undo previous change. Move
 * BVDC_P_Source_UpdateSrcState_isr up.
 *
 * Hydra_Software_Devel/457   9/14/11 3:51p yuxiaz
 * SW7425-1276: Fixed switch between 3DLR and 3DOU.
 *
 * Hydra_Software_Devel/456   9/14/11 11:47a yuxiaz
 * SW7405-5490: Reduce slave slot to 1 for each MFD source in mosaic mode.
 *
 * Hydra_Software_Devel/455   9/8/11 10:21a syang
 * SW7425-1265: honor back to back new setting
 *
 * Hydra_Software_Devel/454   9/1/11 9:48p yuxiaz
 * SW7425-985, SW7425-967: Added MRE support in MFD.
 *
 * Hydra_Software_Devel/453   8/27/11 7:23p hongtaoz
 * SW7425-1132: replaced slip2lock boolean flag with integer counter to
 * fix a timing sensitive hang conditiion when NRT sync-locked window is
 * brought up;
 *
 * Hydra_Software_Devel/452   8/19/11 1:36p pntruong
 * SW7425-59: Don't flood the with the repeat message.
 *
 * Hydra_Software_Devel/451   7/18/11 11:21a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 *
 * Hydra_Software_Devel/450   7/8/11 5:22p vanessah
 * SW7425-832: get rid of the workround and platform dependent limitation
 *
 * Hydra_Software_Devel/449   7/1/11 4:06p vanessah
 * SW7425-832: set monitor refresh rate default value to display refresh
 * rate when XVD give BFMT_Vert_eInvalid value.
 *
 * Hydra_Software_Devel/448   6/28/11 5:24p vanessah
 * SW7425-788: fix coverity error
 *
 * Hydra_Software_Devel/447   6/28/11 4:56p vanessah
 * SW7425-686: change Srcpolarity when XVD and BVN refreshrate mismatch
 *
 * Hydra_Software_Devel/446   6/28/11 9:40a vanessah
 * SW7425-686: Get rid of black frame
 *
 * Hydra_Software_Devel/445   6/27/11 7:58p pntruong
 * SW7425-772: Fine tuned deinterlacer policy.
 *
 * Hydra_Software_Devel/444   6/26/11 5:55p vanessah
 * SW7425-686: fix XVD and BVN fmt missync by mute MFD
 *
 * Hydra_Software_Devel/443   6/10/11 6:27p hongtaoz
 * SW7425-704: fixed compile error for chips without STG;
 *
 * Hydra_Software_Devel/442   6/10/11 5:49p hongtaoz
 * SW7425-704: add NRT mode support to VDC; moved the STG meta data
 * passing from window writer isr to reader isr; added bStallStc flag
 * support;
 *
 * Hydra_Software_Devel/441   5/17/11 4:19p yuxiaz
 * SW7422-446: Added orientation override for graphics source.
 *
 * Hydra_Software_Devel/440   5/11/11 3:50p pntruong
 * SW7231-140: Correctly compute the MFD fieldswap flag.
 *
 * Hydra_Software_Devel/439   4/8/11 1:56p yuxiaz
 * SW7425-236: Fixed flashing when source orientation changes.
 *
 * Hydra_Software_Devel/438   3/30/11 4:40p yuxiaz
 * SW7420-1186: Change BDBG_MSG to BDBG_WRN when VDC gets stream larger
 * than 720p frame picture.
 *
 * Hydra_Software_Devel/437   3/29/11 1:42p yuxiaz
 * SW7422-143, SW7425-236: Added new source dirty bit for orientation
 * change. Fixed flashing when source orientation changes.
 *
 * Hydra_Software_Devel/436   2/24/11 4:51p albertl
 * SW7425-59: Changed debug message from BDBG_WRN.
 *
 * Hydra_Software_Devel/435   2/24/11 4:33p albertl
 * SW7405-4434: Fixed ApplyChanges taking more than one vsync on sync
 * locked windows.
 *
 * Hydra_Software_Devel/434   2/10/11 1:17p hongtaoz
 * SW7425-59: updated buffer repeat debug message;
 *
 * Hydra_Software_Devel/433   2/4/11 4:21p yuxiaz
 * SW7550-661: About shake of screen when NEXUS_VideoWindow_AddInput call.
 *
 * Hydra_Software_Devel/432   12/29/10 6:12p pntruong
 * SW7422-175: Scan out SVC 1080p@60 content as 1080p and not 1080i.
 *
 * Hydra_Software_Devel/431   12/20/10 5:46p yuxiaz
 * SW7422-34: Added BVDC_Source_SetOrientation and
 * BVDC_Source_GetOrientation.
 *
 * Hydra_Software_Devel/430   12/9/10 9:51a yuxiaz
 * SW7422-39: Added more debug message for 3d.
 *
 * Hydra_Software_Devel/429   12/8/10 5:44p tdo
 * SW7422-36: Correctly program MCVP 3D mode based on source and display
 * orientation
 *
 * Hydra_Software_Devel/428   11/12/10 3:56p pntruong
 * SW7425-31: Takes bvn yuv into account for hddvi input.  Fixed bad debug
 * register read on non-mfd source.  Updated scratch registers
 * availabliity.
 *
 * Hydra_Software_Devel/427   11/11/10 7:30p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/426   11/11/10 7:12p hongtaoz
 * SW7425-51: reduced combo trigger use in VDC to 1 per MFD source;
 *
 * Hydra_Software_Devel/425   10/29/10 10:58a yuxiaz
 * SW7422-31, SW7422-32, SW7422-39: More for 3D Video support.
 *
 * Hydra_Software_Devel/424   10/11/10 12:43p jessem
 * SW7420-173: Added support for VFD as source feature.
 *
 * Hydra_Software_Devel/423   9/21/10 6:10p yuxiaz
 * SW7422-31, SW7422-32: Handle single and dual buffer 3D cases in MFD.
 *
 * Hydra_Software_Devel/422   8/31/10 5:00p yuxiaz
 * SW7422-39: Clean up feeder programming.
 *
 * Hydra_Software_Devel/421   6/18/10 4:23p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/420   5/7/10 7:20p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/419   5/3/10 4:57p syang
 * SW7405-4239: 1). use uint64_t for aspect ratio numbers; 2). avoid even
 * pixel rounding before aspect ratio correction calculation
 *
 * Hydra_Software_Devel/418   4/23/10 6:00p tdo
 * SW7401-3828: Support AFD=2, 3 for asia market. Add the AFD clipping for
 * SAR that has equivalent DAR of 4x3 and 16x9.
 *
 * Hydra_Software_Devel/417   4/19/10 10:20p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/416   4/8/10 2:09p syang
 * SW7405-4090: in non-mosaic mode case need to initMute only if
 * picChanged
 *
 * Hydra_Software_Devel/415   4/7/10 11:35a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/414   4/6/10 2:12p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Fix window timeout issue
 *
 * Hydra_Software_Devel/413   4/5/10 4:13p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/412   4/2/10 2:57p yuxiaz
 * SW7405-3965: Mosaic decoder can't be attached to the window of the 2nd
 * display.
 *
 * Hydra_Software_Devel/411   4/1/10 4:48p syang
 * SW7405-4090: avoid to use prev mosaic rect data in muted case
 *
 * Hydra_Software_Devel/410   3/17/10 12:04p syang
 * SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
 * same as video; ensure that reader and writer vnetState match in sync-
 * locked case; aligned width up to even for stride in feeder and cap;
 * assert mosaic mode doesn't co-exist with dest cut;
 *
 * Hydra_Software_Devel/409   12/23/09 1:13p yuxiaz
 * SW7405-3670: Source callback doesn't work correctly for MPEG source.
 *
 * Hydra_Software_Devel/408   12/22/09 5:16p tdo
 * SW7401-3828: Add AFD support
 *
 * Hydra_Software_Devel/407   12/22/09 4:02p syang
 * SW7550-143: fix coverity complain
 *
 * Hydra_Software_Devel/406   12/18/09 10:29a yuxiaz
 * SW7405-3633: Fix Coverity warings in VDC
 *
 * Hydra_Software_Devel/405   12/2/09 3:04p yuxiaz
 * SW7405-3120: Added support for overlapping mosaic videos with z-
 * ordering
 *
 * Hydra_Software_Devel/404   11/17/09 10:00p pntruong
 * SW3556-866: Expose vdec frontend csc for adc calibration for scart
 * input.
 *
 * Hydra_Software_Devel/403   11/16/09 5:03p yuxiaz
 * SW7405-3406: Clean up bMosaicMode and  bMosaicClearOutside.
 *
 * Hydra_Software_Devel/402   11/6/09 3:28p yuxiaz
 * SW7420-117: Added support for Newer Chroma Sample Location Types.
 *
 * Hydra_Software_Devel/401   10/22/09 6:20p hongtaoz
 * SW7405-3280: always update source picture context;
 *
 * Hydra_Software_Devel/400   10/22/09 2:23p yuxiaz
 * SW7405-3200: Destroy PIP window causes momentary flashes on the display
 * in mosaic mode.
 *
 * Hydra_Software_Devel/399   10/5/09 3:21p tdo
 * SW7405-3044:align H&V size in Mpeg source data ready so that it can be
 * consistent downstream
 *
 * Hydra_Software_Devel/398   6/2/09 3:03p syang
 * PR 55646:  smoothly filtering  in field inversion cases
 *
 * Hydra_Software_Devel/397   5/28/09 2:19p yuxiaz
 * PR55364: Disable dithering when any testfeature1 is enabled
 *
 * Hydra_Software_Devel/396   5/20/09 7:31p pntruong
 * PR53654: Make for format change msg consistence for ease of debugging
 * memory allocation.
 *
 * Hydra_Software_Devel/395   4/24/09 9:09a syang
 * PR 54557:  avoid src_isr to build ShutDownRul before disp_isr cleanUp
 * src slots
 *
 * Hydra_Software_Devel/394   4/17/09 11:56a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/393   4/10/09 6:28p hongtaoz
 * PR54064: source pending callback must fire after window shutdown;
 *
 * Hydra_Software_Devel/392   4/3/09 4:34p yuxiaz
 * PR53460: Optimal 444/422 conversion settings
 *
 * Hydra_Software_Devel/391   3/30/09 4:12p rpan
 * PR53677: Add support for XVD NTSC->PAL frame drop mode.
 *
 * Hydra_Software_Devel/390   3/16/09 10:50p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/4   3/16/09 1:02p tdo
 * PR45785, PR45789: merge from main branch on 3/16/09
 *
 * Hydra_Software_Devel/389   3/13/09 5:29p syang
 * PR 52376, PR 41481, PR 44041:
 * 1). exec user buf returning to hBuffer before shut-down code in
 * writer_isr; 2). clean up mad-delay handling code and add handling to
 * all settings in pic-node that are delayed by mad; 3). 444 SCL no-
 * longer uses BVN_IN to align-up for half-band; 4). add pic-node rects
 * dump feature as setting lbox_0_scratch to 2 (1 still printing mpeg pic
 * info).
 *
 * Hydra_Software_Devel/7420_mcvp/3   3/2/09 4:21p tdo
 * PR 45785, PR 45789: merge from main branch on 3/02/09
 *
 * Hydra_Software_Devel/388   3/2/09 3:46p yuxiaz
 * PR52287: More update on 422-444 and 444-422 converters.
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/5/09 9:53a syang
 * PR 45785, PR 45789:   get rid of BVDC_Source_Get/SetAnrConfiguration
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:31p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/387   1/5/09 8:23p pntruong
 * PR50722, PR50724: Improve the speed of RGB input auto phase adjustment.
 *
 * Hydra_Software_Devel/PR50722/1   12/29/08 12:34p gorgon
 * PR50724: Improve the speed of RGB input auto phase adjustment.
 *
 * Hydra_Software_Devel/386   12/4/08 5:23p pntruong
 * PR50002: VDC: Added function to force source pending.
 *
 * Hydra_Software_Devel/385   12/1/08 7:50p pntruong
 * PR49391, PR49398: Initialized get storage to avoid unitialized usage.
 *
 * Hydra_Software_Devel/384   11/24/08 3:34p albertl
 * PR49393: Fixed unused value coverity warning
 *
 * Hydra_Software_Devel/383   11/10/08 5:01p syang
 * PR 41898: remove un-needed bools from win-context and dirty bits
 *
 * Hydra_Software_Devel/382   11/7/08 5:24p albertl
 * PR48740: Fixed compiler warnings when debug is turned off.
 *
 * Hydra_Software_Devel/381   10/25/08 1:29p pntruong
 * PR47747: Added ability to bypass adc common mode.
 *
 * Hydra_Software_Devel/380   10/3/08 3:34p syang
 * PR 45751:  no need to send src-pending callback if pic changes from un-
 * muted to muted
 *
 * Hydra_Software_Devel/379   10/3/08 12:21p syang
 * pr 47129, pr 45402 :  1). improve  the VDC internally used videoFmtCode
 * approximate; 2). return w, h, frmRate and bInterlaced with src-pending
 * callback for non-enumerated mpeg-src video format
 *
 * Hydra_Software_Devel/378   9/26/08 1:32p pntruong
 * PR46515: Adapted to new power management.
 *
 * Hydra_Software_Devel/377   9/26/08 11:49a syang
 * PR45751: PR43944: send out source pending callback when mpeg field mute
 * status change
 *
 * Hydra_Software_Devel/PR45751/1   9/16/08 8:33p mlei
 * PR43944: send out source pending callback when mpeg field mute status
 * change
 *
 * Hydra_Software_Devel/376   8/28/08 5:44p pntruong
 * PR46252: Need to expose 444 format in static buffers case.  Localize
 * the validity of pixel in feeder.
 *
 * Hydra_Software_Devel/375   8/21/08 6:20p pntruong
 * PR45830: Allowed the 1080p feeding for jpg thru mfd.
 *
 * Hydra_Software_Devel/374   8/12/08 9:31p pntruong
 * PR45566: Added support for standard (legacy) mode of combing.
 *
 * Hydra_Software_Devel/373   7/11/08 11:32a yuxiaz
 * PR44072, PR43785: Clear callback dirty bits.
 *
 * Hydra_Software_Devel/372   7/10/08 10:49a pntruong
 * PR40213: Corrected the bHVStartOverride it must be set thru the
 * BVDC_Source_SetHVStart API.
 *
 * Hydra_Software_Devel/371   7/2/08 10:51a yuxiaz
 * PR44072, PR43785: Do not adjust original source size if requested to
 * capture CRC.
 *
 * Hydra_Software_Devel/370   6/27/08 1:41p yuxiaz
 * PR43785: Combine source pending and CRC callback into one.
 *
 * Hydra_Software_Devel/369   6/24/08 4:20p syang
 * PR35549, PR44050:  1).  fire gen-callbck for format change even if not
 * src-pending;   2).  allow callbck even if NULL pic for jpeg pic
 * display
 *
 * Hydra_Software_Devel/368   6/22/08 9:42a pntruong
 * PR40213: Fine tune settings for PC raster adjustment.
 *
 * Hydra_Software_Devel/367   6/20/08 3:43p tdo
 * PR43662 , PR43645, PR38841: Fix mosaic and SD to HD channel change due
 * to adjust rectangle being ignored when picture is muted.
 *
 * Hydra_Software_Devel/366   6/18/08 3:05p syang
 * PR35549: 1). src-pending when new win is created; 2). don't do
 * BufferHeap_AllocateBuffers as bReConfigSrc
 *
 * Hydra_Software_Devel/365   6/14/08 4:00p syang
 * PR35549:  added src-pending callback for mpeg src
 *
 * Hydra_Software_Devel/364   5/30/08 1:37p tdo
 * PR38362, PR38678, PR38841, PR38842: More fine tuning of the BVN HW
 * limitation implementation.
 *
 * Hydra_Software_Devel/363   5/29/08 1:30p tdo
 * PR38362, PR38678, PR38841, PR38842: Enforce Hardware Minimum Size
 * Limitation for BVN Blocks and Partition Between Destination and Source
 * Clipping To Support 2x1 Window Size
 *
 * Hydra_Software_Devel/362   5/13/08 3:35p jessem
 * PR 39456: Added big endian check for valid pixel formats in
 * BVDC_P_Source_ValidateMpegData_isr. Also, removed muting of picture if
 * pixel format is invalid. If invalid, a green and pink picture will
 * result alerting the user that the pixel format used is not supported.
 *
 * Hydra_Software_Devel/361   5/8/08 5:54p pntruong
 * PR35549: Added more preparations for source pending.
 *
 * Hydra_Software_Devel/360   4/21/08 4:34p hongtaoz
 * PR35398, PR40895: some compiler might generate floating point
 * instructions;
 *
 * Hydra_Software_Devel/359   4/3/08 1:47p tdo
 * PR38771, PR38777, PR39246: all chips with DNR are supporting DCR now,
 * remove unnecessary check for DCR
 *
 * Hydra_Software_Devel/358   4/1/08 5:24p pntruong
 * PR41124: [vdc]: Add an api to get source default h/v start.
 *
 * Hydra_Software_Devel/357   3/26/08 1:15p hongtaoz
 * PR35398, PR40895: added ulVsyncRate and removed bGameModeDelayLags in
 * the window lipsync callback structure; added BFMT_FREQ_FACTOR to unify
 * Vsync rate and pixel frequency calculation;
 *
 * Hydra_Software_Devel/356   3/25/08 3:17p syang
 * PR 40431: add complete assert for critical section protection among src
 * _isr, dsp _isr, and ApplyChanges
 *
 * Hydra_Software_Devel/355   3/19/08 5:28p pntruong
 * PR38442: Updated BVDC_Source_SetAdcConfiguration so that only set dirty
 * bit on change is detected, and set bAdcSetting dirty bit.  This dirty
 * bit will re-initialize vdec internal state machine, and briefly mute
 * video.
 *
 * Hydra_Software_Devel/354   3/17/08 5:49p pntruong
 * PR39162: Added stub api for documentation.
 *
 * Hydra_Software_Devel/353   3/3/08 4:47p pntruong
 * PR40201: Added better default for choosing interlaced/progressived.
 *
 * Hydra_Software_Devel/352   3/3/08 4:11p yuxiaz
 * PR40201: VDC has bad BDBG_WRN which causes intermittent crash
 *
 * Hydra_Software_Devel/351   2/26/08 4:48p jessem
 * PR 34590, PR 39456: Removed ENDIAN treatment of pixel format of
 * pNewPic->hSurface in BVDC_P_Source_ValidateMpegData_isr.
 *
 * Hydra_Software_Devel/350   2/21/08 5:01p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/349   2/15/08 2:33p yuxiaz
 * PR34712, PR39251: Use feeder version to combine all the feeder flags.
 * Regroup feeder support based on feeder version.
 *
 * Hydra_Software_Devel/348   2/12/08 7:43p tdo
 * PR38771, PR38777, PR39246: Implement changes for DNR/DCR cores for 3548
 *
 * Hydra_Software_Devel/347   2/4/08 6:13p pntruong
 * PR39317: Correctly search for 288p and 240p.
 *
 * Hydra_Software_Devel/346   1/24/08 5:10p pntruong
 * PR38832: Video decoding and display (noise stream) failed in customer's
 * QA testing.  Need ensure parameters from decoder to mfd is valid.
 *
 * Hydra_Software_Devel/345   1/22/08 11:51a pntruong
 * PR38657:  Relax format mapping to only scantype and height.
 *
 * Hydra_Software_Devel/344   1/21/08 3:42p pntruong
 * PR38657: 576p video blinking problem with Phase5.0 take2.  Identify
 * correct source information to determine correct cappture buffer size.
 *
 * Hydra_Software_Devel/343   11/15/07 3:53p yuxiaz
 * PR34523: 7325 bring up.
 *
 * Hydra_Software_Devel/342   11/14/07 10:55a rpan
 * PR31084: Consider NULL pointers as valid values when getting
 * VDEC/Deinterlace default settings.
 *
 * Hydra_Software_Devel/341   11/13/07 5:34p rpan
 * PR31084: Added support for getting VDEC default settings.
 *
 * Hydra_Software_Devel/340   10/30/07 7:24p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/339   10/23/07 11:17a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/3   10/19/07 11:31a yuxiaz
 * PR29569: Merge from mainline.
 *
 * Hydra_Software_Devel/338   10/19/07 8:33a pntruong
 * PR36256: [VDEC][PC]:Ensure the H positon is right when finding new
 * format, adjusting manua phase and adjusting manual clock.
 *
 * Hydra_Software_Devel/Gorgon_temp_20071019/1   10/19/07 2:56p gorgon
 * PR36256:[VDEC][PC]:Ensure the H positon is right when finding new
 * format, adjusting manua phase and adjusting manual clock.
 *
 * Hydra_Software_Devel/337   10/18/07 12:05a pntruong
 * PR36137: [PC]:Set pc input video format without disable auto format
 * detection.
 *
 * Hydra_Software_Devel/336   10/16/07 2:56p hongtaoz
 * PR34955: enforce 1080p condition for PsF feature; restore source
 * vertical freq when DM starts non-1080p picture, to avoid sync-slipped
 * multi-buffer bouncing due to rate gap;
 *
 * Hydra_Software_Devel/335   10/11/07 2:09p albertl
 * PR35135:  Fixed user csc macros and added 10-bit/8-bit video data
 * conversion.
 *
 * Hydra_Software_Devel/334   10/4/07 4:15p pntruong
 * PR35412: Clearout pointers for gets as specified in api.
 *
 * Hydra_Software_Devel/333   10/3/07 4:53p pntruong
 * PR35412: Expose API to enable/Disable of Coring
 * (VDEC_BE_0_DITHER_CORING_ENABLE).
 *
 * Hydra_Software_Devel/332   10/2/07 11:25a pntruong
 * PR35136: Some cleanup of unused code.
 *
 * Hydra_Software_Devel/331   9/23/07 3:38p hongtaoz
 * PR34955: added 1080p PsF support for PIP mpg source; fixed debug
 * message error;
 *
 * Hydra_Software_Devel/330   9/21/07 3:59p hongtaoz
 * PR34955: added Progressive Segmented Frame(PsF) 1080p support; fixed
 * 1080p pass-through condition; fixed bvdc_dbg register access error;
 * fixed general progressive capture interlaced playback feeder
 * programming problem;
 *
 * Hydra_Software_Devel/329   9/20/07 1:53a albertl
 * PR34966:  Added 7440 to 1080p MFD support macro.
 *
 * Hydra_Software_Devel/328   9/20/07 1:21a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/327   9/12/07 3:24p rpan
 * PR 34144: Added 7400 to 1080p support list
 *
 * Hydra_Software_Devel/326   9/12/07 11:28a rpan
 * PR 34144: Allow 1080p mpeg src to pass through vdc pipe line as 1080p.
 *
 * Hydra_Software_Devel/326   9/12/07 11:25a rpan
 * PR 34144: Allow 1080p mpeg src to pass through vdc pipe line as 1080p.
 *
 * Hydra_Software_Devel/326   9/12/07 10:56a rpan
 * PR 34144: allow 1080p mpeg src to pass through vdc pipe line as 1080p.
 *
 * Hydra_Software_Devel/325   9/10/07 1:55p pntruong
 * PR34690: [PC]:Smooth the performance of manual clock. Toggle the
 * H_threshold when manual clock to make sure the position accurate.
 *
 * Hydra_Software_Devel/PR34690/1   9/10/07 8:58p gorgon
 * PR34690:[PC]:Smooth the performance of manual clock. Toggle the
 * H_threshold when manual clock to make sure the position accurate.
 *
 * Hydra_Software_Devel/324   8/6/07 11:08a pntruong
 * PR33348: Identify mpeg format of hd/sd/ed with curinfo->pfmt.
 *
 * Hydra_Software_Devel/323   7/31/07 10:12p pntruong
 * PR33634: ANR assert when turning off MAD and switching progressive mpeg
 * source to interlaced mpeg source.
 *
 * Hydra_Software_Devel/322   7/26/07 6:27p pntruong
 * PR33427:  Need to trigger aspect ratio computation when panscan widnow
 * size changes.
 *
 * Hydra_Software_Devel/321   7/23/07 10:14a pntruong
 * PR31869: [VDEC] snow noise blink between normal snow noise and black
 * screen.
 *
 * Hydra_Software_Devel/320   7/19/07 2:37p tdo
 * PR33034: Modify DNR API to extend MNR/BNR/DCR filter level
 *
 * Hydra_Software_Devel/319   7/9/07 3:48p pntruong
 * PR32775: Hstart user setting is overwritten in VDEC.
 *
 * Hydra_Software_Devel/318   6/27/07 1:46p pntruong
 * PR32503: Cleanup unused code.  Prepare for 480i/480p vdec oversample.
 *
 * Hydra_Software_Devel/317   6/27/07 12:32p tdo
 * PR32489: Add support for custom adjust DNR
 *
 * Hydra_Software_Devel/316   6/26/07 5:37p pntruong
 * PR32402: TV display "unsupport format" and "no signal" OSD for
 * componet/hdmi input.
 *
 * Hydra_Software_Devel/315   6/21/07 7:39p pntruong
 * PR30427: Allow blackoffset to be custom even for component input.
 *
 * Hydra_Software_Devel/314   6/21/07 3:38p syang
 * PR 32440: clean up inefficient "if" statement
 *
 * Hydra_Software_Devel/313   6/21/07 2:17p yuxiaz
 * PR32023: Added video format tolerance to HDDVI.
 *
 * Hydra_Software_Devel/312   6/14/07 7:30p pntruong
 * PR31977: [VDEC]Auto detection for PAL can not work well when user
 * toggle auto/manual mode.
 *
 * Hydra_Software_Devel/311   6/13/07 2:25p hongtaoz
 * PR28019: downgrade message level for the invalid eYCbCr type to avoid
 * flooding the terminal before decode;
 *
 * Hydra_Software_Devel/310   6/13/07 9:45a syang
 * PR 31944: put ANR K value table inro seperate file; change to use
 * SnDbAdj from 5 level adj
 *
 * Hydra_Software_Devel/309   6/11/07 7:25p tdo
 * PR31947: provide API to allow user setting the Qp value for DNR
 *
 * Hydra_Software_Devel/308   6/6/07 2:01p yuxiaz
 * PR30015: Use stripe width from XVD.
 *
 * Hydra_Software_Devel/307   5/31/07 1:43p yuxiaz
 * PR30015: Added support in VDC to accept strip width as input parameter
 * from XVD.
 *
 * Hydra_Software_Devel/306   5/16/07 11:15a pntruong
 * PR30361: Provide API to control DC restore.
 *
 * Hydra_Software_Devel/305   5/15/07 11:47a yuxiaz
 * PR29940: Add support to set and get Hstart and Vstart for HDDVI.
 *
 * Hydra_Software_Devel/304   5/14/07 7:07p pntruong
 * PR30360: Provide API to customer VDEC YCbCr delay.
 *
 * Hydra_Software_Devel/303   5/14/07 3:05p hongtaoz
 * PR30307: DM should not send toggling frame/field to VDC once started;
 *
 * Hydra_Software_Devel/302   5/14/07 2:01p pntruong
 * PR30363: Provide API to enable/disable ACGC.
 *
 * Hydra_Software_Devel/301   5/14/07 1:13p pntruong
 * PR30427: Intialized black offset for vdec.
 *
 * Hydra_Software_Devel/300   5/14/07 11:40a pntruong
 * PR30427: Provide API to control black offset in VDEC.
 *
 * Hydra_Software_Devel/299   5/10/07 9:59a syang
 * PR 28895, PR 28898, PR 29846 , PR 28550: 1). add frame for C0 ANR PI
 * support; 2). move MAD/ANR buf alloc/free to MAD/ANR; 3). remove
 * drainVnet state
 *
 * Hydra_Software_Devel/298   4/19/07 1:48p yuxiaz
 * PR 26052: Add CSC support in MFD on 7400 B0.
 *
 * Hydra_Software_Devel/297   4/12/07 5:01p albertl
 * PR28019:  BVDC_Source_MpegDataReady_isr no longer modifies pvMvdField
 * and now uses local copy.
 *
 * Hydra_Software_Devel/296   4/9/07 11:25a pntruong
 * PR28266: Provide API to customize DVO CSC.
 *
 * Hydra_Software_Devel/295   4/6/07 9:49a pntruong
 * PR29555: Support feeding 1080p JPEG image from MFD.
 *
 * Hydra_Software_Devel/294   4/5/07 11:25p pntruong
 * PR28395, PR28538, PR28539, PR28540: Vdec customizations, and
 * deinterlacer customizations.
 *
 * Hydra_Software_Devel/293   4/5/07 9:23a syang
 * PR 27611: allow  ANR on digital src, DNR on analog src, and ANR and DNR
 * used together
 *
 * Hydra_Software_Devel/292   4/2/07 11:31a syang
 * PR 29243: clean up: take off the obselete old-vnet-code
 *
 * Hydra_Software_Devel/291   3/30/07 10:41a yuxiaz
 * PR28350, PR27180: Added BVDC_Source_GetInputStatus to get input status
 * for VDEC and HD_DVI.
 *
 ***************************************************************************/
#include "bstd.h"              /* standard types */
#include "bkni.h"              /* memcpy calls */
#include "bvdc.h"              /* Video display */
#include "bvdc_common_priv.h"
#include "bvdc_priv.h"
#include "bvdc_dnr_priv.h"
#include "bvdc_feeder_priv.h"
#include "bvdc_capture_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_hddvi_priv.h"
#include "bvdc_vdec_priv.h"
#include "bvdc_downsample_priv.h"
#include "bchp_int_id_bvnf_intr2_0.h"

BDBG_MODULE(BVDC_SRC);


/* How to map mpeg format to BFMT format. */
static const BFMT_VideoFmt s_aeMpegToFmt[] =
{
	BFMT_VideoFmt_eNTSC,
	BFMT_VideoFmt_ePAL_G,
	BFMT_VideoFmt_e480p,
	BFMT_VideoFmt_e576p_50Hz,
	BFMT_VideoFmt_e720p,
	BFMT_VideoFmt_e720p_50Hz,
	BFMT_VideoFmt_e1080i,
	BFMT_VideoFmt_e1080i_50Hz,
	BFMT_VideoFmt_e1080p,
	BFMT_VideoFmt_e1080p_24Hz,
	BFMT_VideoFmt_e1080p_25Hz,
	BFMT_VideoFmt_e1080p_30Hz,
	BFMT_VideoFmt_e1080p_50Hz,
	BFMT_VideoFmt_ePAL_60,
	BFMT_VideoFmt_e720p_24Hz,

	/* */
	BFMT_VideoFmt_e1250i_50Hz,
	BFMT_VideoFmt_e240p_60Hz,
	BFMT_VideoFmt_e288p_50Hz
};

static const uint32_t s_aulFrmRate[] =
{
	0,     /*BAVC_FrameRateCode_eUnknown = 0,*/
	2397,  /*BAVC_FrameRateCode_e23_976 = 1,*/
	2400,  /*BAVC_FrameRateCode_e24        ,*/
	2500,  /*BAVC_FrameRateCode_e25        ,*/
	2997,  /*BAVC_FrameRateCode_e29_97     ,*/
	3000,  /*BAVC_FrameRateCode_e30        ,*/
	5000,  /*BAVC_FrameRateCode_e50        ,*/
	5994,  /*BAVC_FrameRateCode_e59_94     ,*/
	6000   /*BAVC_FrameRateCode_e60         */
};

#define BVDC_P_MPEG_FMT_DELTA                 (16) /* detal for matching */
#define BVDC_P_MPEG_FMT_COUNT                 (sizeof(s_aeMpegToFmt)/sizeof(BFMT_VideoFmt))
#define BVDC_P_MAX_FRM_RATE_CODE              (sizeof(s_aulFrmRate)/sizeof(uint32_t) -1)

/* PsF: 30Hz is the max PsF scanout frequency (x100 to follow FMT style); */
#define BVDC_P_PSF_VERT_FREQ                     (30 * BFMT_FREQ_FACTOR)

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetDefaultSettings
	( BAVC_SourceId                    eSourceId,
	  BVDC_Source_Settings            *pDefSettings )
{
	BSTD_UNUSED(eSourceId);

	/* To make sure thing get initialize */
	BKNI_Memset(pDefSettings, 0, sizeof(*pDefSettings));

	/* TODO: double check the default settings */
	pDefSettings->hHeap        = NULL;
	pDefSettings->eDsSrcCompId = BVDC_CompositorId_eCompositor0;

	if (BVDC_P_SRC_IS_GFX(eSourceId) || BVDC_P_SRC_IS_VFD(eSourceId))
	{
		pDefSettings->bGfxSrc = true;
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_QueryVfd
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_WindowId                    eWindowId,
	  BAVC_SourceId                   *peSourceId )
{
	BVDC_Window_Handle  hWindow;
	BERR_Code  eRet;

	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BDBG_ASSERT(NULL!=peSourceId);

	eRet = BVDC_P_Window_GetPrivHandle(
		hCompositor, eWindowId, BAVC_SourceId_eVfd0, &hWindow);
	if (BERR_SUCCESS != eRet)
	{
		return BERR_TRACE(eRet);
	}

	*peSourceId = BAVC_SourceId_eVfd0 +
		(hWindow->pResource->ePlayback - BVDC_P_FeederId_eVfd0);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_Create
	( BVDC_Handle                      hVdc,
	  BVDC_Source_Handle              *phSource,
	  BAVC_SourceId                    eSourceId,
	  const BVDC_Source_Settings      *pDefSettings )
{
	BVDC_P_SourceContext *pSource;
	BVDC_P_DrainContext stTmpDrain;

	BDBG_ENTER(BVDC_Source_Create);
	BDBG_ASSERT(phSource);
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	BKNI_Memset((void*)&stTmpDrain, 0x0, sizeof(BVDC_P_DrainContext));

	/* Making sure features is available. */
	if(!hVdc->pFeatures->abAvailSrc[eSourceId])
	{
		BDBG_ERR(("Source[%d] not supported on this chipset.", eSourceId));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	/* User asked to disabled these sources in BVDC_Open */
	if((BVDC_P_SRC_IS_HDDVI(eSourceId) && hVdc->stSettings.bDisableHddviInput) ||
	   (BVDC_P_SRC_IS_ITU656(eSourceId) && hVdc->stSettings.bDisable656Input))
	{
		BDBG_ERR(("Source[%d] disabled by BVDC_Settings.", eSourceId));
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	/* Find an available drain for the source. */
	if(BVDC_P_SRC_NEED_DRAIN(eSourceId))
	{
		if(BVDC_P_Drain_Acquire(&stTmpDrain, hVdc->hResource, eSourceId) != BERR_SUCCESS)
		{
			BDBG_ERR(("No Vnet drain available for Source[%d]", eSourceId));
			BDBG_ERR(("Source[%d] not supported on this chipset.", eSourceId));
			return BERR_TRACE(BERR_NOT_SUPPORTED);
		}
	}

	pSource = hVdc->ahSource[eSourceId];
	BDBG_OBJECT_ASSERT(pSource, BVDC_SRC);
	pSource->stDrain = stTmpDrain;

	if(BVDC_P_SRC_IS_DS(eSourceId))
	{
		BVDC_P_CompositorContext *pDsCompositor;

		pSource->eDsSrcCompId = (pDefSettings)
			? pDefSettings->eDsSrcCompId : BVDC_CompositorId_eCompositor0;

		pDsCompositor = hVdc->ahCompositor[pSource->eDsSrcCompId];

		if(pDsCompositor->eState != BVDC_P_State_eActive)
		{
			BDBG_ERR(("Source compositor must be open before opening downsample source."));
			return BERR_INVALID_PARAMETER;
		}
	}

	/* Check if source is created or not. */
	if(BVDC_P_STATE_IS_ACTIVE(pSource) ||
	   BVDC_P_STATE_IS_CREATE(pSource) ||
	   BVDC_P_STATE_IS_DESTROY(pSource))
	{
		return BERR_TRACE(BVDC_ERR_SOURCE_ALREADY_CREATED);
	}

	BDBG_MSG(("Creating source%d", pSource->eId));
	BDBG_ASSERT(BVDC_P_STATE_IS_INACTIVE(pSource));

	/* Reinitialize context.  But not make it active until apply. */
	*phSource = pSource;

	BVDC_P_Source_Init(*phSource, pDefSettings);

	/* Mark as create, awating for apply. */
	pSource->eState = BVDC_P_State_eCreate;

	BDBG_LEAVE(BVDC_Source_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_Destroy
	( BVDC_Source_Handle               hSource )
{
	int i;

	BDBG_ENTER(BVDC_Source_Destroy);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* Check to see if there are any active windows that's still using
	 * this source. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(hSource->ahWindow[i]))
		{
			BDBG_ERR(("Window %d is still using this source.", hSource->ahWindow[i]->eId));
			BDBG_ERR(("The window state is %d.", hSource->ahWindow[i]->eState));
			return BERR_TRACE(BERR_LEAKED_RESOURCE);
		}
	}

	/* Release the drain for the source. */
	if(BVDC_P_SRC_NEED_DRAIN(hSource->eId))
	{
		BVDC_P_Drain_Release(&hSource->stDrain, hSource->hVdc->hResource);
	}

	/* Disconnect source compositor from DS. */
	if( BVDC_P_SRC_IS_DS(hSource->eId) )
	{
		BVDC_P_DownSample_Disconnect(hSource->hDownSample);
		BDBG_MSG(("Src[%d] disconnected from source compositor %d", hSource->eDsSrcCompId));
	}

	if(BVDC_P_STATE_IS_DESTROY(hSource) ||
	   BVDC_P_STATE_IS_INACTIVE(hSource))
	{
		goto done;
	}

	if(BVDC_P_STATE_IS_ACTIVE(hSource) ||
	   BVDC_P_STATE_IS_CREATE(hSource))
	{
		hSource->eState = BVDC_P_State_eDestroy;
	}

done:
	BDBG_LEAVE(BVDC_Source_Destroy);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetSize
	( const BVDC_Source_Handle         hSource,
	  uint32_t                        *pulWidth,
	  uint32_t                        *pulHeight )
{
	const BFMT_VideoInfo *pFmtInfo;
	uint32_t ulWidth  = 0;
	uint32_t ulHeight = 0;

	BDBG_ENTER(BVDC_Source_GetSize);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* Analog/656 we know the source size, but for mpeg the source size
	 * can change dynamically (dynamic source picture change).   Hence
	 * we can only return the size of analog or 656 source. */
	if(BVDC_P_SRC_IS_ANALOG(hSource->eId) ||
	   BVDC_P_SRC_IS_ITU656(hSource->eId))
	{
		pFmtInfo = hSource->stCurInfo.pFmtInfo;
		ulWidth  = pFmtInfo->ulWidth;
		ulHeight = pFmtInfo->ulHeight;
	}
	else if(BVDC_P_SRC_IS_MPEG(hSource->eId) ||
	        BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		BKNI_EnterCriticalSection();
		ulWidth  = hSource->stExtVideoFmtInfo.ulWidth;
		ulHeight = hSource->stExtVideoFmtInfo.ulHeight;
		BKNI_LeaveCriticalSection();
	}

	if(pulWidth)
	{
		*pulWidth = ulWidth;
	}

	if(pulHeight)
	{
		*pulHeight = ulHeight;
	}

	BDBG_LEAVE(BVDC_Source_GetSize);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetVideoFormat
	( BVDC_Source_Handle               hSource,
	  BFMT_VideoFmt                    eVideoFmt )
{
	BVDC_P_Source_Info *pNewInfo;

	BDBG_ENTER(BVDC_Source_SetVideoFormat);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pNewInfo  = &hSource->stNewInfo;

	/* Restricted to only supported format. */
	if(BVDC_P_SRC_IS_ITU656(hSource->eId))
	{
		/* 656 specific validation. */
		if(!VIDEO_FORMAT_IS_PAL(eVideoFmt) &&
		   !VIDEO_FORMAT_IS_NTSC(eVideoFmt))
		{
			return BERR_TRACE(BVDC_ERR_VIDEO_FORMAT_NOT_SUPPORTED);
		}
	}
	else if(BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		/* Support all or limit some for here! */
	}
	else if(!BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* set new info */
	pNewInfo->pFmtInfo = BFMT_GetVideoFormatInfoPtr(eVideoFmt);
	pNewInfo->pVdcFmt  = BVDC_P_GetFormatInfoPtr_isr(pNewInfo->pFmtInfo);

	if((hSource->stCurInfo.pFmtInfo->eVideoFmt != eVideoFmt) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set */
		pNewInfo->stDirty.stBits.bInputFormat = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_SetVideoFormat);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetVideoFormat
	( const BVDC_Source_Handle         hSource,
	  BFMT_VideoFmt                   *peVideoFmt )
{
	BDBG_ENTER(BVDC_Source_GetVideoFormat);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* set return value */
	if(peVideoFmt)
	{
		*peVideoFmt = hSource->stCurInfo.pFmtInfo->eVideoFmt;
	}

	BDBG_LEAVE(BVDC_Source_GetVideoFormat);
	return BERR_SUCCESS;
}


/***************************************************************************
 * User call this function to get the interrupt name, so they can create
 * callback with BINT_CreateCallback.
 *
 */
BERR_Code BVDC_Source_GetInterruptName
	( BVDC_Source_Handle               hSource,
	  const BAVC_Polarity              eFieldId,
	  BINT_Id                         *pInterruptName )
{
	BDBG_ENTER(BVDC_Source_GetInterruptName);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_MPEG(hSource->eId))
	{
		BDBG_ERR(("Not a MPEG source"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* set return value */
	if(pInterruptName)
	{
		/* Non-mpeg source should not need the interrupt name. */
		*pInterruptName = BRDC_Slot_GetIntId(
			hSource->ahSlot[BVDC_P_NEXT_POLARITY(eFieldId)]);

		BDBG_MSG(("What interrupt MVD should response to next "));
		BDBG_MSG(("eFieldId==%d?  Ans: %d (or BINT_ID=0x%08x)",
			eFieldId, BVDC_P_NEXT_POLARITY(eFieldId), *pInterruptName));
	}

	BDBG_LEAVE(BVDC_Source_GetInterruptName);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetVideoMuteColor
	( BVDC_Source_Handle               hSource,
	  const uint8_t                    ucRed,
	  const uint8_t                    ucGreen,
	  const uint8_t                    ucBlue )
{
	unsigned int uiColorARGB;
	BVDC_P_Source_Info *pNewInfo;

	BDBG_ENTER(BVDC_Source_SetVideoMuteColor);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pNewInfo = &hSource->stNewInfo;

	/* set new info */
	pNewInfo->ucRed   = ucRed;
	pNewInfo->ucGreen = ucGreen;
	pNewInfo->ucBlue  = ucBlue;
	uiColorARGB = BPXL_MAKE_PIXEL(BPXL_eA8_R8_G8_B8, 0x00, ucRed, ucGreen, ucBlue);
	BPXL_ConvertPixel_RGBtoYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8,
		uiColorARGB, (unsigned int*)&pNewInfo->ulMuteColorYCrCb);

	if((hSource->stCurInfo.ucRed   != ucRed) ||
	   (hSource->stCurInfo.ucGreen != ucGreen) ||
	   (hSource->stCurInfo.ucBlue  != ucBlue) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set */
		pNewInfo->stDirty.stBits.bUserChanges = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_SetVideoMuteColor);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetVideoMuteColor
	( const BVDC_Source_Handle          hSource,
	  uint8_t                          *pucRed,
	  uint8_t                          *pucGreen,
	  uint8_t                          *pucBlue )
{
	BDBG_ENTER(BVDC_Source_GetVideoMuteColor);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(pucRed)
	{
		*pucRed   = hSource->stCurInfo.ucRed;
	}

	if(pucGreen)
	{
		*pucGreen = hSource->stCurInfo.ucGreen;
	}

	if(pucBlue)
	{
		*pucBlue  = hSource->stCurInfo.ucBlue;
	}

	BDBG_LEAVE(BVDC_Source_GetVideoMuteColor);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetMuteMode
	( BVDC_Source_Handle               hSource,
	  BVDC_MuteMode                    eMuteMode )
{
	BVDC_P_Source_Info *pNewInfo;

	BDBG_ENTER(BVDC_Source_SetMuteMode);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pNewInfo = &hSource->stNewInfo;

	if(eMuteMode > BVDC_MuteMode_eRepeat)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(((BVDC_P_SRC_IS_VFD(hSource->eId)) ||
		(BVDC_P_SRC_IS_MPEG(hSource->eId) && hSource->hMpegFeeder->bGfxSrc)) &&
	   (BVDC_MuteMode_eConst < eMuteMode))
	{
		BDBG_ERR(("MPEG/VFD source %d don't need to SetMuteMode to repeat gfx surface.", hSource->eId));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(BVDC_P_SRC_IS_MPEG(hSource->eId) && !hSource->hMpegFeeder->bGfxSrc &&
	   (BVDC_MuteMode_eDisable != eMuteMode))
	{
		BDBG_ERR(("MPEG source %d shall be muted from decoder.", hSource->eId));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* set new value */
	pNewInfo->eMuteMode = eMuteMode;

	if((hSource->stCurInfo.eMuteMode != eMuteMode) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set */
		pNewInfo->stDirty.stBits.bUserChanges = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_SetMuteMode);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetMuteMode
	( const BVDC_Source_Handle          hSource,
	  BVDC_MuteMode                    *peMuteMode )
{
	BDBG_ENTER(BVDC_Source_GetMuteMode);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(peMuteMode)
	{
		*peMuteMode = hSource->stCurInfo.eMuteMode;
	}

	BDBG_LEAVE(BVDC_Source_GetMuteMode);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_OverrideAspectRatio
	( BVDC_Source_Handle               hSource,
	  BFMT_AspectRatio                 eAspectRatio )
{
	BVDC_P_Source_Info *pNewInfo;

	BDBG_ENTER(BVDC_Source_OverrideAspectRatio);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pNewInfo = &hSource->stNewInfo;

	if(eAspectRatio > BFMT_AspectRatio_e15_9)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* set new value */
	pNewInfo->eAspectRatio  = eAspectRatio;

	/* don't auto-update src aspect ratio accoring to videoFmt in
	 * next ApplyChanges, since user override it */
	hSource->bNewUserModeAspRatio = true;

	if((hSource->stCurInfo.eAspectRatio != eAspectRatio) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set */
		pNewInfo->stDirty.stBits.bAspectRatio = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_OverrideAspectRatio);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_OverrideAspectRatio_isr
	( BVDC_Source_Handle               hSource,
	  BFMT_AspectRatio                 eAspectRatio )
{
	BDBG_ENTER(BVDC_Source_OverrideAspectRatio_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(eAspectRatio > BFMT_AspectRatio_e15_9)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if((hSource->stCurInfo.eAspectRatio != eAspectRatio) ||
	   ((hSource->stIsrInfo.stDirty.stBits.bAspectRatio) &&
		(hSource->stIsrInfo.eAspectRatio != eAspectRatio)))
	{
		BVDC_P_Source_IsrInfo *pIsrInfo = &hSource->stIsrInfo;
		BVDC_P_Source_DirtyBits *pIsrDirty = &pIsrInfo->stDirty;

		/* set isr value */
		pIsrInfo->eAspectRatio  = eAspectRatio;

		/* Dirty bit set */
		pIsrDirty->stBits.bAspectRatio = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_OverrideAspectRatio_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetAspectRatio
	( const BVDC_Source_Handle          hSource,
	  BFMT_AspectRatio                 *peAspectRatio )
{
	BDBG_ENTER(BVDC_Source_GetAspectRatio);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(peAspectRatio)
	{
		*peAspectRatio = hSource->stCurInfo.eAspectRatio;
	}

	BDBG_LEAVE(BVDC_Source_GetAspectRatio);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetAspectRatioCanvasClip
	( BVDC_Source_Handle               hSource,
	  uint32_t                         ulLeft,
	  uint32_t                         ulRight,
	  uint32_t                         ulTop,
	  uint32_t                         ulBottom )
{
	BVDC_P_Source_Info *pNewInfo;

	BDBG_ENTER(BVDC_Source_SetAspectRatioCanvasClip);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pNewInfo = &hSource->stNewInfo;

	if(!BVDC_P_SRC_IS_VIDEO(hSource->eId))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}


	/* set new value */
	pNewInfo->stAspRatRectClip.ulLeft   = ulLeft;
	pNewInfo->stAspRatRectClip.ulRight  = ulRight;
	pNewInfo->stAspRatRectClip.ulTop    = ulTop;
	pNewInfo->stAspRatRectClip.ulBottom = ulBottom;
	pNewInfo->stAspRatRectClip.lLeftDelta_R = 0;

	if((hSource->stCurInfo.stAspRatRectClip.ulLeft   != ulLeft)   ||
	   (hSource->stCurInfo.stAspRatRectClip.ulRight  != ulRight)  ||
	   (hSource->stCurInfo.stAspRatRectClip.ulTop    != ulTop)    ||
	   (hSource->stCurInfo.stAspRatRectClip.ulBottom != ulBottom) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set */
		pNewInfo->stDirty.stBits.bAspectRatioClip = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_SetAspectRatioCanvasClip);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetAspectRatioCanvasClip_isr
	( BVDC_Source_Handle               hSource,
	  uint32_t                         ulLeft,
	  uint32_t                         ulRight,
	  uint32_t                         ulTop,
	  uint32_t                         ulBottom )
{
	BVDC_P_Source_IsrInfo *pIsrInfo;

	BDBG_ENTER(BVDC_Source_SetAspectRatioCanvasClip_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_VIDEO(hSource->eId))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	pIsrInfo = &hSource->stIsrInfo;

	/* set isr value */
	pIsrInfo->stAspRatRectClip.ulLeft   = ulLeft;
	pIsrInfo->stAspRatRectClip.ulRight  = ulRight;
	pIsrInfo->stAspRatRectClip.ulTop    = ulTop;
	pIsrInfo->stAspRatRectClip.ulBottom = ulBottom;
	pIsrInfo->stAspRatRectClip.lLeftDelta_R = 0;

	if((hSource->stCurInfo.stAspRatRectClip.ulLeft   != ulLeft)   ||
	   (hSource->stCurInfo.stAspRatRectClip.ulRight  != ulRight)  ||
	   (hSource->stCurInfo.stAspRatRectClip.ulTop    != ulTop)    ||
	   (hSource->stCurInfo.stAspRatRectClip.ulBottom != ulBottom) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set */
		pIsrInfo->stDirty.stBits.bAspectRatioClip = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_SetAspectRatioCanvasClip_isr);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetAdcConfiguration
	( BVDC_Source_Handle               hSource,
	  uint32_t                         ulAdcs,
	  BVDC_AdcInput                    eAdcInput )
{
	int i;

	BDBG_ENTER(BVDC_Source_SetAdcConfiguration);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* Update new inputs for each ADC */
	for(i = 0; i < BVDC_P_MAX_ADC; i++)
	{
		if(ulAdcs & (1 << i))
		{
			BVDC_P_Source_Info *pNewInfo = &hSource->stNewInfo;
			const BVDC_P_Source_Info *pCurInfo = &hSource->stCurInfo;

			/* set new value */
			pNewInfo->aeAdcInput[i] = eAdcInput;

			/* Dirty bit set */
			if((pNewInfo->aeAdcInput[i] != pCurInfo->aeAdcInput[i]) ||
			   (pNewInfo->bErrorLastSetting))
			{
				pNewInfo->stDirty.stBits.bAdcSetting = BVDC_P_DIRTY;
			}
		}
	}

	BDBG_LEAVE(BVDC_Source_SetAdcConfiguration);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetAdcConfiguration
	( const BVDC_Source_Handle         hSource,
	  uint32_t                         ulAdc,
	  BVDC_AdcInput                   *peAdcInput )
{
	int i;

	BDBG_ENTER(BVDC_Source_GetAdcConfiguration);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* Don't allow OR-ed of ADC */
	if(ulAdc > BVDC_Ibafe_0)
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Get index to ADC table */
	for(i = 0;
		((i < BVDC_P_MAX_ADC) && (peAdcInput));
		i++)
	{
		if(ulAdc & (1 << i))
		{
			*peAdcInput = hSource->stCurInfo.aeAdcInput[i];
			break;
		}
	}

	BDBG_LEAVE(BVDC_Source_GetAdcConfiguration);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_Set3DComb
	( BVDC_Source_Handle               hSource,
	  bool                             b3DComb )
{
	BVDC_P_Source_Info *pNewInfo;

	BDBG_ENTER(BVDC_Source_Set3DComb);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pNewInfo = &hSource->stNewInfo;

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		BDBG_ERR(("Source is not VDEC"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* set new value */
	pNewInfo->bEnable3DComb = b3DComb;

	if((hSource->stCurInfo.bEnable3DComb != b3DComb) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set */
		pNewInfo->stDirty.stBits.b3DComb = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_Set3DComb);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_Get3DComb
	( BVDC_Source_Handle                hSource,
	  bool                             *pb3DComb )
{
	BDBG_ENTER(BVDC_Source_Get3DComb);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		BDBG_ERR(("Source is not VDEC"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(pb3DComb)
	{
		*pb3DComb = hSource->stCurInfo.bEnable3DComb;
	}

	BDBG_LEAVE(BVDC_Source_Get3DComb);

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetVdecConfiguration
	( BVDC_Source_Handle               hSource,
	  const BVDC_Vdec_Settings        *pVdecSettings )
{
	BVDC_P_Source_Info *pNewInfo;

	BDBG_ENTER(BVDC_Source_SetVdecConfiguration);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		BDBG_ERR(("Source is not VDEC"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	pNewInfo  = &hSource->stNewInfo;

	/* Take in the customized settings. */
	if(pVdecSettings)
	{
		if(pVdecSettings->pAwc)
		{
			pNewInfo->stAwc = *pVdecSettings->pAwc; /* fe */
		}
		if(pVdecSettings->pCti)
		{
			pNewInfo->stCti = *pVdecSettings->pCti; /* cd */
		}
		if(pVdecSettings->pDcr)
		{
			pNewInfo->stDcr = *pVdecSettings->pDcr; /* fe */
		}
		if(pVdecSettings->pComb)
		{
			pNewInfo->stComb = *pVdecSettings->pComb; /* yc */
		}
		if(pVdecSettings->pAGain)
		{
			pNewInfo->stAGain = *pVdecSettings->pAGain; /* fe */
		}
		if(pVdecSettings->pDGain)
		{
			pNewInfo->stDGain = *pVdecSettings->pDGain; /* fe */
		}
		if(pVdecSettings->pDelay)
		{
			pNewInfo->stDelay = *pVdecSettings->pDelay; /* be */
		}
		if(pVdecSettings->pBackend)
		{
			pNewInfo->stBackend = *pVdecSettings->pBackend; /* be */
		}

		pNewInfo->stVdecSettings = *pVdecSettings;
		pNewInfo->stDirty.stBits.bMiscCtrl = BVDC_P_DIRTY;
		pNewInfo->bCustomCfg = true;
	}
	else
	{
		/* Use/Restore default settings*/
		BKNI_Memset((void*)&pNewInfo->stVdecSettings, 0x0, sizeof(BVDC_Vdec_Settings));
		BVDC_P_Vdec_InitMisc(&pNewInfo->stVdecSettings, pNewInfo);
		pNewInfo->stDirty.stBits.bMiscCtrl = BVDC_P_DIRTY;
		pNewInfo->bCustomCfg = false;
	}

	BDBG_LEAVE(BVDC_Source_SetVdecConfiguration);

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetVdecConfiguration
	( BVDC_Source_Handle               hSource,
	  BVDC_Vdec_Settings              *pVdecSettings )
{
	BDBG_ENTER(BVDC_Source_GetVdecConfiguration);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(pVdecSettings)
	{
		const BVDC_Vdec_Settings *pCurSettings = &hSource->stCurInfo.stVdecSettings;

		/* Clearout the structure to be filled in. */
		BKNI_Memset(pVdecSettings, 0x0, sizeof(BVDC_Vdec_Settings));

		/* Always return these */
		pVdecSettings->bBypassAcgc       = pCurSettings->bBypassAcgc;
		pVdecSettings->bOverSample       = pCurSettings->bOverSample;
		pVdecSettings->bBypassCommonMode = pCurSettings->bBypassCommonMode;
		pVdecSettings->ulBlackOffset     = pCurSettings->ulBlackOffset;

		/* Power user will need to fill it in by themselves. */
		/* pVdecSettings->pXYZ = NULL; */
	}

	BDBG_LEAVE(BVDC_Source_GetVdecConfiguration);

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetVdecDefaultConfiguration
	( BVDC_Source_Handle               hSource,
	  BVDC_Vdec_Settings              *pVdecSettings )
{
#if !(BDBG_DEBUG_BUILD)
	BSTD_UNUSED(pVdecSettings);
#endif

	BDBG_ENTER(BVDC_Source_GetVdecDefaultConfiguration);

	/* Sanity check input parameters as much as possible */
	BDBG_ASSERT(pVdecSettings);

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	BVDC_P_Vdec_GetDefaultSettings(hSource->hVdec, pVdecSettings);

	BDBG_LEAVE(BVDC_Source_GetVdecDefaultConfiguration);

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetInputPort
	( BVDC_Source_Handle               hSource,
	  uint32_t                         ulInputPort )
{
	BVDC_P_Source_Info *pNewInfo;

	BDBG_ENTER(BVDC_Source_SetInputPort);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pNewInfo = &hSource->stNewInfo;

	/* set new value */
	pNewInfo->ulInputPort = ulInputPort;

	if((hSource->stCurInfo.ulInputPort != ulInputPort) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set!  Shared the input format/port  dirty bits */
		pNewInfo->stDirty.stBits.bInputFormat = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_SetInputPort);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetInputPort
	( BVDC_Source_Handle               hSource,
	  uint32_t                        *pulInputPort )
{
	BDBG_ENTER(BVDC_Source_GetInputPort);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(pulInputPort)
	{
		*pulInputPort = hSource->stCurInfo.ulInputPort;
	}

	BDBG_LEAVE(BVDC_Source_GetInputPort);

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetAutoFormat
	( BVDC_Source_Handle               hSource,
	  bool                             bAutoDetect,
	  const BFMT_VideoFmt              aeFormats[],
	  uint32_t                         ulNumFormats )
{
	BVDC_P_Source_Info *pNewInfo;
	uint32_t ulIndex;

	BDBG_ENTER(BVDC_Source_SetAutoFormat);

	BDBG_ASSERT(hSource);
	BDBG_ASSERT(ulNumFormats < BFMT_VideoFmt_eMaxCount);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId) &&
	   !BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		BDBG_ERR(("Source is not VDEC"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	pNewInfo = &hSource->stNewInfo;

	/* set new value */
	pNewInfo->bAutoDetect = bAutoDetect;
	pNewInfo->ulNumFormats = ulNumFormats;

	if((hSource->stCurInfo.bAutoDetect != bAutoDetect)   ||
	   (hSource->stCurInfo.ulNumFormats != ulNumFormats) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set */
		pNewInfo->stDirty.stBits.bAutoDetectFmt = BVDC_P_DIRTY;
	}

	for(ulIndex = 0; ulIndex < ulNumFormats; ulIndex++)
	{
		pNewInfo->aeFormats[ulIndex] = aeFormats[ulIndex];
		if((hSource->stCurInfo.aeFormats[ulIndex] != aeFormats[ulIndex]) ||
		   (hSource->stNewInfo.bErrorLastSetting))
		{
			pNewInfo->stDirty.stBits.bAutoDetectFmt = BVDC_P_DIRTY;
		}
	}

	BDBG_LEAVE(BVDC_Source_SetAutoFormat);

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetDnrConfiguration
	( BVDC_Source_Handle               hSource,
	  const BVDC_Dnr_Settings         *pDnrSettings )
{
#if (!BVDC_P_SUPPORT_DNR)
	BSTD_UNUSED(hSource);
	BSTD_UNUSED(pDnrSettings);
	BDBG_WRN(("DNR is not supported for this chipset"));
#else
	BDBG_ENTER(BVDC_Source_SetDnrConfiguration);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if((pDnrSettings->iBnrLevel <= (-1) * BVDC_QP_ADJUST_STEPS) ||
	   (pDnrSettings->iMnrLevel <= (-1) * BVDC_QP_ADJUST_STEPS) ||
	   (pDnrSettings->iDcrLevel <= (-1) * BVDC_QP_ADJUST_STEPS))
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* only support mvd/xvd source; */
	if((pDnrSettings->eMnrMode != BVDC_FilterMode_eDisable) ||
	   (pDnrSettings->eBnrMode != BVDC_FilterMode_eDisable) ||
	   (pDnrSettings->eDcrMode != BVDC_FilterMode_eDisable))
	{
		hSource->stNewInfo.bDnr = true;
	}
	else
	{
		hSource->stNewInfo.bDnr = false;
	}

	/* set new value */
	hSource->stNewInfo.stDnrSettings = *pDnrSettings;
	hSource->stNewInfo.stDirty.stBits.bDnrAdjust = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Source_SetDnrConfiguration);
#endif

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetDnrConfiguration
	( const BVDC_Source_Handle          hSource,
	  BVDC_Dnr_Settings                *pDnrSettings )
{
#if (!BVDC_P_SUPPORT_DNR)
	BSTD_UNUSED(hSource);
	BSTD_UNUSED(pDnrSettings);
	BDBG_WRN(("DNR is not supported for this chipset"));
#else
	BDBG_ENTER(BVDC_Source_GetDnrConfiguration);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* set new value */
	if(pDnrSettings)
	{
		*pDnrSettings = hSource->stCurInfo.stDnrSettings;
	}

	BDBG_LEAVE(BVDC_Source_GetDnrConfiguration);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetSplitScreenMode
	( BVDC_Source_Handle                      hSource,
	  const BVDC_Source_SplitScreenSettings  *pSplitScreen )
{
	BVDC_P_Source_Info *pNewInfo;

	BDBG_ENTER(BVDC_Source_SetSplitScreenMode);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	pNewInfo  = &hSource->stNewInfo;

	pNewInfo->stSplitScreenSetting.eDnr = pSplitScreen->eDnr;
	if((hSource->stCurInfo.stSplitScreenSetting.eDnr != pSplitScreen->eDnr) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		pNewInfo->stDirty.stBits.bDnrAdjust = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_SetSplitScreenMode);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetSplitScreenMode
	( const BVDC_Source_Handle          hSource,
	  BVDC_Source_SplitScreenSettings  *pSplitScreen )
{
	BDBG_ENTER(BVDC_Source_GetSplitScreenMode);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(pSplitScreen)
	{
		*pSplitScreen = hSource->stCurInfo.stSplitScreenSetting;
	}

	BDBG_LEAVE(BVDC_Source_GetSplitScreenMode);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetHdDviConfiguration
	( const BVDC_Source_Handle         hSource,
	  BVDC_HdDvi_Settings             *pHdDviSettings )
{
	BDBG_ENTER(BVDC_Source_GetHdDviConfiguration);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		BDBG_ERR(("Source is not HD_DVI"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}

	if(pHdDviSettings)
	{
		*pHdDviSettings = hSource->stCurInfo.stHdDviSetting;
	}

	BDBG_LEAVE(BVDC_Source_GetHdDviConfiguration);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetHdDviConfiguration
	( BVDC_Source_Handle               hSource,
	  const BVDC_HdDvi_Settings       *pHdDviSettings )
{
	BDBG_ENTER(BVDC_Source_SetHdDviConfiguration);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		BDBG_ERR(("Source is not HD_DVI"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}
	else
	{
		BVDC_P_Source_Info *pNewInfo = &hSource->stNewInfo;
		BVDC_P_Source_Info *pCurInfo = &hSource->stCurInfo;
		BVDC_P_Source_DirtyBits *pNewDirty = &pNewInfo->stDirty;
		BVDC_HdDvi_Settings *pCurSetting = &pCurInfo->stHdDviSetting;

		/* Enable or disable DE, data mode */
		pNewInfo->stHdDviSetting = *pHdDviSettings;
		if((pCurSetting->bEnableDe != pHdDviSettings->bEnableDe) ||
		   (pCurSetting->eInputDataMode != pHdDviSettings->eInputDataMode) ||
		   (pCurSetting->stFmtTolerence.ulWidth  != pHdDviSettings->stFmtTolerence.ulWidth)  ||
		   (pCurSetting->stFmtTolerence.ulHeight != pHdDviSettings->stFmtTolerence.ulHeight) ||
		   (hSource->stNewInfo.bErrorLastSetting))
		{
			pNewDirty->stBits.bMiscCtrl = BVDC_P_DIRTY;
		}
	}

	BDBG_LEAVE(BVDC_Source_SetHdDviConfiguration);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetSyncConfiguration
	( BVDC_Source_Handle               hSource,
	  const BVDC_Source_SyncSettings  *pSyncSettings )
{
	BDBG_ENTER(BVDC_Source_SetSyncConfiguration);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		BDBG_ERR(("Source is not VDEC"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}
	else
	{
		BVDC_P_Source_Info *pNewInfo = &hSource->stNewInfo;
		BVDC_P_Source_Info *pCurInfo = &hSource->stCurInfo;
		BVDC_P_Source_DirtyBits *pNewDirty = &pNewInfo->stDirty;

		/* set new value */

		/* auto/manual clock adjustment */
		pNewInfo->stPcInSyncSettings.bManualClock =
			pSyncSettings->bManualClock;
		if(pCurInfo->stPcInSyncSettings.bManualClock !=
		    pSyncSettings->bManualClock)
		{
			pNewDirty->stBits.bManualClk = BVDC_P_DIRTY;
		}

		if(pSyncSettings->bManualClock)
		{
			pNewInfo->stPcInSyncSettings.lClockAdj =
				pSyncSettings->lClockAdj;
			if(pCurInfo->stPcInSyncSettings.lClockAdj !=
			    pSyncSettings->lClockAdj)
			{
				pNewDirty->stBits.bManualClk = BVDC_P_DIRTY;
			}
		}
		else /* reset to restore the norminal clock */
		{
			pNewInfo->stPcInSyncSettings.lClockAdj = 0;
		}

		/* auto/manual phase adjustment */
		pNewInfo->stPcInSyncSettings.bManualPhase =
			pSyncSettings->bManualPhase;
		if(pCurInfo->stPcInSyncSettings.bManualPhase !=
		    pSyncSettings->bManualPhase)
		{
			pNewDirty->stBits.bManualClk = BVDC_P_DIRTY;
		}

		if(pSyncSettings->bManualPhase)
		{
			uint32_t ulIndex;
			for(ulIndex = 0; ulIndex < BVDC_COLOR_CHANNELS; ulIndex++)
			{
				pNewInfo->stPcInSyncSettings.aulPhase[ulIndex] =
					pSyncSettings->aulPhase[ulIndex];
				if(pCurInfo->stPcInSyncSettings.aulPhase[ulIndex] !=
					pSyncSettings->aulPhase[ulIndex])
				{
					pNewDirty->stBits.bManualClk = BVDC_P_DIRTY;
				}
			}
		}

#if (BVDC_P_SUPPORT_VDEC_VER >= BVDC_P_VDEC_VER_4)
		/* auto/manual position adjustment */
		pNewInfo->stPcInSyncSettings.bManualPosition =
			pSyncSettings->bManualPosition;
		if(pCurInfo->stPcInSyncSettings.bManualPosition !=
		    pSyncSettings->bManualPosition)
		{
			pNewDirty->stBits.bManualPos = BVDC_P_DIRTY;
		}
#endif

		/* hysteresis */
		pNewInfo->stPcInSyncSettings.bOverrideHysteresis =
			pSyncSettings->bOverrideHysteresis;
		if(pCurInfo->stPcInSyncSettings.bOverrideHysteresis!=
		    pSyncSettings->bOverrideHysteresis)
		{
			pNewDirty->stBits.bManualClk = BVDC_P_DIRTY;
		}

		if(pSyncSettings->bOverrideHysteresis)
		{
			pNewInfo->stPcInSyncSettings.ulHysteresis =
				pSyncSettings->ulHysteresis;
			if(pCurInfo->stPcInSyncSettings.ulHysteresis!=
			    pSyncSettings->ulHysteresis)
			{
				pNewDirty->stBits.bManualClk = BVDC_P_DIRTY;
			}
		}
		/* manual type */
		pNewInfo->stPcInSyncSettings.bOverrideManualType =
			pSyncSettings->bOverrideManualType;
		if(pCurInfo->stPcInSyncSettings.bOverrideManualType!=
		    pSyncSettings->bOverrideManualType)
		{
			pNewDirty->stBits.bManualClk = BVDC_P_DIRTY;
		}

		if(pSyncSettings->bOverrideManualType)
		{
			pNewInfo->stPcInSyncSettings.bMultiVsync =
				pSyncSettings->bMultiVsync;
			if(pCurInfo->stPcInSyncSettings.bMultiVsync!=
			    pSyncSettings->bMultiVsync)
			{
				pNewDirty->stBits.bManualClk = BVDC_P_DIRTY;
			}
		}
	}

	BDBG_LEAVE(BVDC_Source_SetSyncConfiguration);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetSyncConfiguration
	( const BVDC_Source_Handle         hSource,
	  BVDC_Source_SyncSettings        *pSyncSettings )
{
	BDBG_ENTER(BVDC_Source_GetSyncConfiguration);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		BDBG_ERR(("Source is not VDEC"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}

	if(pSyncSettings)
	{
		*pSyncSettings = hSource->stCurInfo.stPcInSyncSettings;
	}

	BDBG_LEAVE(BVDC_Source_GetSyncConfiguration);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetSyncStatus
	( const BVDC_Source_Handle         hSource,
	  BVDC_Source_SyncStatus          *pSyncStatus )
{
	BDBG_ENTER(BVDC_Source_GetSyncStatus);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId) ||
	   !hSource->stCurInfo.bPcInput)
	{
		BDBG_ERR(("Source is not PC input"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}

	if(pSyncStatus)
	{
		BVDC_P_Vdec_GetPcInSyncStatus(hSource->hVdec, pSyncStatus);
	}

	BDBG_LEAVE(BVDC_Source_GetSyncStatus);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetHVStart
	( BVDC_Source_Handle               hSource,
	  bool                             bOverrideVdc,
	  uint32_t                         ulHStart,
	  uint32_t                         ulVStart )
{
	BDBG_ENTER(BVDC_Source_SetHVStart);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId) && !BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		BDBG_ERR(("Source is not VDEC or HD_DVI"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}
	else
	{
		BVDC_P_Source_Info *pNewInfo = &hSource->stNewInfo;
		BVDC_P_Source_Info *pCurInfo = &hSource->stCurInfo;
		BVDC_P_Source_DirtyBits *pNewDirty = &pNewInfo->stDirty;

		/* set new value */
		/* auto/manual clock adjustment */
		pNewInfo->bHVStartOverride = bOverrideVdc;
		if(pCurInfo->bHVStartOverride != bOverrideVdc)
		{
			pNewDirty->stBits.bManualPos = BVDC_P_DIRTY;
		}

		if(bOverrideVdc)
		{
			pNewInfo->ulHstart = ulHStart;
			pNewInfo->ulVstart = ulVStart;
 			if((pCurInfo->ulHstart != ulHStart) ||
			   (pCurInfo->ulVstart != ulVStart))
			{
				pNewDirty->stBits.bManualPos = BVDC_P_DIRTY;
			}
		}
	}

	BDBG_LEAVE(BVDC_Source_SetHVStart);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_Source_GetHVStart
	( BVDC_Source_Handle               hSource,
	  bool                            *pbOverrideVdc,
	  uint32_t                        *pulHStart,
	  uint32_t                        *pulVStart,
	  bool                             bGetDefault )
{
	BDBG_ENTER(BVDC_Source_GetHVStart);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId) && !BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		BDBG_ERR(("Source is not VDEC or HD_DVI"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}

	if(pbOverrideVdc)
	{
		*pbOverrideVdc = hSource->stCurInfo.bHVStartOverride;
	}

	if((hSource->stCurInfo.bHVStartOverride) && (!bGetDefault))
	{
		if(pulHStart)
		{
			*pulHStart = hSource->stCurInfo.ulHstart;
		}

		if(pulVStart)
		{
			*pulVStart = hSource->stCurInfo.ulVstart;
		}
	}
	else
	{
		if(BVDC_P_SRC_IS_ANALOG(hSource->eId))
		{
			if(pulHStart)
			{
				/*get ulHstart directly. Is it right?*/
				*pulHStart = hSource->stCurInfo.pFdEntry->ulHStart;
			}

			if(pulVStart)
			{
				/*get ulVstart directly. Is it right?*/
				*pulVStart = hSource->stCurInfo.pFmtInfo->ulTopActive;
			}
		}
		else if(BVDC_P_SRC_IS_HDDVI(hSource->eId))
		{
			if(pulHStart)
			{
				*pulHStart = hSource->hHdDvi->ulHorzDelay;
			}

			if(pulVStart)
			{
				*pulVStart = hSource->hHdDvi->ulVertDelay;
			}
		}
	}

	BDBG_LEAVE(BVDC_Source_GetHVStart);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetHVStart
	( BVDC_Source_Handle               hSource,
	  bool                            *pbOverrideVdc,
	  uint32_t                        *pulHStart,
	  uint32_t                        *pulVStart )
{
	return BVDC_P_Source_GetHVStart(hSource, pbOverrideVdc, pulHStart, pulVStart,
		false);
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetDefaultHVStart
	( BVDC_Source_Handle               hSource,
	  uint32_t                        *pulHStart,
	  uint32_t                        *pulVStart )
{
	return BVDC_P_Source_GetHVStart(hSource, NULL, pulHStart, pulVStart, true);
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetOrientation
	( BVDC_Source_Handle               hSource,
	  bool                             bOverride,
	  BFMT_Orientation                 eOrientation )
{
	BDBG_ENTER(BVDC_Source_SetOrientation);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_HDDVI(hSource->eId) && !BVDC_P_SRC_IS_MPEG(hSource->eId) &&
	   !BVDC_P_SRC_IS_GFX(hSource->eId))
	{
		BDBG_ERR(("Source is not HD_DVI or MPEG or GFX"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}
	else
	{
		BVDC_P_Source_Info *pNewInfo = &hSource->stNewInfo;
		BVDC_P_Source_Info *pCurInfo = &hSource->stCurInfo;
		BVDC_P_Source_DirtyBits *pNewDirty = &pNewInfo->stDirty;

		/* set new value */
		pNewInfo->bOrientationOverride = bOverride;
		if(pCurInfo->bOrientationOverride != bOverride)
		{
			pNewDirty->stBits.bOrientation = BVDC_P_DIRTY;
		}

		if(bOverride)
		{
			pNewInfo->eOrientation = eOrientation;
 			if(pCurInfo->eOrientation != eOrientation)
			{
				pNewDirty->stBits.bOrientation = BVDC_P_DIRTY;
			}
		}
	}

	BDBG_LEAVE(BVDC_Source_SetOrientation);
	return BERR_SUCCESS;

}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetOrientation
	( BVDC_Source_Handle               hSource,
	  bool                            *pbOverride,
	  BFMT_Orientation                *peOrientation )
{
	BDBG_ENTER(BVDC_Source_GetOrientation);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_HDDVI(hSource->eId) && !BVDC_P_SRC_IS_MPEG(hSource->eId) &&
	   !BVDC_P_SRC_IS_GFX(hSource->eId))
	{
		BDBG_ERR(("Source is not HD_DVI or MPEG or GFX"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}

	if(pbOverride)
	{
		*pbOverride = hSource->stCurInfo.bOrientationOverride;
	}

	if(peOrientation)
	{
		*peOrientation = hSource->stCurInfo.eOrientation;
	}

	BDBG_LEAVE(BVDC_Source_GetOrientation);
	return BERR_SUCCESS;

}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetColorMatrix
	( BVDC_Source_Handle               hSource,
	  bool                             bOverride,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift )
{
	BDBG_ENTER(BVDC_Source_SetColorMatrix);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId) &&
	   !BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		BDBG_ERR(("Source is not VDEC or HDDVI"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}
	else
	{
		BVDC_P_Source_Info *pNewInfo = &hSource->stNewInfo;
		BVDC_P_Source_DirtyBits *pNewDirty = &pNewInfo->stDirty;

		/* set new value */
		pNewInfo->bUserCsc = bOverride;
		if(bOverride)
		{
			uint32_t ulIndex;
			pNewInfo->ulUserShift = ulShift;
			for(ulIndex = 0; ulIndex < BVDC_CSC_COEFF_COUNT; ulIndex++)
			{
				pNewInfo->pl32_Matrix[ulIndex] = pl32_Matrix[ulIndex];
			}
		}
		pNewDirty->stBits.bColorspace = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_SetColorMatrix);
	return BERR_SUCCESS;

}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetColorMatrix
	( BVDC_Source_Handle               hSource,
	  bool                            *pbOverride,
	  int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                        *pulShift )
{
	BDBG_ENTER(BVDC_Source_GetColorMatrix);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(pbOverride)
	{
		*pbOverride = hSource->stCurInfo.bUserCsc;
	}

	if(hSource->stCurInfo.bUserCsc)
	{
		uint32_t ulIndex;
		for(ulIndex = 0; ulIndex < BVDC_CSC_COEFF_COUNT; ulIndex++)
		{
			pl32_Matrix[ulIndex] = hSource->stCurInfo.pl32_Matrix[ulIndex];
		}

		if(pulShift)
		{
			*pulShift = hSource->stCurInfo.ulUserShift;
		}
	}
	else
	{
		if(BVDC_P_SRC_IS_ANALOG(hSource->eId))
		{
			BKNI_EnterCriticalSection();
			BVDC_P_Csc_ToMatrix_isr(pl32_Matrix, &hSource->hVdec->stCsc,
				BVDC_P_FIX_POINT_SHIFT);
			BKNI_LeaveCriticalSection();
		}
		else if(BVDC_P_SRC_IS_HDDVI(hSource->eId))
		{
			BKNI_EnterCriticalSection();
			BVDC_P_Csc_ToMatrix_isr(pl32_Matrix, &hSource->hHdDvi->stCsc,
				BVDC_P_FIX_POINT_SHIFT);
			BKNI_LeaveCriticalSection();
		}

		if(pulShift)
		{
			*pulShift = BVDC_P_FIX_POINT_SHIFT;
		}
	}

	BDBG_LEAVE(BVDC_Source_GetColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetFrontendColorMatrix
	( BVDC_Source_Handle               hSource,
	  bool                             bOverride,
	  const int32_t                    pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                         ulShift )
{
	BDBG_ENTER(BVDC_Source_SetFrontendColorMatrix);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		BDBG_ERR(("Source is not VDEC"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}
	else
	{
		BVDC_P_Source_Info *pNewInfo = &hSource->stNewInfo;
		BVDC_P_Source_DirtyBits *pNewDirty = &pNewInfo->stDirty;

		/* set new value */
		pNewInfo->bUserFrontendCsc = bOverride;
		if(bOverride)
		{
			uint32_t ulIndex;
			pNewInfo->ulUserFrontendShift = ulShift;
			for(ulIndex = 0; ulIndex < BVDC_CSC_COEFF_COUNT; ulIndex++)
			{
				pNewInfo->pl32_FrontendMatrix[ulIndex] = pl32_Matrix[ulIndex];
			}
		}
		pNewDirty->stBits.bColorspace = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_SetFrontendColorMatrix);
	return BERR_SUCCESS;

}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetFrontendColorMatrix
	( BVDC_Source_Handle               hSource,
	  bool                            *pbOverride,
	  int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT],
	  uint32_t                        *pulShift )
{
	BDBG_ENTER(BVDC_Source_GetFrontendColorMatrix);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(pbOverride)
	{
		*pbOverride = hSource->stCurInfo.bUserFrontendCsc;
	}

	if(hSource->stCurInfo.bUserFrontendCsc)
	{
		uint32_t ulIndex;
		for(ulIndex = 0; ulIndex < BVDC_CSC_COEFF_COUNT; ulIndex++)
		{
			pl32_Matrix[ulIndex] = hSource->stCurInfo.pl32_FrontendMatrix[ulIndex];
		}

		if(pulShift)
		{
			*pulShift = hSource->stCurInfo.ulUserFrontendShift;
		}
	}
	else
	{
		if(BVDC_P_SRC_IS_ANALOG(hSource->eId))
		{
			BKNI_EnterCriticalSection();
			BVDC_P_Csc_ToMatrix_isr(pl32_Matrix, &hSource->hVdec->stFrontendCsc,
				BVDC_P_FIX_POINT_SHIFT);
			BKNI_LeaveCriticalSection();
		}
		/* else * return unmodified structure */

		if(pulShift)
		{
			*pulShift = BVDC_P_FIX_POINT_SHIFT;
		}
	}

	BDBG_LEAVE(BVDC_Source_GetFrontendColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetInputStatus
	( const BVDC_Source_Handle         hSource,
	  BVDC_Source_InputStatus         *pInputStatus )
{
	BDBG_ENTER(BVDC_Source_GetInputStatus);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_HDDVI(hSource->eId) &&
	   !BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		BDBG_ERR(("Source is not PC, Component or HD_DVI input"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}

	if(pInputStatus)
	{
		/* To make sure thing get initialize */
		BKNI_Memset(pInputStatus, 0, sizeof(*pInputStatus));

		if(BVDC_P_SRC_IS_ANALOG(hSource->eId))
		{
			BVDC_P_Vdec_GetInputStatus(hSource->hVdec, pInputStatus);
		}
		else if(BVDC_P_SRC_IS_HDDVI(hSource->eId))
		{
			BVDC_P_HdDvi_GetInputStatus(hSource->hHdDvi, pInputStatus);
		}
	}

	BDBG_LEAVE(BVDC_Source_GetInputStatus);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetPsfMode
	( BVDC_Source_Handle               hSource,
	  bool                             bEnable )
{
	BDBG_ENTER(BVDC_Source_SetPsfMode);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_MPEG(hSource->eId))
	{
		BDBG_ERR(("Source is not MPEG input"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}

	hSource->stNewInfo.bPsfEnable = bEnable;
	if(hSource->stCurInfo.bPsfEnable != bEnable)
	{
		/* Dirty bit set */
		hSource->stNewInfo.stDirty.stBits.bPsfMode = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_Source_SetPsfMode);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetPsfMode
	( BVDC_Source_Handle               hSource,
	  bool                            *pbEnable )
{
	BDBG_ENTER(BVDC_Source_GetPsfMode);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_MPEG(hSource->eId))
	{
		BDBG_ERR(("Source is not MPEG input"));
		return BERR_TRACE(BVDC_ERR_BAD_SRC_TYPE);
	}

	if(pbEnable)
	{
		*pbEnable = hSource->stCurInfo.bPsfEnable;
	}

	BDBG_LEAVE(BVDC_Source_GetPsfMode);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetReMapFormats
	( BVDC_Source_Handle               hSource,
	  bool                             bReMap,
	  const BVDC_VideoFmtPair          aReMapFormats[],
	  uint32_t                         ulReMapFormats )
{
	uint32_t i;
	BVDC_P_Source_Info *pNewInfo;

	BDBG_ENTER(BVDC_Source_SetReMapFormats);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(!BVDC_P_SRC_IS_ANALOG(hSource->eId) &&
	   !BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		BDBG_ERR(("Source is not VDEC/HD_DVI"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	if(ulReMapFormats >= BFMT_VideoFmt_eMaxCount)
	{
		BDBG_ERR(("Out of bound ulReMapFormats = %d", ulReMapFormats));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	pNewInfo = &hSource->stNewInfo;

	/* set new value */
	pNewInfo->bReMapFormats = bReMap;
	pNewInfo->ulReMapFormats = ulReMapFormats;

	if((bReMap) && (ulReMapFormats))
	{
		for(i = 0; i < ulReMapFormats; i++)
		{
			pNewInfo->aReMapFormats[i] = aReMapFormats[i];
		}
	}

	/* Dirty bit set */
	pNewInfo->stDirty.stBits.bAutoDetectFmt = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_Source_SetReMapFormats);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_InstallPictureCallback
	( BVDC_Source_Handle               hSource,
	  BVDC_Source_PictureCallback_isr  pfSrcCallback,
	  void                            *pvParm1,
	  int                              iParm2 )
{
	BVDC_P_Source_Info *pNewInfo;

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pNewInfo = &hSource->stNewInfo;

	if((!BVDC_P_SRC_IS_HDDVI(hSource->eId)) &&
	   (!BVDC_P_SRC_IS_GFX(hSource->eId)) &&
	   (!BVDC_P_SRC_IS_VFD(hSource->eId)) &&
	   (!(BVDC_P_SRC_IS_MPEG(hSource->eId) && hSource->hMpegFeeder->bGfxSrc)))
	{
		BDBG_ERR(("Source is not HD_DVI, VFD, or GFX"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* set new info and dirty bits. */
	pNewInfo->pfPicCallbackFunc = pfSrcCallback;
	pNewInfo->pvParm1 = pvParm1;
	pNewInfo->iParm2 = iParm2;

	if((hSource->stCurInfo.pfPicCallbackFunc != pfSrcCallback) ||
	   (hSource->stCurInfo.pvParm1 != pvParm1) ||
	   (hSource->stCurInfo.iParm2 != iParm2)   ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set */
		pNewInfo->stDirty.stBits.bPicCallback = BVDC_P_DIRTY;
	}

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_InstallCallback
	( BVDC_Source_Handle               hSource,
	  const BVDC_CallbackFunc_isr      pfCallback,
	  void                            *pvParm1,
	  int                              iParm2 )
{
	BVDC_P_Source_Info *pNewInfo;

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pNewInfo = &hSource->stNewInfo;

	/* set new info and dirty bits. */
	pNewInfo->pfGenericCallback = pfCallback;
	pNewInfo->pvGenericParm1 = pvParm1;
	pNewInfo->iGenericParm2 = iParm2;

	if((hSource->stCurInfo.pfGenericCallback != pfCallback) ||
	   (hSource->stCurInfo.pvGenericParm1 != pvParm1) ||
	   (hSource->stCurInfo.iGenericParm2 != iParm2)   ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set */
		pNewInfo->stDirty.stBits.bGenCallback = BVDC_P_DIRTY;
	}

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetResumeMode
	( BVDC_Source_Handle               hSource,
	  BVDC_ResumeMode                  eResumeMode )
{
	BVDC_P_Source_Info *pNewInfo;

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pNewInfo = &hSource->stNewInfo;

	/* set new info and dirty bits. */
	pNewInfo->eResumeMode = eResumeMode;

	if((hSource->stCurInfo.eResumeMode != eResumeMode) ||
	   (hSource->stNewInfo.bErrorLastSetting))
	{
		/* Dirty bit set */
		pNewInfo->stDirty.stBits.bUserChanges = BVDC_P_DIRTY;
	}

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetResumeMode
	( BVDC_Source_Handle               hSource,
	  BVDC_ResumeMode                 *peResumeMode )
{
	BDBG_ENTER(BVDC_Source_GetResumeMode);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(peResumeMode)
	{
		*peResumeMode = hSource->stCurInfo.eResumeMode;
	}

	BDBG_LEAVE(BVDC_Source_GetResumeMode);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_Resume
	( BVDC_Source_Handle               hSource )
{
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* set new info and dirty bits. */
	hSource->stNewInfo.stDirty.stBits.bResume = BVDC_P_DIRTY;

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Source_ForcePending
	( BVDC_Source_Handle               hSource )
{
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* set new info and dirty bits. */
	hSource->stNewInfo.bForceSrcPending = true;

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_GetCapabilities
	( const BVDC_Source_Handle         hSource,
	  BVDC_Source_Capabilities        *pCapabilities )
{
	BSTD_UNUSED(hSource);

	if(pCapabilities)
	{
		/* To make sure thing get initialize */
		BKNI_Memset(pCapabilities, 0, sizeof(*pCapabilities));

		pCapabilities->pfIsPxlfmtSupported = BVDC_P_IsPxlfmtSupported;
	}

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetSurface
	( BVDC_Source_Handle      hSource,
	 const BAVC_Gfx_Picture  *pAvcGfxPic)
{
	BERR_Code  eStatus = BERR_SUCCESS;
	BPXL_Format ePxlFmt;

	BDBG_ENTER(BVDC_Source_SetSurface);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_ASSERT(NULL != pAvcGfxPic);

	BKNI_EnterCriticalSection();
	BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs);
	hSource->hVdc->ulInsideCs ++;

	if (NULL != hSource->hGfxFeeder && NULL != pAvcGfxPic)
	{
#ifdef BVDC_P_SUPPORT_OLD_SET_ALPHA_SUR
		if (hSource->hGfxFeeder->stTmpNewAvcPic.hAlphaSurface)
		{
			/* old BVDC_Source_SetAlpha() has been called */
			hSource->hGfxFeeder->stTmpNewAvcPic.hSurface = pAvcGfxPic->hSurface;
			BVDC_P_GfxSurface_SetSurface_isr(
				&hSource->hGfxFeeder->stGfxSurface,
				&hSource->hGfxFeeder->stGfxSurface.stNewSurInfo,
				&hSource->hGfxFeeder->stTmpNewAvcPic);
		}
		else
		{
			eStatus = BVDC_P_GfxSurface_SetSurface_isr(
				&hSource->hGfxFeeder->stGfxSurface,
				&hSource->hGfxFeeder->stGfxSurface.stNewSurInfo, pAvcGfxPic);
		}
#else
		eStatus = BVDC_P_GfxSurface_SetSurface_isr(
			&hSource->hGfxFeeder->stGfxSurface,
			&hSource->hGfxFeeder->stGfxSurface.stNewSurInfo, pAvcGfxPic);
#endif
	}
	else if (NULL != hSource->hVfdFeeder && NULL != pAvcGfxPic)
	{
		eStatus = BSUR_Surface_GetFormat( pAvcGfxPic->hSurface, &ePxlFmt );
		if (BERR_SUCCESS != eStatus || !BPXL_IS_YCbCr422_FORMAT(ePxlFmt) ||
			NULL!=pAvcGfxPic->hAlphaSurface)
		{
			BDBG_ERR(("Invalid pixel format set to vfd"));
			eStatus = BERR_INVALID_PARAMETER;
		}
		else
		{
			eStatus = BVDC_P_GfxSurface_SetSurface_isr(
				&hSource->hVfdFeeder->stGfxSurface,
				&hSource->hVfdFeeder->stGfxSurface.stNewSurInfo, pAvcGfxPic);
		}
	}
	else if (NULL != hSource->hMpegFeeder && NULL != pAvcGfxPic)
	{
		eStatus = BSUR_Surface_GetFormat( pAvcGfxPic->hSurface, &ePxlFmt );
		if (BERR_SUCCESS != eStatus || !BPXL_IS_YCbCr422_FORMAT(ePxlFmt) ||
			NULL!=pAvcGfxPic->hAlphaSurface)
		{
			BDBG_ERR(("Invalid pixel format set to vfd"));
			eStatus = BERR_INVALID_PARAMETER;
		}
		else
		{
			eStatus = BVDC_P_GfxSurface_SetSurface_isr(
				&hSource->hMpegFeeder->stGfxSurface,
				&hSource->hMpegFeeder->stGfxSurface.stNewSurInfo, pAvcGfxPic);
		}
	}
	else
	{
		BDBG_ERR(("Invalid source handle or gfx pic to set gfx surface."));
		eStatus = BERR_INVALID_PARAMETER;
	}

	hSource->hVdc->ulInsideCs --;
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVDC_Source_SetSurface);
	return BERR_TRACE(eStatus);
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Source_SetSurface_isr
	( BVDC_Source_Handle      hSource,
	 const BAVC_Gfx_Picture  *pAvcGfxPic)
{
	BPXL_Format ePxlFmt;
	BERR_Code  eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Source_SetSurface_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_ASSERT(NULL != pAvcGfxPic);

	/* possible to be called inside display callback func, so don't assert:
	   BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs); */
	hSource->hVdc->ulInsideCs ++;

	if (NULL != hSource->hGfxFeeder && NULL != pAvcGfxPic)
	{
#ifdef BVDC_P_SUPPORT_OLD_SET_ALPHA_SUR
		if (hSource->hGfxFeeder->stTmpIsrAvcPic.hAlphaSurface)
		{
			/* old BVDC_Source_SetAlpha() has been called */
			hSource->hGfxFeeder->stTmpIsrAvcPic.hSurface = pAvcGfxPic->hSurface;
			BVDC_P_GfxSurface_SetSurface_isr(
				&hSource->hGfxFeeder->stGfxSurface,
				&hSource->hGfxFeeder->stGfxSurface.stIsrSurInfo,
				&hSource->hGfxFeeder->stTmpIsrAvcPic);
		}
		else
		{
			eStatus = BVDC_P_GfxSurface_SetSurface_isr(
				&hSource->hGfxFeeder->stGfxSurface,
				&hSource->hGfxFeeder->stGfxSurface.stIsrSurInfo, pAvcGfxPic);
		}
#else
		eStatus = BVDC_P_GfxSurface_SetSurface_isr(
			&hSource->hGfxFeeder->stGfxSurface,
			&hSource->hGfxFeeder->stGfxSurface.stIsrSurInfo, pAvcGfxPic);
#endif
	}
	else if (NULL != hSource->hVfdFeeder && NULL != pAvcGfxPic)
	{
		eStatus = BSUR_Surface_GetFormat( pAvcGfxPic->hSurface, &ePxlFmt );
		if (BERR_SUCCESS != eStatus || !BPXL_IS_YCbCr422_FORMAT(ePxlFmt) ||
			NULL!=pAvcGfxPic->hAlphaSurface)
		{
			BDBG_ERR(("Invalid pixel format set to vfd"));
			eStatus = BERR_INVALID_PARAMETER;
		}
		else
		{
			eStatus = BVDC_P_GfxSurface_SetSurface_isr(
				&hSource->hVfdFeeder->stGfxSurface,
				&hSource->hVfdFeeder->stGfxSurface.stIsrSurInfo, pAvcGfxPic);
		}
	}
	else if (NULL != hSource->hMpegFeeder && NULL != pAvcGfxPic)
	{
		eStatus = BSUR_Surface_GetFormat( pAvcGfxPic->hSurface, &ePxlFmt );
		if (BERR_SUCCESS != eStatus || !BPXL_IS_YCbCr422_FORMAT(ePxlFmt) ||
			NULL!=pAvcGfxPic->hAlphaSurface)
		{
			BDBG_ERR(("Invalid pixel format set to vfd"));
			eStatus = BERR_INVALID_PARAMETER;
		}
		else
		{
			eStatus = BVDC_P_GfxSurface_SetSurface_isr(
				&hSource->hMpegFeeder->stGfxSurface,
				&hSource->hMpegFeeder->stGfxSurface.stIsrSurInfo, pAvcGfxPic);
		}
	}
	else
	{
		BDBG_ERR(("Invalid source handle or gfx pic to set gfx surface."));
		eStatus = BERR_INVALID_PARAMETER;
	}

	hSource->hVdc->ulInsideCs --;

	BDBG_LEAVE(BVDC_Source_SetSurface_isr);
	return BERR_TRACE(eStatus);
}

/***************************************************************************
*
*/
BERR_Code BVDC_Source_GetSurface
	( BVDC_Source_Handle         hSource,
	 BAVC_Gfx_Picture           *pAvcGfxPic)
{
	BERR_Code  eStatus = BERR_SUCCESS;
	BAVC_Gfx_Picture  *pGfxPic;

	BDBG_ENTER(BVDC_Source_GetSurface);

	if(NULL != pAvcGfxPic)
	{
		BKNI_EnterCriticalSection();
		BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs);
		hSource->hVdc->ulInsideCs ++;

		if (NULL != hSource->hGfxFeeder)
		{
			pGfxPic = BVDC_P_GfxSurface_GetSurfaceInHw_isr(
				&hSource->hGfxFeeder->stGfxSurface);
			*pAvcGfxPic = *pGfxPic;
		}
		else if (NULL != hSource->hVfdFeeder)
		{
			pGfxPic = BVDC_P_GfxSurface_GetSurfaceInHw_isr(
				&hSource->hVfdFeeder->stGfxSurface);
			*pAvcGfxPic = *pGfxPic;
		}
		else if (NULL != hSource->hMpegFeeder)
		{
			pGfxPic = BVDC_P_GfxSurface_GetSurfaceInHw_isr(
				&hSource->hMpegFeeder->stGfxSurface);
			*pAvcGfxPic = *pGfxPic;
		}
		else
		{
			BDBG_ERR(("Invalid source handle to get gfx surface."));
			eStatus = BERR_INVALID_PARAMETER;
		}

		hSource->hVdc->ulInsideCs--;
		BKNI_LeaveCriticalSection();
	}

	BDBG_LEAVE(BVDC_Source_GetSurface);
	return BERR_TRACE(eStatus);
}

/***************************************************************************
*
*/
BERR_Code BVDC_Source_GetSurface_isr
	( BVDC_Source_Handle         hSource,
	 BAVC_Gfx_Picture           *pAvcGfxPic)
{
	BERR_Code  eStatus = BERR_SUCCESS;
	BAVC_Gfx_Picture  *pGfxPic;

	BDBG_ENTER(BVDC_Source_GetSurface_isr);

	if(NULL != pAvcGfxPic)
	{
		/* possible to be called inside display callback func, so don't assert:
		   BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs); */
		hSource->hVdc->ulInsideCs ++;

		if (NULL != hSource->hGfxFeeder)
		{
			pGfxPic = BVDC_P_GfxSurface_GetSurfaceInHw_isr(
				&hSource->hGfxFeeder->stGfxSurface);
			*pAvcGfxPic = *pGfxPic;
		}
		else if (NULL != hSource->hVfdFeeder)
		{
			pGfxPic = BVDC_P_GfxSurface_GetSurfaceInHw_isr(
				&hSource->hVfdFeeder->stGfxSurface);
			*pAvcGfxPic = *pGfxPic;
		}
		else if (NULL != hSource->hMpegFeeder)
		{
			pGfxPic = BVDC_P_GfxSurface_GetSurfaceInHw_isr(
				&hSource->hMpegFeeder->stGfxSurface);
			*pAvcGfxPic = *pGfxPic;
		}
		else
		{
			BDBG_ERR(("Invalid source handle to get gfx surface."));
			eStatus = BERR_INVALID_PARAMETER;
		}

		hSource->hVdc->ulInsideCs--;
	}

	BDBG_LEAVE(BVDC_Source_GetSurface_isr);
	return BERR_TRACE(eStatus);
}

#if (BDBG_DEBUG_BUILD)
/***************************************************************************
 *
 */
static void BVDC_P_Source_PrintPicture
	( const BVDC_Source_Handle         hSource,
	  const BAVC_MVD_Field            *pCurPic,
	  const BAVC_MVD_Field            *pNewPic )
{
	uint32_t ulMsgCount;
	uint32_t ulPrintOnDelta;
	uint32_t ulDebugReg = BVDC_P_MPEG_DEBUG_SCRATCH(hSource->eId);

	ulMsgCount = BREG_Read32_isr(hSource->hVdc->hRegister, ulDebugReg);

	/* bit 1: print on mpeg-pic-delta;
	 * other 31-bit: message count; */
	ulPrintOnDelta = ulMsgCount & 1;

	/* Check if different from previous field. */
	if(ulPrintOnDelta)
	{
		/* If interlaced make sure the polarity are not repeat. */
		if((false == pNewPic->bMute) &&
		   (BAVC_Polarity_eFrame != pNewPic->eSourcePolarity) &&
		   (pNewPic->eSourcePolarity == pCurPic->eSourcePolarity))
		{
			BDBG_ERR(("*** XDM sends repeated buffer (%s)!",
				(BAVC_Polarity_eTopField == pNewPic->eSourcePolarity) ? "T" :
				(BAVC_Polarity_eBotField == pNewPic->eSourcePolarity) ? "B" : "F"));
		}
		ulPrintOnDelta = (uint32_t)hSource->bPictureChanged ||
			BVDC_P_FIELD_DIFF(pNewPic, pCurPic, bStreamProgressive) ||
			BVDC_P_FIELD_DIFF(pNewPic, pCurPic, eMpegType) ||
			BVDC_P_FIELD_DIFF(pNewPic, pCurPic, eYCbCrType) ||
			BVDC_P_FIELD_DIFF(pNewPic, pCurPic, eAspectRatio) ||
			BVDC_P_FIELD_DIFF(pNewPic, pCurPic, eFrameRateCode) ||
			BVDC_P_FIELD_DIFF(pNewPic, pCurPic, eMatrixCoefficients) ||
			BVDC_P_FIELD_DIFF(pNewPic, pCurPic, eChrominanceInterpolationMode);
	}

	if((ulMsgCount >= 2) || (ulPrintOnDelta))
	{
		BAVC_MVD_Field *pPic = (BAVC_MVD_Field *)pNewPic;
		uint32_t ulPicIdx = 0;
		do {
			BDBG_ERR(("---------------------------------- pic %d", ulPicIdx++));
			BDBG_ERR(("src_id                        : mfd%d", hSource->eId));
			BDBG_ERR(("pPic->bMute                   : %d", pPic->bMute));
			BDBG_ERR(("pPic->ulAdjQp                 : %d", pPic->ulAdjQp));
			BDBG_ERR(("pPic->bCaptureCrc             : %d", pPic->bCaptureCrc));
			BDBG_ERR(("pPic->ulOrigPTS               : %d", pPic->ulOrigPTS));
			BDBG_ERR(("pPic->ulIdrPicID              : %d", pPic->ulIdrPicID));
			BDBG_ERR(("pPic->int32_PicOrderCnt       : %d", pPic->int32_PicOrderCnt));
			BDBG_ERR(("pPic->eSourcePolarity         : %d", pPic->eSourcePolarity));
			BDBG_ERR(("pPic->eInterruptPolarity      : %d", pPic->eInterruptPolarity));
			BDBG_ERR(("pPic->eChrominanceIntMode     : %d", pPic->eChrominanceInterpolationMode));
			BDBG_ERR(("pPic->eMpegType               : %d", pPic->eMpegType));
			BDBG_ERR(("pPic->eYCbCrType              : %d", pPic->eYCbCrType));
			BDBG_ERR(("pPic->eAspectRatio            : %d", pPic->eAspectRatio));
			BDBG_ERR(("pPic->uiSampleAspectRatioX    : %d", pPic->uiSampleAspectRatioX));
			BDBG_ERR(("pPic->uiSampleAspectRatioY    : %d", pPic->uiSampleAspectRatioY));
			BDBG_ERR(("pPic->eFrameRateCode          : %d", pPic->eFrameRateCode));
			BDBG_ERR(("pPic->ePxlFmt                 : %s", BPXL_ConvertFmtToStr(pPic->ePxlFmt)));
			BDBG_ERR(("pPic->eMatrixCoefficients     : %d", pPic->eMatrixCoefficients));
			BDBG_ERR(("pPic->bStreamProgressive      : %d", pPic->bStreamProgressive));
			BDBG_ERR(("pPic->bFrameProgressive       : %d", pPic->bFrameProgressive));
			BDBG_ERR(("pPic->hHeap                   : 0x%08x", pPic->hHeap));
			BDBG_ERR(("pPic->pChrominanceBufAddr     : 0x%08x", pPic->pChrominanceFrameBufferAddress));
			BDBG_ERR(("pPic->pLuminanceBufAddr       : 0x%08x", pPic->pLuminanceFrameBufferAddress));
			BDBG_ERR(("pPic->ulRowStride             : 0x%08x", pPic->ulRowStride));
			BDBG_ERR(("pPic->ulLuminanceNMBY         : %d", pPic->ulLuminanceNMBY));
			BDBG_ERR(("pPic->ulChrominanceNMBY       : %d", pPic->ulChrominanceNMBY));
			BDBG_ERR(("pPic->ulLumaRangeRemapping    : 0x%x", pPic->ulLumaRangeRemapping));
			BDBG_ERR(("pPic->ulChromaRangeRemapping  : 0x%x", pPic->ulChromaRangeRemapping));
			BDBG_ERR(("pPic->ulDisplayHorizontalSize : %d", pPic->ulDisplayHorizontalSize));
			BDBG_ERR(("pPic->ulDisplayVerticalSize   : %d", pPic->ulDisplayVerticalSize));
			BDBG_ERR(("pPic->ulSourceHorizontalSize  : %d", pPic->ulSourceHorizontalSize));
			BDBG_ERR(("pPic->ulSourceVerticalSize    : %d", pPic->ulSourceVerticalSize));
			BDBG_ERR(("pPic->i32_HorizontalPanScan   : %d", pPic->i32_HorizontalPanScan));
			BDBG_ERR(("pPic->i32_VerticalPanScan     : %d", pPic->i32_VerticalPanScan));
			BDBG_ERR(("pPic->ulSourceClipLeft        : %d", pPic->ulSourceClipLeft));
			BDBG_ERR(("pPic->ulSourceClipTop         : %d", pPic->ulSourceClipTop));
			BDBG_ERR(("pPic->hSurface                : 0x%08x", pPic->hSurface));
			BDBG_ERR(("pPic->bPictureRepeatFlag      : %d", pPic->bPictureRepeatFlag));
			BDBG_ERR(("pPic->hFgtHeap                : 0x%08x", pPic->hFgtHeap));
			BDBG_ERR(("pPic->pFgtSeiBufferAddress    : 0x%08x", pPic->pFgtSeiBufferAddress));
			BDBG_ERR(("pPic->ulChannelId             : %d", pPic->ulChannelId));
			BDBG_ERR(("pPic->eStripeWidth            : %d", pPic->eStripeWidth));
			BDBG_ERR(("pPic->bIgnoreCadenceMatch     : %d", pPic->bIgnoreCadenceMatch));
			BDBG_ERR(("pPic->bValidAfd               : %d", pPic->bValidAfd));
			BDBG_ERR(("pPic->ulAfd                   : %d", pPic->ulAfd));
			BDBG_ERR(("pPic->eBarDataType            : %d", pPic->eBarDataType));
			BDBG_ERR(("pPic->ulTopLeftBarValue       : %d", pPic->ulTopLeftBarValue));
			BDBG_ERR(("pPic->ulBotRightBarValue      : %d", pPic->ulBotRightBarValue));
			BDBG_ERR(("pPic->eOrientation            : %d", pPic->eOrientation));
			BDBG_ERR(("pPic->pNext                   : %p", pPic->pNext));
			BDBG_ERR(("pPic->pEnhanced               : %p", pPic->pEnhanced));
			if(pPic->pEnhanced)
			{
				BAVC_MVD_Field *pEnhanced = (BAVC_MVD_Field *)pPic->pEnhanced;

				BDBG_ERR(("pPic->pEnhanced->eOrientation       : %p",     pEnhanced->eOrientation));
				BDBG_ERR(("pPic->pEnhanced->pChrominanceBufAddr: 0x%08x", pEnhanced->pChrominanceFrameBufferAddress));
				BDBG_ERR(("pPic->pEnhanced->pLuminanceBufAddr  : 0x%08x", pEnhanced->pLuminanceFrameBufferAddress));
			}
			pPic = pPic->pNext;
		}while (pPic);

		/* keep on printing until we get to zero, and update field. */
		if(ulMsgCount >= 2)
		{
			BREG_Write32_isr(hSource->hVdc->hRegister, ulDebugReg, ulMsgCount - 2);
		}
	}
	return;
}
#endif


/**************************************************************************
 *
 */
void BVDC_P_Source_ValidateMpegData_isr
	( BVDC_Source_Handle         hSource,
	  BAVC_MVD_Field            *pNewPic,
	  const BAVC_MVD_Field      *pCurPic)
{
	/* SW7425-686 solve XVD and BVN refresh rate mismatch problem */
	if(hSource->hSyncLockCompositor)
	{
		const BFMT_VideoInfo *pFmtInfo;
		uint32_t ulSrcVerq = 0;

		pFmtInfo = hSource->hSyncLockCompositor->hDisplay->stCurInfo.pFmtInfo;
		hSource->bFreshRateMismatch = false;

		if(pNewPic->bMute == false)
		{
			switch(pNewPic->eInterruptRefreshRate)
			{
				case(BFMT_Vert_e23_976Hz):
					ulSrcVerq = 2397; break;
				case(BFMT_Vert_e24Hz):
					ulSrcVerq = 2400; break;
				case(BFMT_Vert_e25Hz):
					ulSrcVerq = 2500; break;
				case(BFMT_Vert_e29_97Hz):
					ulSrcVerq = 2997; break;
				case(BFMT_Vert_e30Hz):
					ulSrcVerq = 3000; break;
				case(BFMT_Vert_e48Hz):
					ulSrcVerq =4800; break;
				case(BFMT_Vert_e50Hz):
					ulSrcVerq = 5000; break;
				case(BFMT_Vert_e59_94Hz):
					ulSrcVerq = 5994; break;
				case(BFMT_Vert_e60Hz):
					ulSrcVerq = 6000; break;
				case(BFMT_Vert_eInvalid):
				case(BFMT_Vert_eLast):
				default:
				/* @@@ SW7425-832: set SrcVerq to Display fresh rate
				when BFMT_Vert_eInvalid or BFMT_Vert_eLast.
				Need to check with XVD why it is invalid number */
				ulSrcVerq = pFmtInfo->ulVertFreq; break;
			}

			if(!BVDC_P_EQ_DELTA (ulSrcVerq, pFmtInfo->ulVertFreq, 10)) {
				hSource->bFreshRateMismatch  = true;
				BDBG_WRN(("XVD -> BVN refresh rate mismatch! XVD %4d x %4d @ %4d BVN %4d x %4d @ %4d",
				pNewPic->ulSourceHorizontalSize, pNewPic->ulSourceVerticalSize, pNewPic->eInterruptRefreshRate,
				pFmtInfo->ulDigitalWidth, pFmtInfo->ulDigitalHeight, pFmtInfo->ulVertFreq));
			}
		}
	}

	/* Not gfx surface displayed as mpeg buffer case:
	 * if null address or size => force mute */
	/* if requested to capture CRC, don't adjust original source size */
	if((!pNewPic->bCaptureCrc) && (pNewPic->ePxlFmt == BPXL_INVALID) &&
	   ((pNewPic->ulSourceHorizontalSize < BVDC_P_SRC_INPUT_H_MIN) ||
	    (pNewPic->ulSourceVerticalSize   < BVDC_P_SRC_INPUT_V_MIN) ||
	    (!pNewPic->pLuminanceFrameBufferAddress) ||
	    (!pNewPic->pChrominanceFrameBufferAddress)))
	{
		pNewPic->ulSourceHorizontalSize  = BVDC_P_SRC_INPUT_H_MIN;
		pNewPic->ulSourceVerticalSize    = BVDC_P_SRC_INPUT_V_MIN;
		pNewPic->ulDisplayHorizontalSize = BVDC_P_SRC_INPUT_H_MIN;
		pNewPic->ulDisplayVerticalSize   = BVDC_P_SRC_INPUT_V_MIN;
		pNewPic->bMute = true;
	}

	/* Avoid invalid size that can lock up BVN when muted */
	if(!pNewPic->bCaptureCrc || pNewPic->bMute)
	{
		/* (1) Source/display size are invalid.  Corrected them here.  Non-zero. */
		if((pNewPic->ulSourceHorizontalSize  < BVDC_P_SRC_INPUT_H_MIN) ||
		   (pNewPic->ulSourceVerticalSize    < BVDC_P_SRC_INPUT_V_MIN))
		{
			pNewPic->ulSourceHorizontalSize  = BVDC_P_SRC_INPUT_H_MIN;
			pNewPic->ulSourceVerticalSize    = BVDC_P_SRC_INPUT_V_MIN;
		}

		/* (2) If we receiving 1088i stream we will assume it contains the
		 * non-active video in the last 8 lines.  We'll drop them.  PR10698.  Or
		 * any value that might break the VDC (restricting to HW limit, not
		 * necessary rts limit). */
		if((1088 == pNewPic->ulSourceVerticalSize)
#ifdef BCHP_MFD_0_DISP_VSIZE_VALUE_MASK
		   || (!BVDC_P_SRC_VALIDATE_FIELD(MFD_0_DISP_VSIZE, VALUE, pNewPic->ulSourceVerticalSize))
#endif
#ifdef BCHP_MFD_0_DISP_HSIZE_VALUE_MASK
		   || (!BVDC_P_SRC_VALIDATE_FIELD(MFD_0_DISP_HSIZE, VALUE, pNewPic->ulSourceHorizontalSize))
#endif
#ifdef BCHP_MFD_0_PICTURE0_DISP_VERT_WINDOW_START_MASK
		   || (!BVDC_P_SRC_VALIDATE_FIELD(MFD_0_PICTURE0_DISP_VERT_WINDOW, START, pNewPic->ulSourceVerticalSize))
#endif
		  )
		{
			pNewPic->ulSourceVerticalSize    =
				BVDC_P_MIN(pNewPic->ulSourceVerticalSize,    BFMT_1080I_HEIGHT);
			pNewPic->ulSourceHorizontalSize  =
				BVDC_P_MIN(pNewPic->ulSourceHorizontalSize,  BFMT_1080I_WIDTH);
		}

#if (!BVDC_P_SUPPORT_3D_VIDEO)
		/* (3) Handle stream larger than 720p.  Feeder will NOT be
		 * able to handle this, and halt the bvb downstream.  VDC will
		 * force a field output for this pictures.
		 * Two exceptions:
		 *   - PsF mode;
		 *   - 1080p24/25/30 pass-thru mode;
		 *   - Feeding using GFX surface;
		 * For MPG source, ulVertFreq is the max scanout frame rate; */
		if((BAVC_Polarity_eFrame == pNewPic->eSourcePolarity) &&
		   (pNewPic->ulSourceVerticalSize  > BFMT_720P_HEIGHT) &&
		   (BVDC_P_PSF_VERT_FREQ < hSource->ulVertFreq) &&
		   (!hSource->hMpegFeeder->bGfxSrc))
		{
			BDBG_WRN(("MVD[%d] passing larger than 720p frame picture", hSource->eId));
			pNewPic->eSourcePolarity = BAVC_Polarity_eTopField;
		}
#endif

		/* (4) If interlaced stream vertical size must be even */
		if(BAVC_Polarity_eFrame != pNewPic->eSourcePolarity)
		{
			/* Jira SW7405-4239: XMD might send odd width for special aspect ratio purpose
			   pNewPic->ulSourceHorizontalSize = pNewPic->ulSourceHorizontalSize & ~1; */
			pNewPic->ulSourceVerticalSize   = pNewPic->ulSourceVerticalSize & ~1;
		}
	}

	/* (4) Handle unknown color space.  In the cases of 'unknown', 'forbidden'
	 * and 'reserved' values passed from MVD, we display the picture according
	 * to its frame size, i.e. if frame size > 720x576, treat it as default
	 * HD(BT.709) color; else treat it as NTSC or PAL SD. */
	if((BAVC_MatrixCoefficients_eFCC != pNewPic->eMatrixCoefficients) &&
	   (BAVC_MatrixCoefficients_eSmpte_170M != pNewPic->eMatrixCoefficients) &&
	   (BAVC_MatrixCoefficients_eSmpte_240M != pNewPic->eMatrixCoefficients) &&
	   (BAVC_MatrixCoefficients_eItu_R_BT_709 != pNewPic->eMatrixCoefficients) &&
	   (BAVC_MatrixCoefficients_eItu_R_BT_470_2_BG != pNewPic->eMatrixCoefficients))
	{
		const BVDC_Settings *pDefSetting = &hSource->hVdc->stSettings;
		if(pNewPic->ulSourceVerticalSize > BFMT_PAL_HEIGHT)
		{
			pNewPic->eMatrixCoefficients = pDefSetting->eColorMatrix_HD;
		}
		else if(pNewPic->ulSourceVerticalSize > BFMT_480P_HEIGHT)
		{
			pNewPic->eMatrixCoefficients = BAVC_MatrixCoefficients_eItu_R_BT_470_2_BG;
		}
		else
		{
			pNewPic->eMatrixCoefficients = pDefSetting->eColorMatrix_SD;
		}
	}

#ifdef BVDC_MPEG_SOURCE_MAD_DEBUG
	/* Note:
	 * 1. if mvd passes frame to vdc, we need to be intelligent here;
	 * 2. progressive sequence is true progressive material, so we don't need to
	 *    detect and force scanout fields second hand for deinterlacer;
	 * 3. interlaced sequence could decode into fields or frames; there is no harm
	 *    to force scanout fields for deinterlacer; */
	if(!pNewPic->bCaptureCrc || pNewPic->bMute)
	{
		if((pNewPic->eSourcePolarity == BAVC_Polarity_eFrame) &&
		   !pNewPic->bStreamProgressive)
		{
			if((hSource->stCurInfo.bDeinterlace) &&
#if (BVDC_P_SUPPORT_MAD_SRC_1080I)
			   (pNewPic->ulSourceVerticalSize   <= BFMT_1080I_HEIGHT)
#else
			   (pNewPic->ulSourceHorizontalSize <= BFMT_PAL_WIDTH) &&
			   (pNewPic->ulSourceVerticalSize   <= BFMT_PAL_HEIGHT)
#endif
			  )
			{
				BDBG_WRN(("Force scanout field instead of frame"));
				/* force the field data source polarity */
				pNewPic->eSourcePolarity = BVDC_P_NEXT_POLARITY(pCurPic->eSourcePolarity);
			}
		}
	}
#endif

	/* MVD should not give VDC a bad interrupt field somehing like
	 * 5 or -1.  It should giving the interrupt that VDC interrupted them
	 * with.   For mute pictures everything about field buffer can be
	 * invalid, but not the interrupt field id. */
	BDBG_ASSERT(
		(BAVC_Polarity_eTopField==pNewPic->eInterruptPolarity) ||
		(BAVC_Polarity_eBotField==pNewPic->eInterruptPolarity) ||
		(BAVC_Polarity_eFrame   ==pNewPic->eInterruptPolarity));

	/* Chroma pan-scan/src-clip for H.264 */
	if(pNewPic->eYCbCrType == BAVC_YCbCrType_e4_4_4)
	{
		BDBG_ERR(("eYCbCrType 4:4:4 is not supported by MFD!"));
		BDBG_ASSERT(0);
	}

	/* Check if fields that are to be written to registers fit
	 * within their field sizes. */
	if((!hSource->hMpegFeeder->bGfxSrc) &&
		(pNewPic->eSourcePolarity != BAVC_Polarity_eFrame) &&
	    (!BVDC_P_SRC_VALIDATE_FIELD(MFD_0_LAC_CNTL, OUTPUT_FIELD_POLARITY,
	                                pNewPic->eSourcePolarity)))
	{
		BDBG_ERR(("eSourcePolarity is invalid"));
		pNewPic->eSourcePolarity = BAVC_Polarity_eTopField;
	}

	/* Muted channel doesn't need to check the following things! */
	if(!pNewPic->bMute)
	{
		if(!BVDC_P_SRC_VALIDATE_FIELD(MFD_0_LAC_CNTL, CHROMA_TYPE,
			                           pNewPic->eYCbCrType - BAVC_YCbCrType_e4_2_0))
		{
			BDBG_ERR(("eYCbCrType is invalid"));
			pNewPic->eYCbCrType = BAVC_YCbCrType_e4_2_0;
		}

		if(!BVDC_P_SRC_VALIDATE_FIELD(MFD_0_LAC_CNTL, CHROMA_INTERPOLATION,
			                           pNewPic->eChrominanceInterpolationMode))
		{
			BDBG_ERR(("eChrominanceInterpolationMode is invalid"));
			pNewPic->eChrominanceInterpolationMode = BAVC_InterpolationMode_eField;
		}

		if((!hSource->hMpegFeeder->bGfxSrc) &&
			!BVDC_P_SRC_VALIDATE_FIELD(MFD_0_LUMA_NMBY, VALUE, pNewPic->ulLuminanceNMBY))
		{
			BDBG_ERR(("ulLuminanceNMBY is invalid: %d", pNewPic->ulLuminanceNMBY));
			pNewPic->ulLuminanceNMBY = BVDC_P_SRC_FIELD_DATA_MASK(MFD_0_LUMA_NMBY, VALUE,
		                                                          pNewPic->ulLuminanceNMBY);
		}

		if((!hSource->hMpegFeeder->bGfxSrc) &&
			!BVDC_P_SRC_VALIDATE_FIELD(MFD_0_CHROMA_NMBY, VALUE, pNewPic->ulChrominanceNMBY))
		{
			BDBG_ERR(("ulChrominanceNMBY is invalid: %d", pNewPic->ulChrominanceNMBY));
			pNewPic->ulChrominanceNMBY = BVDC_P_SRC_FIELD_DATA_MASK(MFD_0_CHROMA_NMBY, VALUE,
		                                                            pNewPic->ulLuminanceNMBY);
		}

		/* Only support BAVC_ChromaLocation_eType0 to BAVC_ChromaLocation_eType3 for now */
		if(pNewPic->eMpegType > BAVC_ChromaLocation_eType3)
		{
			BDBG_ERR(("eMpegType is invalid"));
			pNewPic->eMpegType = BAVC_MpegType_eMpeg2;

		}
	}

	/* Check if there are general changes from previous pictures. */
	if(BVDC_P_FIELD_DIFF(pNewPic, pCurPic, bMute) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, ulSourceHorizontalSize) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, ulSourceVerticalSize) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, ulDisplayHorizontalSize) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, ulDisplayVerticalSize) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, eOrientation) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, uiSampleAspectRatioX) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, uiSampleAspectRatioY) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, eAspectRatio) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, bValidAfd) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, ulAfd) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, eBarDataType) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, ulTopLeftBarValue) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, ulBotRightBarValue) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, bStreamProgressive) ||
	   BVDC_P_FIELD_DIFF(pNewPic, pCurPic, bIgnoreCadenceMatch))
	{
		hSource->bPictureChanged = true;

		BDBG_MSG(("bMute changes: %d->%d", pCurPic->bMute, pNewPic->bMute));
		BDBG_MSG(("ulSourceHorizontalSize changes: %d->%d", pCurPic->ulSourceHorizontalSize, pNewPic->ulSourceHorizontalSize));
		BDBG_MSG(("ulSourceVerticalSize changes: %d->%d", pCurPic->ulSourceVerticalSize, pNewPic->ulSourceVerticalSize));
		BDBG_MSG(("ulDisplayHorizontalSize changes: %d->%d", pCurPic->ulDisplayHorizontalSize, pNewPic->ulDisplayHorizontalSize));
		BDBG_MSG(("ulDisplayVerticalSize changes: %d->%d", pCurPic->ulDisplayVerticalSize, pNewPic->ulDisplayVerticalSize));
		BDBG_MSG(("eOrientation changes: %d->%d", pCurPic->eOrientation, pNewPic->eOrientation));
		BDBG_MSG(("uiSampleAspectRatioX changes: %d->%d", pCurPic->uiSampleAspectRatioX, pNewPic->uiSampleAspectRatioX));
		BDBG_MSG(("uiSampleAspectRatioY changes: %d->%d", pCurPic->uiSampleAspectRatioY, pNewPic->uiSampleAspectRatioY));
		BDBG_MSG(("eAspectRatio changes: %d->%d", pCurPic->eAspectRatio, pNewPic->eAspectRatio));
		BDBG_MSG(("bValidAfd changes: %d->%d", pCurPic->bValidAfd, pNewPic->bValidAfd));
		BDBG_MSG(("ulAfd changes: %d->%d", pCurPic->ulAfd, pNewPic->ulAfd));
		BDBG_MSG(("eBarDataType changes: %d->%d", pCurPic->eBarDataType, pNewPic->eBarDataType));
		BDBG_MSG(("ulTopLeftBarValue changes: %d->%d", pCurPic->ulTopLeftBarValue, pNewPic->ulTopLeftBarValue));
		BDBG_MSG(("ulBotRightBarValue changes: %d->%d", pCurPic->ulBotRightBarValue, pNewPic->ulBotRightBarValue));
		BDBG_MSG(("bIgnoreCadenceMatch changes: %d->%d", pCurPic->bIgnoreCadenceMatch, pNewPic->bIgnoreCadenceMatch));
	}

	/* I -> P, or P -> I, for non-muted pictures. */
	if((pNewPic->eSourcePolarity | BAVC_Polarity_eBotField) !=
	   (pCurPic->eSourcePolarity | BAVC_Polarity_eBotField))
	{
		hSource->bPictureChanged = true;
		hSource->bRasterChanged  = true;
		hSource->stCurInfo.stDirty.stBits.bInputFormat = BVDC_P_DIRTY;
		BDBG_MSG(("mpeg source raster change: old %d -> new %d", pCurPic->eSourcePolarity, pNewPic->eSourcePolarity));
	}

	/* Check if anything wrong with mvd field. */
#if (BDBG_DEBUG_BUILD)
	BVDC_P_Source_PrintPicture(hSource, pCurPic, pNewPic);
#endif

	return;
}

/***************************************************************************
 * Application calls this function at every interrupt to set source info.
 *
 * Called to pass infomation from MVD to VDC.  For non-mpeg source
 * pNewPic will be NULL.   The data is use to build for the next
 * display vsync (or possibly queue up).
 * pvSourceHandle is (BVDC_Source_Handle).
 *
 */
void BVDC_Source_MpegDataReady_isr
	( void                            *pvSourceHandle,
	  int                              iParm2,
	  void                            *pvMvdField )
{
	uint32_t i;
	BVDC_Source_Handle hSource = (BVDC_Source_Handle)pvSourceHandle;
	BVDC_Window_Handle hWindow;
	BRDC_Slot_Handle hSlot;
	BRDC_List_Handle hList;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	BAVC_MVD_Field *pCurPic;
	BAVC_MVD_Field *pNewPic;
#else
	BAVC_MVD_Field stNewPic;
	BAVC_MVD_Field *pCurPic;
	BAVC_MVD_Field *pNewPic = &stNewPic;
#endif
	uint32_t ulOldEntries;
	BVDC_P_Source_DirtyBits *pCurDirty, *pOldDirty;
	bool bBuildBoth = false, bNewPicMute;
	uint32_t ulPictureIdx = 0;
	uint32_t ulSubRulEntries = 0;
	BRDC_Slot_Handle hSlotSlave = NULL;
	BRDC_List_Handle hListSlave = NULL;
	BVDC_P_ListInfo stMasterList, stList;
	BRDC_Trigger eTrigger = 0;
	uint32_t ulMosaicCount = 0;
	uint32_t ulMosaicWindows = 0;
	uint32_t ulComboTrigBitMask = 0; /* for combined triggers */
	bool bNoScanout = false; /* for PsF */
	BVDC_P_Source_Info *pNewInfo = &hSource->stNewInfo;
	BVDC_P_Source_Info *pCurInfo = &hSource->stCurInfo;
	BAVC_Polarity       ePrePolarity;

	BDBG_ENTER(BVDC_Source_MpegDataReady_isr);
	BSTD_UNUSED(iParm2);

	/* Casting to get parms */
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hSource->hVdc, BVDC_VDC);
	pCurDirty = &hSource->stCurInfo.stDirty;

	/* Make sure the BKNI enter/leave critical section works. */
	BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs);
	hSource->hVdc->ulInsideCs++;

	/* We should not even get the Gfx or analog source here. */
	BDBG_ASSERT(BVDC_P_SRC_IS_MPEG(hSource->eId));


#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#if BVDC_P_SUPPORT_MOSAIC_MODE
	pNewPic = &(hSource->stNewPic[0]);
#else
	pNewPic = &(hSource->stNewPic);
#endif
#endif
	pCurPic = &hSource->stPrevMvdField;
	bNewPicMute = ((BAVC_MVD_Field*)pvMvdField)->bMute;
	if(bNewPicMute)
	{
		*pNewPic = *pCurPic;

		/* When bMute is set, all values except eInterruptPolarity,
		 * bIgnorePicture, and bStallStc are invalid and undefined */
		pNewPic->bMute = ((BAVC_MVD_Field*)pvMvdField)->bMute;
		pNewPic->eInterruptPolarity = ((BAVC_MVD_Field*)pvMvdField)->eInterruptPolarity;
		pNewPic->bIgnorePicture = ((BAVC_MVD_Field*)pvMvdField)->bIgnorePicture;
		pNewPic->bStallStc = ((BAVC_MVD_Field*)pvMvdField)->bStallStc;
		pNewPic->bLast = ((BAVC_MVD_Field*)pvMvdField)->bLast;
		pNewPic->bChannelChange = ((BAVC_MVD_Field*)pvMvdField)->bChannelChange;
		pNewPic->eOrientation = BFMT_Orientation_e2D;
	}
	else
	{
		*pNewPic = *(BAVC_MVD_Field*)pvMvdField;
	}

	if ((hSource->hMpegFeeder->bGfxSrc) &&
		(((BAVC_MVD_Field*)pvMvdField)->pLuminanceFrameBufferAddress !=
		 (void *)hSource->hMpegFeeder->stGfxSurface.stCurSurInfo.ulAddress))
	{
		/* must be called from XVD when this MFD is used for gfx */
		BDBG_ERR(("Must disable XVD callback as MFD is used for gfx!"));
		goto MpegDataReady_Done;
	}

	hSource->ulMosaicFirstUnmuteRectIndex = 0;
	hSource->bMosaicFirstUnmuteRectIndexSet = false;

	/* Update source user info before using pCurInfo */
	BVDC_P_Source_UpdateSrcState_isr(hSource);

#if BVDC_P_SUPPORT_MOSAIC_MODE
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	if(hSource->stCurInfo.bMosaicMode && !bNewPicMute)
	{
		/* re-order pic list for z-order */
		uint32_t  ulIndex;
		BAVC_MVD_Field   *pXvdPic, *pVdcPic;

		/* Copy link list based on zorder */
		i = 0;
		pXvdPic = (BAVC_MVD_Field*)pvMvdField;
		while(pXvdPic && !pXvdPic->bMute)
		{
			ulIndex = hSource->aulMosaicZOrderIndex[i];
			pVdcPic = &(hSource->stNewPic[ulIndex]);
			*pVdcPic = *(BAVC_MVD_Field*)(pXvdPic);

			/* When input vertical size is odd, we round up to even lines for MFD 
			 * scan out, then use scaler to cut the last line. Scaler needs to 
			 * drain the last line at the end of picture. This drian may cause 
			 * scaler input completes after its output. This will cause scaler 
			 * enable error in mosaic mode */
			pVdcPic->ulSourceVerticalSize = pVdcPic->ulSourceVerticalSize & ~1;
			pVdcPic->ulDisplayVerticalSize = pVdcPic->ulDisplayVerticalSize & ~1;

			if((ulIndex + 1) < BVDC_P_MAX_MOSAIC)
				pVdcPic->pNext = &(hSource->stNewPic[ulIndex+1]);

			i++;
			pXvdPic = pXvdPic->pNext;
		}
		if(i > 0)
		{
			hSource->stNewPic[i-1].pNext = NULL;
		}
		
		while(i < hSource->ulMosaicCount)
		{
			/* Add dummy node */
			if(i == 0)
			{
				pVdcPic = &(hSource->stNewPic[0]);
				*pVdcPic = *(BAVC_MVD_Field*)(pvMvdField);
			}
			else
			{
				hSource->stNewPic[i-1].pNext = &(hSource->stNewPic[i]);
			}

			hSource->stNewPic[i].bMute = true;
			hSource->stNewPic[i].ulChannelId = i;
			hSource->stNewPic[i].pNext = NULL;
			i++;
		}
#else
	if(hSource->stCurInfo.bMosaicMode)
	{
		/* re-order pic list for z-order */
		uint32_t          i = 0;
		BAVC_MVD_Field   *pOldCurPic, *pOldPrevPic;
		BAVC_MVD_Field   *pNewCurPic, *pNewPrevPic;

		/* Loop through old pic list */
		pOldPrevPic = pNewPic;
		pOldCurPic = pOldPrevPic->pNext;
		while(pOldCurPic && (++i < hSource->ulMosaicCount))
		{
			if(hSource->aucMosaicZOrder[pOldCurPic->ulChannelId] <
			   hSource->aucMosaicZOrder[pOldPrevPic->ulChannelId])
			{
				/* Find an out of order pic */
				BDBG_MSG(("Src[%d] Find out of order tile: Chan[%d]: %d, Chan[%d]: %d",
					hSource->eId, pOldPrevPic->ulChannelId,
					hSource->aucMosaicZOrder[pOldPrevPic->ulChannelId],
					pOldCurPic->ulChannelId,
					hSource->aucMosaicZOrder[pOldCurPic->ulChannelId]));

				/* remove pOldCurPic from list */
				pOldPrevPic->pNext = pOldCurPic->pNext;
				BDBG_MSG(("Src[%d] Remove Chan: %d from list",
					hSource->eId, pOldCurPic->ulChannelId));

				/* Loop through new pic list */
				pNewPrevPic = NULL;
				pNewCurPic = pNewPic;
				while(pNewCurPic &&
					(hSource->aucMosaicZOrder[pOldCurPic->ulChannelId] >
					 hSource->aucMosaicZOrder[pNewCurPic->ulChannelId]))
				{
					pNewPrevPic = pNewCurPic;
					pNewCurPic = pNewCurPic->pNext;
				}

				/* Find the new position for pOldCurPic */
				if(pNewPrevPic == NULL)
				{
					/* Add pOldCurPic back to beginning of list */
					pOldCurPic->pNext = pNewPic;
					pNewPic = pOldCurPic;
				}
				else
				{
					/* Add pOldCurPic back to list between pNewPrevPic and pNewCurPic */
					pNewPrevPic->pNext = pOldCurPic;
					pOldCurPic->pNext = pNewCurPic;
				}

			}
			else
			{
				pOldPrevPic = pOldCurPic;
			}

			pOldCurPic = pOldPrevPic->pNext;
		}
#endif
	}
#endif

#if (BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP)
	if((pNewPic->eOrientation == BFMT_Orientation_e3D_LeftRight) &&
	    pNewPic->pEnhanced)
	{
		BAVC_MVD_Field   *pEnhanced;

		pEnhanced = (BAVC_MVD_Field *)pNewPic->pEnhanced;
		if(pEnhanced->eOrientation == BFMT_Orientation_eLeftRight_Enhanced)
		{
			/* Set eOrientation to BFMT_Orientation_eLeftRight_Enhanced
			 * to distinguish from regular BFMT_Orientation_e3D_LeftRight */
			pNewPic->eOrientation = BFMT_Orientation_eLeftRight_Enhanced;
		}
	}
#endif

#if (BVDC_P_SUPPORT_3D_VIDEO)
	/* bOrientationOverride only valid when original
	 * orientation from XVD is 2D */
	if(pNewPic->eOrientation == BFMT_Orientation_e2D)
	{
		if(pCurInfo->bOrientationOverride)
		{
			if(pCurInfo->eOrientation == BFMT_Orientation_e3D_OverUnder)
			{
				pNewPic->ulSourceVerticalSize  = pNewPic->ulSourceVerticalSize / 2;
				pNewPic->ulDisplayVerticalSize = pNewPic->ulDisplayVerticalSize / 2;
				pNewPic->uiSampleAspectRatioY  = pNewPic->uiSampleAspectRatioY * 2;
			}
			else if(pCurInfo->eOrientation == BFMT_Orientation_e3D_LeftRight)
			{
				pNewPic->ulSourceHorizontalSize  = pNewPic->ulSourceHorizontalSize / 2;
				pNewPic->ulDisplayHorizontalSize = pNewPic->ulDisplayHorizontalSize / 2;
				pNewPic->uiSampleAspectRatioX    = pNewPic->uiSampleAspectRatioX * 2;
			}
			pNewPic->eOrientation = pCurInfo->eOrientation;
		}
	}
	else if(pNewPic->eOrientation == BFMT_Orientation_e3D_OverUnder)
	{
		pNewPic->ulSourceVerticalSize  = pNewPic->ulSourceVerticalSize / 2;
		pNewPic->ulDisplayVerticalSize = pNewPic->ulDisplayVerticalSize / 2;
		pNewPic->uiSampleAspectRatioY  = pNewPic->uiSampleAspectRatioY * 2;
	}
	else if(pNewPic->eOrientation == BFMT_Orientation_e3D_LeftRight)
	{
		pNewPic->ulSourceHorizontalSize  = pNewPic->ulSourceHorizontalSize / 2;
		pNewPic->ulDisplayHorizontalSize = pNewPic->ulDisplayHorizontalSize / 2;
		pNewPic->uiSampleAspectRatioX    = pNewPic->uiSampleAspectRatioX * 2;
	}
#endif

	/* H & V size alignment */
	/* Jira SW7405-4239: XMD might send odd width for special aspect ratio purpose
	  pNewPic->ulSourceHorizontalSize = pNewPic->ulSourceHorizontalSize & ~1; */
	pNewPic->ulSourceVerticalSize   = pNewPic->ulSourceVerticalSize & ~1;

	/* PsF: check if need to scanout; also store the scanout state for the
	        next isr's scanout decision; */
	if(hSource->stCurInfo.bPsfEnable)
	{
		bool bPsfSrc =
			(BFMT_720P_HEIGHT     <= pNewPic->ulSourceVerticalSize) &&
			(BAVC_Polarity_eFrame == pNewPic->eSourcePolarity)       &&
			(BAVC_FrameRateCode_e30 >= pNewPic->eFrameRateCode);

		/* if user just turns on PsF, but decoder is in pause mode(bRepeat=true),
		   we still need to scan out for the first time;
		   if 1080p source repeats, don't scanout;
		   if last 1080p frame was scaned out and not in 1080p pass-thru (<=30Hz),
		   don't scanout for this time; */
		bNoScanout = (hSource->bPsfScanout) ||
			(!pCurDirty->stBits.bPsfMode &&
			  pNewPic->bPictureRepeatFlag &&
			  bPsfSrc);

		/* to keep src/win state machine moving, don't drop too many frames; */
		if(bNoScanout)
		{
			if(++hSource->ulDropFramesCnt > 3)
			{
				bNoScanout = false;
				hSource->ulDropFramesCnt = 0;
				BDBG_MSG(("Mfd%d drops 4 1080p frames in a roll; Resume scanout!", hSource->eId));
			}
		}
		else
		{
			hSource->ulDropFramesCnt = 0;
		}

		/* 1080p PsF scanout RUL must chop the RUL size to protect itself! */
		hSource->bPsfScanout =
			!bNoScanout &&
			 bPsfSrc  &&
			(hSource->ulDispVsyncFreq > BVDC_P_PSF_VERT_FREQ);

		/* PsF: the source vertical freq is the frame rate (for frame capture) */
		if(hSource->bPsfScanout)
		{
			uint32_t ulVertFreq;

			switch(pNewPic->eFrameRateCode)
			{
			case BAVC_FrameRateCode_e23_976:
				ulVertFreq = 2397;
				break;
			case BAVC_FrameRateCode_e24:
				ulVertFreq = 24 * BFMT_FREQ_FACTOR;
				break;
			case BAVC_FrameRateCode_e25:
				ulVertFreq = 25 * BFMT_FREQ_FACTOR;
				break;
			case BAVC_FrameRateCode_e29_97:
				ulVertFreq = 2997;
				break;
			case BAVC_FrameRateCode_e30:
				ulVertFreq = 30 * BFMT_FREQ_FACTOR;
				break;
			default: /* shouldn't be here! */
				BDBG_WRN(("=== Something is in doubt!"));
				BDBG_WRN(("ulSourceVerticalSize = %d", pNewPic->ulSourceVerticalSize));
				BDBG_WRN(("eSourcePolarity        = %d", pNewPic->eSourcePolarity));
				BDBG_WRN(("eFrameRateCode      = %d", pNewPic->eFrameRateCode));
				BDBG_WRN(("Ignore stream frame rate. Use PsF %d Hz",
					BVDC_P_PSF_VERT_FREQ/BFMT_FREQ_FACTOR));
				BDBG_WRN(("==="));
				ulVertFreq = BVDC_P_PSF_VERT_FREQ;
			}

			if(hSource->ulVertFreq != ulVertFreq)
			{
				hSource->ulVertFreq = ulVertFreq;
				hSource->bRasterChanged = true;
			}
		}
		else if(!bPsfSrc)
		{
			hSource->ulVertFreq = hSource->ulDispVsyncFreq;
		}
	}
	/* PsF: if user disables PsF, skip one new scanout to avoid MFD hang; */
	else if(hSource->bPsfScanout)
	{
		hSource->bPsfScanout = false;
		bNoScanout = true;
		pCurDirty->stBits.bPsfMode  = BVDC_P_CLEAN;
		BDBG_MSG(("MFD[%d] disables PsF! Skip a new scanout to avoid MFD hang.", hSource->eId));
	}

	/* to avoid display/gfx flickering caused by the nop src RUL due to the
	   field swap on the artificial triggers, we need to
	   build RUL for the field swapped slot; */
	if(hSource->bPrevFieldSwap != hSource->bFieldSwap)
	{
		BDBG_MSG(("MFD[%d] bFieldSwap[%d->%d]!", hSource->eId,
			hSource->bPrevFieldSwap, hSource->bFieldSwap));

		/* field swap: this is the next real interrupt to come */
		pNewPic->eInterruptPolarity =
			BVDC_P_NEXT_POLARITY(pNewPic->eInterruptPolarity);

		/* update prev flag */
		hSource->bPrevFieldSwap = hSource->bFieldSwap;
	}

	/* validate the input data structure; */
	BVDC_P_Source_ValidateMpegData_isr(hSource, pNewPic, pCurPic);

	if(true == hSource->bFreshRateMismatch)
	{
		ePrePolarity = pCurPic->eSourcePolarity;

		pNewPic->eSourcePolarity = (ePrePolarity == BAVC_Polarity_eTopField) ?
			BAVC_Polarity_eBotField : BAVC_Polarity_eTopField;
	}

	/* Update the format information for MPEG */
	if(hSource->bPictureChanged)
	{
		const BFMT_VideoInfo *pNewFmtInfo;
		const BFMT_VideoInfo *pCurFmtInfo;
		uint32_t ulNewPicFrmRate;
		bool bNewPicInterlaced;
		uint32_t ulSourceHorizontalSize, ulSourceVerticalSize;

		ulSourceHorizontalSize = pNewPic->ulSourceHorizontalSize;
		ulSourceVerticalSize   = pNewPic->ulSourceVerticalSize;

#if (BVDC_P_SUPPORT_3D_VIDEO)
		if(pNewPic->eOrientation == BFMT_Orientation_e3D_LeftRight)
		{
			ulSourceHorizontalSize  = ulSourceHorizontalSize * 2;
		}
		else if(pNewPic->eOrientation == BFMT_Orientation_e3D_OverUnder)
		{
			ulSourceVerticalSize  = ulSourceVerticalSize * 2;
		}
#endif

		ulNewPicFrmRate = s_aulFrmRate[BVDC_P_MIN(BVDC_P_MAX_FRM_RATE_CODE, pNewPic->eFrameRateCode)];
		bNewPicInterlaced = (BAVC_Polarity_eFrame != pNewPic->eSourcePolarity);

		/* retain last rate if framerate is unknown. */
		if(BAVC_FrameRateCode_eUnknown != pNewPic->eFrameRateCode)
		{
			hSource->ulStreamVertFreq = ulNewPicFrmRate << (!pNewPic->bStreamProgressive);
		}

		pCurFmtInfo = (BFMT_VideoFmt_eMaxCount == hSource->stExtVideoFmtInfo.eVideoFmt)?
			&(hSource->stExtVideoFmtInfo) : pCurInfo->pFmtInfo;

		if((!BVDC_P_EQ_DELTA(ulSourceHorizontalSize, pCurFmtInfo->ulDigitalWidth, BVDC_P_MPEG_FMT_DELTA)) ||
		   (!BVDC_P_EQ_DELTA(ulSourceVerticalSize, pCurFmtInfo->ulDigitalHeight, BVDC_P_MPEG_FMT_DELTA)) ||
		   (!BVDC_P_EQ_DELTA(ulNewPicFrmRate, pCurFmtInfo->ulVertFreq >> pCurFmtInfo->bInterlaced, BVDC_P_MPEG_FMT_DELTA)) ||
		   (bNewPicInterlaced != pCurFmtInfo->bInterlaced))
		{
			/* src video format changed. try to match an enumerated video format */
			for(i = 0; i < BVDC_P_MPEG_FMT_COUNT; i++)
			{
				pNewFmtInfo = BFMT_GetVideoFormatInfoPtr(s_aeMpegToFmt[i]);
				if((BVDC_P_EQ_DELTA(ulSourceHorizontalSize, pNewFmtInfo->ulDigitalWidth, BVDC_P_MPEG_FMT_DELTA)) &&
				   (BVDC_P_EQ_DELTA(ulSourceVerticalSize, pNewFmtInfo->ulDigitalHeight, BVDC_P_MPEG_FMT_DELTA)) &&
				   (BVDC_P_EQ_DELTA(ulNewPicFrmRate, pNewFmtInfo->ulVertFreq >> pNewFmtInfo->bInterlaced, BVDC_P_MPEG_FMT_DELTA)) &&
				   (bNewPicInterlaced == pNewFmtInfo->bInterlaced))
				{
					break;
				}
			}

			if(BVDC_P_MPEG_FMT_COUNT == i)
			{
				BFMT_VideoFmt  eVFmtCode;

				/* non-enumerated mpeg video format, will return stExtVideoFmtInfo as callback */
				hSource->stExtVideoFmtInfo.eVideoFmt = BFMT_VideoFmt_eMaxCount;
				hSource->stExtVideoFmtInfo.ulDigitalWidth = ulSourceHorizontalSize;
				hSource->stExtVideoFmtInfo.ulDigitalHeight = ulSourceVerticalSize;
				hSource->stExtVideoFmtInfo.ulVertFreq = ulNewPicFrmRate << bNewPicInterlaced;
				hSource->stExtVideoFmtInfo.bInterlaced = bNewPicInterlaced;

				/* approximate video Fmt code for VDC internal use only */
				if (bNewPicInterlaced)
				{
					eVFmtCode = (((ulSourceHorizontalSize <= BFMT_NTSC_WIDTH) &&
								  (ulSourceVerticalSize   <= BFMT_NTSC_HEIGHT)) ?
								 BFMT_VideoFmt_eNTSC :
								 ((ulSourceHorizontalSize <= BFMT_PAL_WIDTH) &&
								  (ulSourceVerticalSize   <= BFMT_PAL_HEIGHT)) ?
								 BFMT_VideoFmt_ePAL_G :
								 BFMT_VideoFmt_e1080i);
				}
				else
				{
					eVFmtCode = (((ulSourceHorizontalSize <= BFMT_480P_WIDTH) &&
								  (ulSourceVerticalSize   <= BFMT_480P_HEIGHT)) ?
								 BFMT_VideoFmt_e480p :
								 ((ulSourceHorizontalSize <= BFMT_576P_WIDTH) &&
								  (ulSourceVerticalSize   <= BFMT_576P_HEIGHT)) ?
								 BFMT_VideoFmt_e576p_50Hz :
								 ((ulSourceHorizontalSize <= BFMT_720P_WIDTH) &&
								  (ulSourceVerticalSize   <= BFMT_720P_HEIGHT)) ?
								 BFMT_VideoFmt_e720p :
								 BFMT_VideoFmt_e1080p);
				}
				pNewFmtInfo = BFMT_GetVideoFormatInfoPtr(eVFmtCode);

				BDBG_MSG(("MPEG[%d] changes to non-enumerated video fmt: w %d, h %d, Frmrate %d/100, bInterlaced %d ",
					hSource->eId, ulSourceHorizontalSize, ulSourceVerticalSize,
					ulNewPicFrmRate, bNewPicInterlaced? 1 : 0));
			}
			else
			{
				/* this is used later as deciding which formatInfo to send with callback */
				hSource->stExtVideoFmtInfo.eVideoFmt = pNewFmtInfo->eVideoFmt;
			}

			/* this is an enumerated video format */
			BVDC_P_BUF_MSG(("MPEG[%d] Format change %s -> %s", hSource->eId,
				pCurInfo->pFmtInfo->pchFormatStr, pNewFmtInfo->pchFormatStr));

			/* for BVDC_Source_GetSize support: need original size  */
			hSource->stExtVideoFmtInfo.ulWidth = ulSourceHorizontalSize;
			hSource->stExtVideoFmtInfo.ulHeight = ulSourceVerticalSize;

			/* Start the new format: this is only used for VDC internally for certain decision,
			 * might not have every thing accurate */
			pCurInfo->pFmtInfo      = pNewInfo->pFmtInfo = pNewFmtInfo;

			/* Get vdc base fmt information */
			pCurInfo->pVdcFmt       = pNewInfo->pVdcFmt  =
					BVDC_P_GetFormatInfoPtr_isr(pNewFmtInfo);

			/* Format changes, so set dirty to rebuild RUL. */
			pCurDirty->stBits.bInputFormat = BVDC_P_DIRTY;

			/* inform next ApplyChanges to copy activated isr setting into new info */
			hSource->stIsrInfo.stActivated.stBits.bInputFormat = BVDC_P_DIRTY;
		}
	}

#if BVDC_P_STG_NRT_CADENCE_WORKAROUND
	if(!hSource->bPrevIgnorePicture)
	{
		hSource->eNextFieldId   = pNewPic->eInterruptPolarity;
		hSource->eNextFieldIntP = pNewPic->eInterruptPolarity;
	}

	hSource->bPrevIgnorePicture = false;

	/* NRT mode only */
	if( hSource->hSyncLockCompositor &&
		BVDC_P_DISPLAY_NRT_STG(hSource->hSyncLockCompositor->hDisplay) &&
		pNewPic->bIgnorePicture)
	{
		hSource->bPrevIgnorePicture = true;
	}
#else
	hSource->eNextFieldId   = pNewPic->eInterruptPolarity;
	hSource->eNextFieldIntP = pNewPic->eInterruptPolarity;
#endif

	/* MosaicMode: the frame rate tracking channel will set it later; */
	if(hSource->stCurInfo.bMosaicMode)
	{
		hSource->eFrameRateCode = BAVC_FrameRateCode_eUnknown;
		hSource->eMatrixCoefficients = BAVC_MatrixCoefficients_eUnknown;
	}
	else
	{
		hSource->eFrameRateCode = pNewPic->eFrameRateCode;
		hSource->eMatrixCoefficients = pNewPic->eMatrixCoefficients;
	}

	if((hSource->stCurInfo.pfGenericCallback) &&
	   (hSource->bCaptureCrc      ||
	    pCurDirty->stBits.bInputFormat   ||
	    hSource->bDeferSrcPendingCb ||
	    (pCurDirty->stBits.bAddWin && hSource->stCurInfo.eResumeMode) ||
	    (pCurPic->bMute && !pNewPic->bMute)))
	{
		BVDC_Source_CallbackData *pCbData = &hSource->stSourceCbData;
		BVDC_Source_DirtyBits *pCbDirty = &pCbData->stDirty;

		/* Clear dirty bits */
		BVDC_P_CB_CLEAN_ALL_DIRTY(pCbDirty);

		/* Issue src pending call back when shutdown BVN completed. */
		if(hSource->bDeferSrcPendingCb)
		{
			BVDC_P_Source_CheckAndIssueCallback(hSource, pCbDirty);
		}

		/* Make sure the callback happen at least once, on first
		 * installation of callback to report the current status. */
		if(pCurDirty->stBits.bGenCallback)
		{
			pCbDirty->bActive     = BVDC_P_DIRTY;
			pCbDirty->bFmtInfo    = BVDC_P_DIRTY;
			hSource->bDeferSrcPendingCb = true;
		}
		else if((pCurDirty->stBits.bInputFormat) ||
		   (pCurDirty->stBits.bAddWin && hSource->stCurInfo.eResumeMode) ||
		   (pCurPic->bMute && !pNewPic->bMute))
		{
			/* defer it until all windows are shutdown!
			 * Note, if input format changes, the next vsync will show window
			 * shutdown dirty bits;
			 * Only when all its windows shutdown dirty bits are cleared;
			 * is it safe to callback source pending; */
			hSource->bDeferSrcPendingCb = hSource->stCurInfo.eResumeMode;
			pCbDirty->bFmtInfo       = pCurDirty->stBits.bInputFormat;
		}

		/* 7401 & beyond MFD supports field-accurate CRC checking on demand.
		 * Note: when a picture is required to capture CRC, its RUL is executed one
		 *       vsync later; and its CRC computation is completed at end of that
		 *       picture, i.e. another vsync later! So we use the stored key flag
		 *       to callback the captured CRC; */
		if(hSource->bCaptureCrc)
		{
			pCbDirty->bCrcValue = true;

			/* update CRC readings */
			BVDC_P_Feeder_GetCrc_isr(hSource->hMpegFeeder, hSource->hVdc->hRegister, pCbData);
		}

		/* callback only if something changed */
		if(BVDC_P_CB_IS_DIRTY(pCbDirty))
		{
			/* Update Callback data */
			pCbData->bActive  = !pNewPic->bMute;
			pCbData->pFmtInfo = ((BFMT_VideoFmt_eMaxCount == hSource->stExtVideoFmtInfo.eVideoFmt)?
								 &(hSource->stExtVideoFmtInfo) : hSource->stCurInfo.pFmtInfo);

			hSource->stCurInfo.pfGenericCallback(
				hSource->stCurInfo.pvGenericParm1,
				hSource->stCurInfo.iGenericParm2, (void*)pCbData);
			BDBG_MSG(("MPEG[%d] callBack: VideoFmtCode %d ",
					  hSource->eId, pCbData->pFmtInfo->eVideoFmt));
		}
	}

	/* if the compositor is in slip2lock transition, clean up syncslip display RUL
	   to avoid sync-slipped VFD overwrite error in case source/display _isr are
	   called in reverse order; */
	if(hSource->hSyncLockCompositor && hSource->hSyncLockCompositor->ulSlip2Lock)
	{
		BDBG_OBJECT_ASSERT(hSource->hSyncLockCompositor, BVDC_CMP);
		BDBG_OBJECT_ASSERT(hSource->hSyncLockCompositor->hDisplay, BVDC_DSP);

		/* Check if we're doing frame.  If we're doing frame we're use a topfield
		 * slot to trigger the frame slot in source isr for sycn lock. */
		if(BAVC_Polarity_eFrame == hSource->eNextFieldIntP)
		{
			BVDC_P_CMP_NEXT_RUL(hSource->hSyncLockCompositor, BAVC_Polarity_eTopField);
			hSlot = BVDC_P_CMP_GET_SLOT(hSource->hSyncLockCompositor, BAVC_Polarity_eTopField);
			hList = BVDC_P_CMP_GET_LIST(hSource->hSyncLockCompositor, BAVC_Polarity_eTopField);
		}
		else
		{
			/* Get the approriate slot/list for building RUL. */
			BVDC_P_CMP_NEXT_RUL(hSource->hSyncLockCompositor, hSource->eNextFieldIntP);
			hSlot = BVDC_P_CMP_GET_SLOT(hSource->hSyncLockCompositor, hSource->eNextFieldIntP);
			hList = BVDC_P_CMP_GET_LIST(hSource->hSyncLockCompositor, hSource->eNextFieldIntP);
		}

		/* Reset the RUL entry count and build synclock display RUL! */
		BRDC_Slot_UpdateLastRulStatus_isr(hSlot, hList, false);
		BRDC_List_SetNumEntries_isr(hList, 0);
		BVDC_P_ReadListInfo_isr(&stList, hList);

		BVDC_P_Compositor_BuildSyncLockRul_isr(hSource->hSyncLockCompositor, &stList, hSource->eNextFieldIntP);

		/* Updated lists count */
		BVDC_P_WriteListInfo_isr(&stList, hList);

		if(BAVC_Polarity_eFrame != hSource->eNextFieldIntP)
		{
			BRDC_Slot_Handle hOtherSlot = BVDC_P_CMP_GET_SLOT(hSource->hSyncLockCompositor,
				BVDC_P_NEXT_POLARITY(hSource->eNextFieldIntP));
			BRDC_List_Handle hOtherList = BVDC_P_CMP_GET_LIST(hSource->hSyncLockCompositor,
				BVDC_P_NEXT_POLARITY(hSource->eNextFieldIntP));
			BVDC_P_ListInfo   stOtherList;

			BRDC_Slot_UpdateLastRulStatus_isr(hOtherSlot, hOtherList, false);
			BRDC_List_SetNumEntries_isr(hOtherList, 0);
			BVDC_P_ReadListInfo_isr(&stOtherList, hOtherList);

			BVDC_P_Compositor_BuildSyncLockRul_isr(hSource->hSyncLockCompositor,
				&stOtherList, BVDC_P_NEXT_POLARITY(hSource->eNextFieldIntP));

			BVDC_P_WriteListInfo_isr(&stOtherList, hOtherList);
			BRDC_Slot_SetList_isr(hOtherSlot, hOtherList);
		}

		/* SWAP: Program the hw, to setup the nEntries for the slot with this
		 * new RUL. */
		BRDC_Slot_SetList_isr(hSlot, hList);

		/* clear the flag */
		hSource->hSyncLockCompositor->ulSlip2Lock = 0;
		BDBG_MSG(("Src[%d] intP%d clears ulSlip2Lock", hSource->eId, hSource->eNextFieldIntP));
	}

	/* Get the approriate slot/list for building RUL. */
	BVDC_P_SRC_NEXT_RUL(hSource, hSource->eNextFieldId);
	hSlot = BVDC_P_SRC_GET_SLOT(hSource, hSource->eNextFieldId);
	hList = BVDC_P_SRC_GET_LIST(hSource, hSource->eNextFieldId);

	BRDC_Slot_UpdateLastRulStatus_isr(hSlot, hList, true);

	/* Reset the RUL entry count and build RUL for backend! */
	BRDC_List_SetNumEntries_isr(hList, 0);

	/* PsF: prefix a NOP RUL at head of PIP source RUL for robustness purpose;
	        sync-slipped mpg source isr will only build capture side RUL; if 1080p
	        PsF scanout is on, robustness RUL will simply chop the RUL size to the
	        NOP minmum; */
	if(hSource->bPsfScanout && !hSource->hSyncLockCompositor)
	{
		BVDC_P_BuildNoOpsRul_isr(hList);
		BVDC_P_ReadListInfo_isr(&stMasterList, hList);
		stMasterList.ulPsfMark = (uint32_t)(stMasterList.pulCurrent - stMasterList.pulStart);
		stList.ulPsfMark = stMasterList.ulPsfMark;
	}
	else
	{
		BVDC_P_ReadListInfo_isr(&stMasterList, hList);
	}
	stMasterList.bMasterList = true;
	stList.bMasterList = true;

	/* MosaicMode: picture list
	 * ------------------------
	 * 1. each picture represents a channel (with channel id) of mosaic list;
	 * 2. picture list is sorted in ascending order by DM; shall we validate?
	 * 3. existing channel needs to be captured into he correct location, according to
	 *    the channel id (equivalent to rectangle id), into the cap buffer;
	 * 3. missing channel (closed channel) simply results in no scanout/capture; but we
	 *    need to use ClearRect to clear its corresponding mosaic rectangle at CMP;
	 * 4. missing channel also means ulPictureIdx != ulChannelId;
	 *    ulPictureIdx = 0 means to build master RUL, while ulPictureIdx > 0 means to
	 *    build slave RUL; we can also assume ulPictureIdx <= ulChannelId;
	 * 5. what if the first picture has ulChannelId != 0, or > ulMosaicCount?
	 * 6. need to make sure ulChannelId < BAVC_MOSAIC_MAX;
	 * 7. if the last picture ulChannelId < ulMosaicCount, ClearRect for the rest;
	 */
	do
	{
		bool  bBuiltCanvasCtrlWithWin = false;

	#define BDC_P_LIST_SEL(master, slave, idx)   ((0 == (idx))? (master) : (slave))

		/* get the old num of entries */
		BRDC_List_GetNumEntries_isr(BDC_P_LIST_SEL(hList, hListSlave, ulPictureIdx), &ulOldEntries);

		/* validate the next input data structure; */
		if(ulPictureIdx)
		{
			BVDC_P_Source_ValidateMpegData_isr(hSource, pNewPic, pCurPic);
		}

#if BVDC_P_SUPPORT_MOSAIC_MODE
		/* make sure channel id is valid */
		BDBG_ASSERT(pNewPic->ulChannelId < BAVC_MOSAIC_MAX);

		if(!hSource->bMosaicFirstUnmuteRectIndexSet)
		{
			if(!pNewPic->bMute)
			{
				hSource->ulMosaicFirstUnmuteRectIndex = ulPictureIdx;
				hSource->bMosaicFirstUnmuteRectIndexSet = true;
			}
		}
#endif

		/* update windows that connected to this source, including user info,
		 * destroy related state and disconnecting from source */
		for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
		{
			/* SKIP: If it's just created or inactive no need to build ruls. */
			if(!hSource->ahWindow[i] ||
				BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
				BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
			{
				continue;
			}

			hWindow = hSource->ahWindow[i];
			BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

			/* Non_MosaicMode window skips; */
			if(ulPictureIdx && !hWindow->stCurInfo.bMosaicMode)
			{
				continue;
			}

			BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
			BDBG_OBJECT_ASSERT(hWindow->hCompositor->hDisplay, BVDC_DSP);

			/* Vec format switch, executed the last force trigger from sync-lock
			 * display's slot.  It's in the middle of format change, no need to
			 * serve this interrupt. Clean up all source slots to be safe. */
			if((BVDC_P_ItState_eActive != hWindow->hCompositor->hDisplay->eItState) &&
			   ((hWindow->hCompositor->hSyncLockSrc == hSource) ||
				(hWindow->hCompositor->hForceTrigPipSrc == hSource)))
			{
				BVDC_P_Source_CleanupSlots_isr(hSource);
				BDBG_MSG(("Synclocked display switches format. Clean up MFD[%d] slots!",
					hSource->eId));
				goto MpegDataReady_isr_Done;
			}

			/* for user-defined real-time Aspect Ratio Correction, we need
			 * to validate the user settings by _isr functions; if the check
			 * fails, we ignore those settings. */
			BVDC_P_Window_ValidateRects_isr(hSource->ahWindow[i], pNewPic);

			/* MosaicMode: don't adjust DstRect; only adj sub-rects SrcCnt and SclOut; */
			if((0 == ulPictureIdx)
#if BVDC_P_SUPPORT_MOSAIC_MODE
				||
			    (hWindow->stCurInfo.bMosaicMode &&
			     (pNewPic->ulChannelId < hWindow->stCurInfo.ulMosaicCount) &&
			     (hSource->bPictureChanged ||
			      BVDC_P_FIELD_DIFF(&hWindow->stCurInfo.astMosaicRect[pNewPic->ulChannelId],
			                       &hWindow->stCurInfo.astMosaicRect[pCurPic->ulChannelId], ulWidth) ||
			      BVDC_P_FIELD_DIFF(&hWindow->stCurInfo.astMosaicRect[pNewPic->ulChannelId],
			                       &hWindow->stCurInfo.astMosaicRect[pCurPic->ulChannelId], ulHeight)))
#endif
			   )
			{
				uint32_t ulRectIdx;

				/* MosaicMode: if the 1st picture's channel id exceeds mosaic count, we still
				   need to adjust the rectangle for scanout; capture can drain it; so take
				   mosaic rect 0; */
#if BVDC_P_SUPPORT_MOSAIC_MODE
				if(pNewPic->ulChannelId >= hWindow->stCurInfo.ulMosaicCount)
				{
					ulRectIdx = 0;
				}
				else
#endif
				{
					ulRectIdx = pNewPic->ulChannelId;
				}

				/* in mosaic mode, even if bPictureChanged==false, we still need to
				 * do initMuteRect or AdjustRectangles, because mosaic pic size might
				 * be diff from prev mosaic pic */
				if(pNewPic->bMute)
				{
					/* If source is muted, VDC will ignore rectangle clipping */
					if(hSource->bPictureChanged || hSource->stCurInfo.bMosaicMode)
					{
						BVDC_P_Window_InitMuteRec_isr(hSource->ahWindow[i],
							hWindow->hCompositor->stCurInfo.pFmtInfo, pNewPic, ulRectIdx);
					}
				}
				else
				{
					BVDC_P_Window_AdjustRectangles_isr(hSource->ahWindow[i], pNewPic, NULL,
						ulRectIdx);
				}
			}
		}

		if(!pNewPic->bCaptureCrc || pNewPic->bMute)
		{
			/* Get the source display rectangle. Combine the user pan-scan info
			 * from all the window that uses this source */
			if((0 == ulPictureIdx) || pCurDirty->stBits.bRecAdjust)
			{
				BVDC_P_Source_GetScanOutRect_isr(hSource, pNewPic, NULL, &hSource->stScanOut);
			}

			BVDC_P_Feeder_SetMpegInfo_isr(hSource->hMpegFeeder,
				hSource->stCurInfo.ulMuteColorYCrCb, pNewPic, &hSource->stScanOut);
		}
		else
		{
			BVDC_P_Rect stScanOut;

			/* if requested to capture CRC, feed the full size picture;
			 * Note, we don't care the display quality in this case; */
			stScanOut.lLeft = 0;
			stScanOut.lLeft_R = 0;
			stScanOut.lTop = 0;
			stScanOut.ulWidth  = pNewPic->ulSourceHorizontalSize & ~1;
			stScanOut.ulHeight = pNewPic->ulSourceVerticalSize & ~1;

			/* Get the source display rectangle. Combine the user pan-scan info
			 * from all the window that uses this source;
			 * Note, this is to avoid crashing BVN since some of downstream modules
			 * programming depend on this rectangle; */
			BVDC_P_Source_GetScanOutRect_isr(hSource, pNewPic, NULL, &hSource->stScanOut);
			BVDC_P_Feeder_SetMpegInfo_isr(hSource->hMpegFeeder,
				hSource->stCurInfo.ulMuteColorYCrCb, pNewPic, &stScanOut);
		}

		/* Now update base next picture to be display, for all windows
		 * connected to this source. */
		for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
		{
			/* SKIP: If it's just created or inactive no need to build ruls. */
			if(!hSource->ahWindow[i] ||
				BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
				BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
			{
				continue;
			}
			hWindow = hSource->ahWindow[i];
			BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

#if BVDC_P_SUPPORT_MOSAIC_MODE
			/* Non_MosaicMode window skips;
			   Note: simul mode, non-mosaic window needs to shrink capture size to
			   reduce bandwidth; */
			if((ulPictureIdx > 1) &&
			   (!hWindow->stCurInfo.bMosaicMode ||
			    hWindow->stCurInfo.ulMosaicCount < pNewPic->ulChannelId))
			{
				continue;
			}

			/* MosaicMode: count number of mosaic windows for combo trigger;
			   note that one of simul windows enables mosaic mode would require
			   combo trigger for the whole picture list; un-captured rects are
			   drained by CAP and need trigger to proceed to the next mosaic picture; */
			if(hSource->stCurInfo.bMosaicMode && (ulPictureIdx == 0))
			{
				/* increment count to set combo trigger */
				ulMosaicWindows++;

				/* get the maxim mosaic count among windows to config proper trigger; */
				ulMosaicCount = BVDC_P_MAX(ulMosaicCount, hWindow->stCurInfo.ulMosaicCount);
				ulComboTrigBitMask |= 1 << hWindow->stCurResource.hCapture->eTrig;
				if(ulMosaicCount == hWindow->stCurInfo.ulMosaicCount)
				{
					eTrigger = hWindow->stCurResource.hCapture->eTrig;
				}
			}
#endif
			/* MosaicMode: need to pass the sub-window index to build Capture RUL; note here we only care master
			    RUL's exec status; */
			/* PsF: no window write state change while no scanout; */
			if(!bNoScanout)
			{
				BVDC_P_Window_Writer_isr(hSource->ahWindow[i], pNewPic, NULL,
					hSource->eNextFieldId, &stMasterList, ulPictureIdx);
			}

			/* note: mfd rul is built together with other modules in the vnet in window */
		}

		if(pCurDirty->stBits.bRecAdjust || hSource->bWait4ReconfigVnet)
		{
			/* Gather window information after vnetmode is determined */
			BVDC_P_Source_GetWindowVnetmodeInfo_isr(hSource);
		}

		/* If this source is sync-lock with a compositor/vec we'll need to
		 * build RUL for that VEC, and CMP.
		 * Note, we need to handle field swap for captured sync-locked src to be displayed
		 * correctly. */
		/* MosaicMode: only build playback side RUL once for master RUL! */
		if(hSource->hSyncLockCompositor && (0 == ulPictureIdx))
		{
			/* FIELDSWAP 4): swap again with reader to display correct polarity; */
			BVDC_P_ReadListInfo_isr(&stList, hList);
			BVDC_P_Compositor_BuildSyncSlipRul_isr(hSource->hSyncLockCompositor,
				&stList, (hSource->bFieldSwap) ?
				BVDC_P_NEXT_POLARITY(hSource->eNextFieldId) : hSource->eNextFieldId,
				false /* bBuildCanvasCtrl */);
			BVDC_P_WriteListInfo_isr(&stList, hList);

			/* PsF: mark the initial chop location; */
			if(hSource->bPsfScanout)
			{
				stList.ulPsfMark = (uint32_t)(stList.pulCurrent - stList.pulStart);
			}
		}

		/* Read plist master ? slave ? */
		BVDC_P_ReadListInfo_isr(&stList, BDC_P_LIST_SEL(hList, hListSlave, ulPictureIdx));

		/* For each window using this SOURCE, If the window is sync-lock to this
		 * source build both the Writer & Reader modules.  If it's slip that means
		 * somebody else already building the Reader we're only need to build the
		 * writer. */
		/* MosaicMode: simul-windows use combined capture triggers; */
		for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
		{
			/* SKIP: If it's just created or inactive no need to build ruls. */
			if(!hSource->ahWindow[i] ||
			   BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
			   BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
			{
				continue;
			}

			hWindow = hSource->ahWindow[i];
			BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

#if BVDC_P_SUPPORT_MOSAIC_MODE
			/* Non_MosaicMode window skips; */
			if((ulPictureIdx > 1) &&
			   (!hWindow->stCurInfo.bMosaicMode ||
			    hWindow->stCurInfo.ulMosaicCount < pNewPic->ulChannelId))
			{
				continue;
			}
#endif
			/* build both slots RUL's if interlaced src format and window state or mode is in
			 * the transition to workaround non-alternating trigger pattern;
			 * during vnet changes, build double RUL's to commit change
			 * and get rid of change's RUL footprint afterwards; */
			bBuildBoth |= hWindow->stCurInfo.stDirty.stBits.bShutdown ||
				hWindow->stCurInfo.stDirty.stBits.bReConfigVnet;

			/* if window is already shutdown, don't need to build the shutdown RUL again
			 * to avoid killing the next established window path in case of variable
			 * isr latency doesn't promptly remove it from the slot next time; */
			if(BVDC_P_State_eShutDown != hWindow->stCurInfo.eWriterState)
			{
				/* in case we just got the sync-lock and the new locked cmp will has
				 * slip to lock transition, then we know that the new synclock cmp
				 * will build the window reader RUL in the next vsync to retain the
				 * consistent timing based on the same vec, so we only need
				 * to build writer side here; */
				if(hWindow->bSyncLockSrc)
				{
					/* MosaicMode: playback side only build RUL once!! */
					BVDC_P_Window_BuildRul_isr(hWindow, &stList, hSource->eNextFieldId,
						!bNoScanout,        /* writer*/
						/* MosaicMode: only need to program reader once; */
						(0 == ulPictureIdx) /* reader */,
						(0 == ulPictureIdx) /* CanvasCtrl */ );
					bBuiltCanvasCtrlWithWin = (0 == ulPictureIdx);
				}
				else
				{
					BVDC_P_Window_BuildRul_isr(hWindow, &stList, hSource->eNextFieldId,
						!bNoScanout,  /* writer*/
						false         /* reader */,
						false         /* CanvasCtrl */ );
				}
			}
		}

		/* build compositor convas ctrl after syncLock window RUL is built */
		if(hSource->hSyncLockCompositor && (0 == ulPictureIdx) && !bBuiltCanvasCtrlWithWin)
		{
			BVDC_P_Compositor_BuildConvasCtrlRul_isr(hSource->hSyncLockCompositor, &stList);
		}

		/* Update pList */
		BVDC_P_WriteListInfo_isr(&stList, BDC_P_LIST_SEL(hList, hListSlave, ulPictureIdx));

		/* save the previous picture's CRC key flag, to be used on next vsync;
		 * update current pictures. */
		if(!pCurPic->bMute)
		{
			hSource->bCaptureCrc  = pCurPic->bCaptureCrc;
			hSource->stSourceCbData.ulIdrPicId   = pCurPic->ulIdrPicID;
			hSource->stSourceCbData.lPicOrderCnt = pCurPic->int32_PicOrderCnt;
			hSource->stSourceCbData.eSourcePolarity = pCurPic->eSourcePolarity;
		}
		else
		{
			/* if muted, don't capture CRC! */
			hSource->bCaptureCrc  = false;
		}

		/* context swap:
		 * Note this may not be optimal but can avoid unnecessary bug and confusion later! */
		*pCurPic = *pNewPic;

		/* reset flag */
		hSource->bPictureChanged = false;
		hSource->bRasterChanged = false;

#if BVDC_P_SUPPORT_MOSAIC_MODE
		/* if combo trigger for the slave slot's trigger */
		if((ulMosaicWindows > 1) && (ulPictureIdx == 0))
		{
			eTrigger = hSource->eCombTrigger;

			BRDC_SetComboTrigMode_isr(hSource->hVdc->hRdc, eTrigger, BRDC_ComboTrigMode_eAllTriggers);
			BRDC_SetComboTriggers_isr(hSource->hVdc->hRdc, eTrigger, ulComboTrigBitMask);
		}
#else
		BSTD_UNUSED(ulComboTrigBitMask);
		BSTD_UNUSED(ulMosaicWindows);
#endif

		/* increment picture index for the next round;
		   if input pictures are more than the mosaic count, drop it; */
		if(++ulPictureIdx >= ulMosaicCount)
		{
			break;
		}

		/* Exhaust the picture list; if input pictures are less than mosaic count,
		   ignore the extra mosaics;
		   Note: window shutdown process shall not loop since window state machine
		   relies on the RUL execution status;  */
		if(!(pNewPic = pNewPic->pNext) || bBuildBoth)
		{
			break;
		}

		/* if channel id is larger than mosaic count, ignore the rest of picture list */
		if(pNewPic->ulChannelId >= ulMosaicCount)
		{
			break;
		}

		/* Only mosaic mode can go beyond this point; do it once; */
		if(ulPictureIdx == 1)
		{
			/* MosaicMode: get next slave slot and list */
			BVDC_P_SRC_GET_NEXT_SLAVE_RUL_IDX(hSource);
			hSlotSlave = hSource->hSlotSlave;

			/* Get the current slave list pointed by ulSlaveRulIdx */
			hListSlave = hSource->ahListSlave[hSource->ulSlaveRulIdx];

			BDBG_ASSERT(hSlotSlave);
			BDBG_ASSERT(hListSlave);

			/* Update the status of last executed RUL.
			 * Only need to check the master RUL exec status; */
			BRDC_Slot_UpdateLastRulStatus_isr(hSlotSlave, hListSlave, false);
			BRDC_List_SetNumEntries_isr(hListSlave, 0);

			/* clear the master flag to avoid building capture timestamp rul */
			stList.bMasterList = false;
		}

		/* config slave slot for the mosaics captures; */
		if(ulPictureIdx == 1)
		{
			/* use capture trigger to drive mosaic slave-RULs; also a place holder of the
			 * count for the next slave RUL! */
			BRDC_Slot_RulConfigSlaveTrigger_isr(hSlot, hSlotSlave, hList,
				eTrigger, true);

			/* link to the slave RUL's header */
			BRDC_List_RulSetSlaveListHead_isr(hList, hSlotSlave, hListSlave);
		}
		else
		{
			uint32_t ulScrap;

			/* place holder to config count of next slave RUL */
			BRDC_Slot_RulConfigSlaveTrigger_isr(hSlotSlave, hSlotSlave, hListSlave,
				eTrigger, true);

			/* link to the next slave RUL */
			ulScrap = BRDC_Slot_RulSetNextAddr_isr(hSlotSlave, hListSlave);

			/* calculate ulSubRulEntries */
			BRDC_List_GetNumEntries_isr(hListSlave, &ulSubRulEntries);
			ulSubRulEntries -= ulOldEntries + ulScrap;

			/* update config count with current slave RUL's size into the previous RUL's
			 * place holder; */
			BRDC_Slot_RulConfigCount_isr((ulPictureIdx == 2) ? hSlot : hSlotSlave, ulSubRulEntries);
		}
	}	while(true);

	/* Clear old dirty bits. */
	/* TODO: should we move this up, at least for vnet updating? */
	pOldDirty = &hSource->astOldDirty[hSource->eNextFieldId];

	if((BVDC_P_IS_DIRTY(pOldDirty)) &&
	   (!stMasterList.bLastExecuted))
	{
		BVDC_P_OR_ALL_DIRTY(pCurDirty, pOldDirty);
	}

	/* Copy cur dirty bits to "old" and clear cur dirty bits. */
	*pOldDirty = *pCurDirty;
	BVDC_P_CLEAN_ALL_DIRTY(pCurDirty);

	/* SWAP: Program the hw, to setup the nEntries for the slot with this new RUL. */
	/* MosaicMode: don't overwrite the first slot config of the RUL chain */
	if(ulPictureIdx >= 2)
	{
		/* chained sub-RULs tail: disable itself */
		BRDC_Slot_RulConfigSlaveListTail_isr(hSlotSlave, hListSlave);

		/* calculate ulSubRulEntries */
		BRDC_List_GetNumEntries_isr(hListSlave, &ulSubRulEntries);
		ulSubRulEntries -= ulOldEntries;

		/* update config count with current slave RUL's size into the previous RUL's
		 * place holder; */
		BRDC_Slot_RulConfigCount_isr((ulPictureIdx == 2)? hSlot : hSlotSlave, ulSubRulEntries);

		/* flush slave RULs */
		BRDC_Slot_FlushCachedList_isr(hSlotSlave, hListSlave);
	}

	/* Put on both slot for interlace */
	if(BAVC_Polarity_eFrame != hSource->eNextFieldId)
	{
		BRDC_Slot_Handle hOtherSlot = BVDC_P_SRC_GET_SLOT(hSource,
			BVDC_P_NEXT_POLARITY(hSource->eNextFieldId));

		/* PsF: build RUL to chop its own size! */
		if(hSource->bPsfScanout)
		{
			/* PsF: interlaced slots need to config both; */
			BRDC_Slot_RulConfigRulSize_isr(hSlot, hList, stList.ulPsfMark);
			BRDC_Slot_RulConfigRulSize_isr(hOtherSlot, hList, stList.ulPsfMark);
		}
		/* Note: to flush the cached RUL only once, call the Dual function
		   instead of two individual slot functions; */
		BRDC_Slot_SetListDual_isr(hSlot, hOtherSlot, hList);
	}
	else
	{
		/* PsF: build RUL to chop its own size! */
		if(hSource->bPsfScanout)
		{
			BRDC_Slot_RulConfigRulSize_isr(hSlot, hList, stList.ulPsfMark);
		}
		BRDC_Slot_SetList_isr(hSlot, hList);
	}
#if BVDC_P_SUPPORT_STG
	/* NRT STG host arm after the new RUL is installed to HW */
	if(hSource->hSyncLockCompositor &&
	   (BVDC_P_DISPLAY_USED_STG(hSource->hSyncLockCompositor->hDisplay->eMasterTg)) &&
	   (hSource->hSyncLockCompositor->hDisplay->stCurInfo.bStgNonRealTime))
	{
		BREG_Write32(hSource->hVdc->hRegister,
			BCHP_VIDEO_ENC_STG_0_HOST_ARM + hSource->hSyncLockCompositor->hDisplay->ulStgRegOffset, 1);
	}
#endif

MpegDataReady_isr_Done:

	/* apply done event early for sync locked windows and windows with
	   identical timing (has source driven by its compositor.  Fixes
	   ApplyChanges taking more than one vsync to complete.
	   Usually handled in BVDC_P_Window_UpdateState_isr() */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if(!hSource->ahWindow[i] ||
		    BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
		    BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
		{
			continue;
		}

		hWindow = hSource->ahWindow[i];
		BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

		if(((hWindow->hCompositor->hSyncLockSrc == hSource) ||
		    (hWindow->hCompositor->hForceTrigPipSrc == hSource)) &&
		    (hWindow->bSetAppliedEventPending) &&
			((BVDC_P_IS_CLEAN(&hWindow->stCurInfo.stDirty)) ||
		    (hWindow->stCurInfo.stDirty.stBits.bSrcPending && !hWindow->stCurInfo.stDirty.stBits.bShutdown)))
		{
			BKNI_SetEvent_isr(hWindow->hAppliedDoneEvent);
			hWindow->bSetAppliedEventPending = false;
			BDBG_MSG(("Window[%d] set apply done event", hWindow->eId));
		}
	}

  MpegDataReady_Done:
	hSource->hVdc->ulInsideCs--;
	BDBG_LEAVE(BVDC_Source_MpegDataReady_isr);
	return;
}

/* End of File */
