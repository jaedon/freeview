/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bvdc_source_priv.c $
* $brcm_Revision: Hydra_Software_Devel/430 $
* $brcm_Date: 10/5/12 4:44p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_source_priv.c $
* 
* Hydra_Software_Devel/440   5/8/13 11:47a yuxiaz
* SW7425-4736: Fixed kernel mode crash on 7468 and 7208.
* 
* Hydra_Software_Devel/430   10/5/12 4:44p yuxiaz
* SW7425-3928: Added synclock status in window callback.
* 
* Hydra_Software_Devel/429   9/20/12 3:36p syang
* SW7425-2172: MfdGfxCallback_isr keep calling MpegDataReady_isr for win
* shut down
* 
* Hydra_Software_Devel/428   9/13/12 4:00p syang
* SW7425-3896: pass ulOrigPTS for mfd/vfd gfx
* 
* Hydra_Software_Devel/427   9/12/12 3:41p syang
* SW7425-2172: more init for mfd gfx-src when surface is not set yet
* 
* Hydra_Software_Devel/425   8/23/12 3:22p vanessah
* SW7425-3797: fix the source assert error
* 
* Hydra_Software_Devel/424   8/16/12 2:43p tdo
* SW7445-8: Create Appframework emulation environment for 7445 A0
*
* Hydra_Software_Devel/423   8/16/12 10:10a tdo
* SW7445-8:  Fix assertion error due to new source
*
* Hydra_Software_Devel/422   8/13/12 4:03p tdo
* SW7425-3734: Fix coverity issues
*
* Hydra_Software_Devel/421   8/13/12 3:08p yuxiaz
* SW7425-3626, SW7425-3619: Replace uncache memory access with cache
* memory access: use the new API names in RDC.
*
* Hydra_Software_Devel/420   8/9/12 11:19a syang
* SW7425-2172: add SetSurface support for MFD gfx feeding
*
* Hydra_Software_Devel/419   6/14/12 3:43p syang
* SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
* prepare to add SetSurface support to MFD
*
* Hydra_Software_Devel/418   5/30/12 4:54p syang
* SW7425-2093: extra ApplyChange with 1st SetSurface no longer needed;
* vfd/cap time stamp related scratch registers now share with vfd's
* surface manager's; added vfd resource check; more clean up
*
* Hydra_Software_Devel/417   5/25/12 5:00p syang
* SW7425-2093: cleanup for vfd gfx code
*
* Hydra_Software_Devel/416   5/23/12 6:47p syang
* SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
* use gfxsurface sub-module
*
* Hydra_Software_Devel/415   5/9/12 10:54a syang
* SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
* error fix with GetSurface; use shared gfxsurface to manage gfx surface
*
* Hydra_Software_Devel/414   5/7/12 6:24p vanessah
* SW7418-96: fix the testfeature propogation error
*
* Hydra_Software_Devel/413   4/12/12 4:14p vanessah
* SW7425-2831: use the source for the field inversion handling in
* ignorepicture
*
* Hydra_Software_Devel/412   4/10/12 11:42a pntruong
* SW7435-80: Fixed typo.
*
* Hydra_Software_Devel/411   4/10/12 11:35a pntruong
* SW7435-80: Reduced BVDC_Open-time allocations from BMEM based on
* maximum usage.
*
* Hydra_Software_Devel/410   2/17/12 2:13p yuxiaz
* SW7231-635: Use BVDC_P_SCB_BURST_SIZE.
*
* Hydra_Software_Devel/409   2/17/12 11:49a yuxiaz
* SW7231-635: Added sw workaround for MFD.
*
* Hydra_Software_Devel/408   12/2/11 9:34a yuxiaz
* SW7425-968, SW7344-95: Fixed compile warning and coverity errors.
*
* Hydra_Software_Devel/407   12/1/11 4:33p yuxiaz
* SW7425-968, SW7344-95: Merged into mainline.: added independent source
* clipping of right window in 3D mode.
*
* Hydra_Software_Devel/406   11/23/11 4:47p tdo
* SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
*
* Hydra_Software_Devel/405   11/23/11 4:25p tdo
* SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
*
* Hydra_Software_Devel/404   9/14/11 11:47a yuxiaz
* SW7405-5490: Reduce slave slot to 1 for each MFD source in mosaic mode.
*
* Hydra_Software_Devel/403   8/27/11 7:23p hongtaoz
* SW7425-1132: replaced slip2lock boolean flag with integer counter to
* fix a timing sensitive hang conditiion when NRT sync-locked window is
* brought up;
*
* Hydra_Software_Devel/402   6/28/11 4:57p vanessah
* SW7425-686: change Srcpolarity when XVD and BVN refreshrate mismatch
*
* Hydra_Software_Devel/401   6/28/11 9:41a vanessah
* SW7425-686: Get rid of black frame
*
* Hydra_Software_Devel/400   6/27/11 7:58p pntruong
* SW7425-772: Fine tuned deinterlacer policy.
*
* Hydra_Software_Devel/399   6/26/11 5:56p vanessah
* SW7425-686: fix XVD and BVN fmt missync by mute MFD
*
* Hydra_Software_Devel/398   5/12/11 4:25p yuxiaz
* SW7231-74: Added software workaround for MADR alignment hung.
*
* Hydra_Software_Devel/397   5/11/11 3:50p pntruong
* SW7231-140: Correctly compute the MFD fieldswap flag.
*
* Hydra_Software_Devel/396   4/25/11 3:50p yuxiaz
* SW7425-348: Added software workaround for MADR alignment.
*
* Hydra_Software_Devel/395   4/1/11 7:38a vanessah
* SW7422-118: double buffer for GFD PI support
*
* Hydra_Software_Devel/394   3/31/11 3:30p pntruong
* SW7425-267: MADR input needs to be multiple of 4.
*
* Hydra_Software_Devel/393   2/4/11 4:21p yuxiaz
* SW7550-661: About shake of screen when NEXUS_VideoWindow_AddInput call.
*
* Hydra_Software_Devel/392   12/21/10 3:05p yuxiaz
* SW7422-28: Adjust to 3D format size change for HD_DVI.
*
* Hydra_Software_Devel/391   12/21/10 11:05a tdo
* SW7422-28: Adjust to 3D format size change
*
* Hydra_Software_Devel/390   12/20/10 5:46p yuxiaz
* SW7422-34: Added BVDC_Source_SetOrientation and
* BVDC_Source_GetOrientation.
*
* Hydra_Software_Devel/389   12/16/10 11:40p pntruong
* SW3548-2987: Added 1080p@30hz 3DOU_AS hdmi input support.
*
* Hydra_Software_Devel/388   11/24/10 1:41p jessem
* SW7422-82: Fixed run-time bugs.
*
* Hydra_Software_Devel/387   11/23/10 2:20p yuxiaz
* SW7422-35: Added 3D support in HD_DVI.
*
* Hydra_Software_Devel/386   11/11/10 7:31p albertl
* SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
* in dirty bits fit within union integer representation.  Fixed naming
* of dirty bits.
*
* Hydra_Software_Devel/385   11/11/10 7:13p hongtaoz
* SW7425-51: reduced combo trigger use in VDC to 1 per MFD source;
*
* Hydra_Software_Devel/384   11/11/10 12:06p hongtaoz
* SW7425-13: removed the unnecessary combo trigger setting at BVDC_Close;
*
* Hydra_Software_Devel/383   11/9/10 3:48p jessem
* SW7422-82: Simplified validation for R surface.
*
* Hydra_Software_Devel/382   11/1/10 4:24p jessem
* SW7422-82: Added stereo 3D support to VFD as source feature.
*
* Hydra_Software_Devel/381   11/1/10 1:14p vanessah
* SW7422-43:  merge 2D into 3D with a unified interface, change input
* data structure as const
*
* Hydra_Software_Devel/380   10/29/10 5:08p vanessah
* SW7422-43:  3D graphics render,merge 2D into 3D with a unified
* interface
*
* Hydra_Software_Devel/379   10/29/10 10:59a yuxiaz
* SW7422-31, SW7422-32, SW7422-39: More for 3D Video support.
*
* Hydra_Software_Devel/378   10/25/10 7:23p vanessah
* SW7422-43:  Handle GFD programming for 3D.
*
* Hydra_Software_Devel/377   10/18/10 10:05a yuxiaz
* SW7422-39: Added BAVC_SourceId_eMpeg2 support in VDC.
*
* Hydra_Software_Devel/376   10/12/10 2:45p jessem
* SW7420-173: Fixed coverity erros.
*
* Hydra_Software_Devel/375   10/11/10 12:55p jessem
* SW7420-173: Added support for VFD as source feature.
*
* Hydra_Software_Devel/374   10/7/10 11:46a yuxiaz
* SW7422-38: Handle VDC changes to memory allocation for 3D.
*
* Hydra_Software_Devel/373   9/29/10 5:22p yuxiaz
* SW7422-39: Added 3D video support in CAP and VFD.
*
* Hydra_Software_Devel/372   9/14/10 3:44p yuxiaz
* SW7422-28: Use new 3D formats
*
* Hydra_Software_Devel/371   5/7/10 7:20p albertl
* SW7125-364: Changed dirty bits to use union structure to avoid type-pun
* warnings
*
* Hydra_Software_Devel/370   5/3/10 4:57p syang
* SW7405-4239: 1). use uint64_t for aspect ratio numbers; 2). avoid even
* pixel rounding before aspect ratio correction calculation
*
* Hydra_Software_Devel/369   4/23/10 3:50p yuxiaz
* SW7405-4234: Kernel crash while playback 480p in PIP and 720p in Main
* and switch display format from 1080i to 1080p30.
*
* Hydra_Software_Devel/368   4/19/10 10:21p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic. Move
* BDBG_OBJECT_ID_DECLARE private header files instead of .c.
*
* Hydra_Software_Devel/367   4/7/10 11:35a tdo
* SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
*
* Hydra_Software_Devel/366   4/5/10 4:14p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic
*
* Hydra_Software_Devel/365   4/2/10 2:57p yuxiaz
* SW7405-3965: Mosaic decoder can't be attached to the window of the 2nd
* display.
*
* Hydra_Software_Devel/364   3/17/10 12:05p syang
* SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
* same as video; ensure that reader and writer vnetState match in sync-
* locked case; aligned width up to even for stride in feeder and cap;
* assert mosaic mode doesn't co-exist with dest cut;
*
* Hydra_Software_Devel/363   2/1/10 6:43p hongtaoz
* SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
* for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
* rate gap cases; reduced capture buffer allocation for interlaced
* pulldown case;
*
* Hydra_Software_Devel/BCM3D/3   1/29/10 6:57p hongtaoz
* SW3548-2680, SW3548-2681: removed 1470p custom formats since they are
* the same as 720p3D formats; added macro to wrap the 3548 specific
* legacy 3DTV format; reduced window buffer allocation needed for
* pulldown case; disable hddvi DE if invalid HVstart;
*
* Hydra_Software_Devel/BCM3D/2   1/15/10 4:53p hongtaoz
* SW3548-2680, SW3548-2681: added HDMI1.4 3D format 720p50 input support;
*
* Hydra_Software_Devel/BCM3D/1   12/17/09 6:09p hongtaoz
* SW3548-2680, SW3548-2681: added HDMI1.4 3D format detection and
* handling for 11-bit BVN;
*
* Hydra_Software_Devel/362   11/16/09 5:04p yuxiaz
* SW7405-3406: Clean up bMosaicMode and  bMosaicClearOutside.
*
* Hydra_Software_Devel/361   11/16/09 2:05p yuxiaz
* SW7405-3371: Reset combo triggers when destroy MPEG source.
*
* Hydra_Software_Devel/360   10/9/09 5:29p syang
* SW7405-2516, SW3548-2439, SW7405-3046: rewrite HSCL setinfo_isr and
* TryXferHrzSclToHscl_isr to do crop correctly; cleanup width alignment
* for DCX_HSIZE_WORKAROUND
*
* Hydra_Software_Devel/359   5/29/09 9:39a yuxiaz
* PR55364: Wait until all the windows finishing reconfig vnet.
*
* Hydra_Software_Devel/358   5/28/09 2:19p yuxiaz
* PR55364: Disable dithering when any testfeature1 is enabled
*
* Hydra_Software_Devel/357   5/22/09 11:33a tdo
* PR55364: Disable dithering when any test feature is enabled
*
* Hydra_Software_Devel/356   4/17/09 11:57a pntruong
* PR54064: Refactored common code for defered callback til bvn shutdown.
* Improved readability and extensibility of dirty bits.
*
* Hydra_Software_Devel/355   4/14/09 2:10p yuxiaz
* PR54096: Video and OSD both disappear if Input source is HDMI or
* Component of 1080I.
*
* Hydra_Software_Devel/354   4/10/09 6:29p hongtaoz
* PR54064: source pending callback must fire after window shutdown;
*
* Hydra_Software_Devel/353   4/6/09 3:42p syang
* PR  53484 , PR 52037: cleanup (rect, scl-ratio, ..) rounding code and
* AdjustRect code ; add 1 line delay to TV chip "early trigger"; change
* BVDC_P_DST_CUT_TOP_THRESH to 4 for 3548;
*
* Hydra_Software_Devel/352   4/3/09 4:34p yuxiaz
* PR53460: Optimal 444/422 conversion settings
*
* Hydra_Software_Devel/351   3/31/09 5:02p hongtaoz
* PR53687, PR53688: mpeg window creation should not take over synclock if
* the window's source or display is in sync-lock transition;
*
* Hydra_Software_Devel/350   3/19/09 11:36p pntruong
* PR53429: Corrected the anr/mad/hscl vnet connection.  Removed ununsed
* bAnrMadCombo flags.
*
* Hydra_Software_Devel/349   3/16/09 10:50p tdo
* PR45785, PR45789: Merge from MCVP branch
*
* Hydra_Software_Devel/7420_mcvp/6   3/5/09 1:49p tdo
* PR45785, PR45789: Fix compiling error
*
* Hydra_Software_Devel/7420_mcvp/5   3/5/09 1:44p tdo
* PR45785, PR45789: merge from main branch on 3/05/09
*
* Hydra_Software_Devel/348   3/4/09 7:03p pntruong
* PR52482: Vnet cannot be rebuilt when CVBS video in freezing then
* unplug/plug the input cable.
*
* Hydra_Software_Devel/7420_mcvp/4   3/2/09 4:22p tdo
* PR 45785, PR 45789: merge from main branch on 3/02/09
*
* Hydra_Software_Devel/347   3/2/09 3:46p yuxiaz
* PR52287: More update on 422-444 and 444-422 converters.
*
* Hydra_Software_Devel/7420_mcvp/3   2/26/09 11:18p tdo
* PR 45785, PR 45789: merge from main branch on 2/26/09
*
* Hydra_Software_Devel/346   2/26/09 6:11p yuxiaz
* PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
*
* Hydra_Software_Devel/7420_mcvp/2   2/25/09 10:00a tdo
* PR 45785, PR 45789: merge from main branch on 2/25/09
*
* Hydra_Software_Devel/345   2/24/09 11:00p pntruong
* PR52471: Addressed global (writable) data in nexus and magnum.
*
* Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:31p syang
* PR 45785, PR 45789: add PI support for new the new module  MCVP
* (MCTF+MCDI)
*
* Hydra_Software_Devel/344   1/7/09 11:47a pntruong
* PR49556: Initialize vdc with adc/pll power off.
*
* Hydra_Software_Devel/343   1/6/09 9:56a rpan
* PR50759: Take the capture buffer held by user into count when re-
* allocating buffers.
*
* Hydra_Software_Devel/342   12/4/08 5:24p pntruong
* PR50002: VDC: Added function to force source pending.
*
* Hydra_Software_Devel/341   10/9/08 5:29p syang
* PR 46891: add _isr to the name of func used in _isr context
*
* Hydra_Software_Devel/340   10/3/08 3:37p syang
* pr 47129, pr 45402 :  add static string for ext video format name
*
* Hydra_Software_Devel/339   10/3/08 12:21p syang
* pr 47129, pr 45402 :  1). improve  the VDC internally used videoFmtCode
* approximate; 2). return w, h, frmRate and bInterlaced with src-pending
* callback for non-enumerated mpeg-src video format
*
* Hydra_Software_Devel/338   9/15/08 9:17p pntruong
* PR46514: Upon destroyed and disabled slot disable adc if not used.
*
* Hydra_Software_Devel/336   9/9/08 5:31p syang
* PR45713:  for src-pending, src resume bit should be inited as 0
*
* Hydra_Software_Devel/335   9/2/08 6:05p pntruong
* PR46314, PR46316: Updated 3dcomb memory allocation, and added buffers
* alloc debug messages.
*
* Hydra_Software_Devel/334   8/22/08 10:28a rpan
* PR45979: Remove VBI passthru support.
*
* Hydra_Software_Devel/333   7/1/08 9:52a yuxiaz
* PR44042: KLOCWORK: bvdc_source_priv.c
*
* Hydra_Software_Devel/332   6/22/08 9:48a pntruong
* PR40213, PR41529: [VDEC] PCIN: interlaced can't be displayed correctly.
* Remove the sw workaround for supporting interlaced pc input in 3548.
* See pr 36216.
*
* Hydra_Software_Devel/331   5/28/08 3:35p pntruong
* PR42072: Make sure all windows get reconfigure on startfeed change.
*
* Hydra_Software_Devel/330   5/22/08 1:19p pntruong
* PR42475, PR41898: Rollback pr41898. Need re-revaluation of resource
* releasing.
*
* Hydra_Software_Devel/329   5/8/08 5:54p pntruong
* PR35549: Added more preparations for source pending.
*
* Hydra_Software_Devel/328   4/29/08 7:48p pntruong
* PR35549: Make room for source pending dirty bit.
*
* Hydra_Software_Devel/327   4/21/08 4:34p hongtaoz
* PR35398, PR40895: some compiler might generate floating point
* instructions;
*
* Hydra_Software_Devel/326   4/18/08 1:43p pntruong
* PR41898: Need better synchronization of window states.  Synchronize the
* releases of resource and vnetmode together in writer task.
*
* Hydra_Software_Devel/325   3/26/08 1:15p hongtaoz
* PR35398, PR40895: added ulVsyncRate and removed bGameModeDelayLags in
* the window lipsync callback structure; added BFMT_FREQ_FACTOR to unify
* Vsync rate and pixel frequency calculation;
*
* Hydra_Software_Devel/324   3/25/08 3:16p syang
* PR 40431: add complete assert for critical section protection among src
* _isr, dsp _isr, and ApplyChanges
*
* Hydra_Software_Devel/323   3/19/08 5:28p pntruong
* PR38442: Updated BVDC_Source_SetAdcConfiguration so that only set dirty
* bit on change is detected, and set bAdcSetting dirty bit.  This dirty
* bit will re-initialize vdec internal state machine, and briefly mute
* video.
*
* Hydra_Software_Devel/322   3/7/08 10:49a yuxiaz
* PR40366: Fixed KLOCWORK errors.
*
* Hydra_Software_Devel/321   2/21/08 5:01p pntruong
* PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
* Improved code readability.
*
* Hydra_Software_Devel/320   2/20/08 4:15p hongtaoz
* PR39154: added window API to toggle ANR; prepare for combo ANR/MAD
* feature on 3548;
*
* Hydra_Software_Devel/319   12/17/07 5:05p pntruong
* PR37992: Added main/pip with mixing 216Mhz and 108Mhz supports.
*
* Hydra_Software_Devel/317   12/17/07 1:14p pntruong
* PR37992: Added validation of duplicated adc usage, turn off unused adc,
* protect usage of pll and pc_in block.  Main/Pip work but more work
* needed for mixing 216Mhz and 108Mhz.
*
* Hydra_Software_Devel/316   11/29/07 1:48p pntruong
* PR36216: [PC]:Support component input formats with PC input.  Tighten
* const.
*
* Hydra_Software_Devel/315   11/28/07 4:47p pntruong
* PR37175: [VDEC][WXGA] 3Dcomb fails when input is PAL/PAL-N/PAL-M format
* with WXGA.  Need to allocate correct amount of memory for PAL.
*
* Hydra_Software_Devel/314   11/20/07 11:07p tdo
* PR36889: Remove strictly alias warnings
*
* Hydra_Software_Devel/313   11/5/07 8:10p pntruong
* PR36462: PR33011:If CVBS signal doesn't contain burst signal, need S/W
* assume a default format
* PR36563:for RF channel, mute the video 20 vysnc when video format
* change. This is to fix the transition that cause by RF auto detection:
* PAL-M/PAL-Nc/NTSC
* PR33797:to fix the transtion and anr hanging up during the switch
* channel
* PR36462: decrease the delay for VCR detection
*
* Hydra_Software_Devel/312   11/2/07 10:43a syang
* PR 29617:  scanTop align to 4 lines only for 4:2:0 format
*
* Hydra_Software_Devel/311   10/30/07 7:25p pntruong
* PR34239: Allow dynamically loading of vec custom timing.
*
* Hydra_Software_Devel/310   10/26/07 6:21p syang
* PR 29617:  still more fix for smooth vert move
*
* Hydra_Software_Devel/309   10/26/07 5:47p pntruong
* PR36469: Relax tolerance to pickup 480p@60Hz from Master Pattern
* Generator, as other leading could also detect and display it.
*
* Hydra_Software_Devel/308   10/26/07 3:45p syang
* PR 29617: add support for smooth vertical mov with MAD
*
* Hydra_Software_Devel/307   10/22/07 11:03a pntruong
* PR36216: Initialized new fields.
*
* Hydra_Software_Devel/306   9/21/07 4:00p hongtaoz
* PR34955: added Progressive Segmented Frame(PsF) 1080p support; fixed
* 1080p pass-through condition; fixed bvdc_dbg register access error;
* fixed general progressive capture interlaced playback feeder
* programming problem;
*
* Hydra_Software_Devel/305   9/17/07 5:47p pntruong
* PR34817: SD format(480X480) decoding problem.  Updated hddvi code to
* use the h/v size from hdr/xvd.
*
* Hydra_Software_Devel/304   8/30/07 12:04p pntruong
* PR33614:  Removed vnet reset, and delay auto-trigger to avoid re-enable
* errors in anr.
*
* Hydra_Software_Devel/303   8/13/07 5:42p syang
* PR 33614: add a vnet-reconfig for 3D comb off to on if anr is enabeld
*
* Hydra_Software_Devel/302   8/8/07 6:38p syang
* PR 29617: make sure horiz move does not change MAD width when
* SrcCnt.width is odd
*
* Hydra_Software_Devel/301   8/1/07 4:40p syang
* PR 29617: make sure horiz move does not change MAD width
*
* Hydra_Software_Devel/300   7/19/07 2:38p tdo
* PR33034: Modify DNR API to extend MNR/BNR/DCR filter level
*
* Hydra_Software_Devel/299   6/29/07 11:47a pntruong
* PR32503: Updated mad settings for oversampling.
*
* Hydra_Software_Devel/298   6/27/07 10:03p pntruong
* PR32503: [VDEC] Add oversampling support.
*
* Hydra_Software_Devel/297   6/27/07 1:47p pntruong
* PR32503: Cleanup unused code.  Prepare for 480i/480p vdec oversample.
*
* Hydra_Software_Devel/296   6/27/07 12:33p tdo
* PR32489: Add support for custom adjust DNR
*
* Hydra_Software_Devel/295   6/22/07 5:10p syang
* PR 32198: align ScanOut top / bot to 4-line boundary for 4:2:0 format
*
* Hydra_Software_Devel/294   6/21/07 3:39p syang
* PR 32440: clean up inefficient "if" statement
*
* Hydra_Software_Devel/293   6/15/07 10:17p pntruong
* PR 31944: Removed debug messages.
*
* Hydra_Software_Devel/292   6/13/07 9:46a syang
* PR 31944: put ANR K value table inro seperate file; change to use
* SnDbAdj from 5 level adj
*
* Hydra_Software_Devel/291   6/11/07 7:26p tdo
* PR31947: provide API to allow user setting the Qp value for DNR
*
* Hydra_Software_Devel/290   5/15/07 10:01a pntruong
* PR30745: Need to re-initialized rul list on initialization.
*
* Hydra_Software_Devel/289   5/11/07 10:48a pntruong
* PR30617: ANR causes undesired "raining" effect.
*
* Hydra_Software_Devel/288   5/10/07 3:23p syang
* PR 30617: add bStartFeed to src for muting writer
*
* Hydra_Software_Devel/287   5/7/07 6:04p pntruong
* PR28899: C0: BVN: VFD 10-bit 444 to 422 Mode Transition Requires Soft
* Reset.
*
* Hydra_Software_Devel/286   5/1/07 4:43p syang
* PR 28541: 1) changed non-linear scl api; 2). carry sub-pxl accuracy
* through the pipe line by SrcHStep and SrcVStep
*
* Hydra_Software_Devel/285   5/1/07 10:33a hongtaoz
* PR30075, PR30170: simplified buffer release algorithm; always release
* from the last active buffer; refactored window buffer code; the
* minimum clear rect size is required for bClearMosaicOutside;
*
* Hydra_Software_Devel/284   4/23/07 1:37p hongtaoz
* PR30085: removed source slots cleanup due to the field swap since we
* always assign single RUL to both t/b slots; turn off slots exec
* tracking for the source slots under cleanup to prevent losing the
* critical RUL;
*
* Hydra_Software_Devel/283   4/19/07 1:48p yuxiaz
* PR 26052: Add CSC support in MFD on 7400 B0.
*
* Hydra_Software_Devel/282   4/6/07 11:37a pntruong
* PR29194: Added the new coefficients.
*
* Hydra_Software_Devel/281   4/5/07 4:56p albertl
* PR29194:  Added BVDC_Window_SetCoefficientIndex and input and index
* matching support to scaler coefficient tables.
*
* Hydra_Software_Devel/280   4/4/07 7:09p tdo
* PR29444: Move Source_AnalogDataReady_isr out of vdec code to share
* between VDEC and 656 input.
*
* Hydra_Software_Devel/279   4/2/07 11:31a syang
* PR 29243: clean up: take off the obselete old-vnet-code
*
* Hydra_Software_Devel/278   3/29/07 9:29a yuxiaz
* PR 28391: Initialize ulInputPort in BVDC_P_Source_Init.
*
* Hydra_Software_Devel/277   3/24/07 1:58p pntruong
* PR29042: Delay the allocation 3d comb memory till 3d comb is actually
* needed.
*
* Hydra_Software_Devel/276   3/23/07 11:18a tdo
* PR 28392: Provide API to customize source CSC
*
* Hydra_Software_Devel/275   3/5/07 6:42p syang
* PR 28298: move DNR's Acquire/Release/SetInfo to window (to be
* consistant with other sub-modules)
*
* Hydra_Software_Devel/274   2/23/07 6:31p tdo
* PR 27970: Share LPB and FCH if source is the same to eliminate the out
* of resource error when number of LPB and FCH are limited
*
* Hydra_Software_Devel/273   2/6/07 7:01p pntruong
* PR23482: Fixed framerate detection for 1080p@30Hz, 1080p@24Hz, and
* 1080p@25Hz.
*
* Hydra_Software_Devel/272   2/6/07 4:43p tdo
* PR 23482: fix typo, use 656In1 instead of 656In0 for source ID
*
* Hydra_Software_Devel/271   2/6/07 11:29a yuxiaz
* PR 25995: Use new vnet code for DNR.
*
* Hydra_Software_Devel/270   2/3/07 3:43p pntruong
* PR23482:  Optimized framerate detection.
*
* Hydra_Software_Devel/269   2/2/07 7:38p pntruong
* PR23482: Added VDEC/HDDVI Frame Rate Detection in VDC.  Also refactored
* detection to use in both hddvi and vdec.  Added support for all
* framerate code suported in bavc.h.
*
* Hydra_Software_Devel/268   1/31/07 3:40p pntruong
* PR23222: Handle default case where user does not set format.
*
* Hydra_Software_Devel/267   1/22/07 11:13a pntruong
* PR22579: Bringup of HD_DVI input (dual core).  Redo hddvi code.
*
* Hydra_Software_Devel/266   1/18/07 5:07p yuxiaz
* PR 23202: Fix dynamic RTS due to user source format change.
*
* Hydra_Software_Devel/265   1/11/07 7:03p syang
* PR 22569: 1st time check in after anr building vnet correctly and not
* hang the sys. But display is still static
*
* Hydra_Software_Devel/264   1/11/07 10:28a yuxiaz
* PR 26313: Change DNR API to source based API.
*
* Hydra_Software_Devel/263   1/10/07 3:31p yuxiaz
* PR 26313: Change DNR API to source based API: Remove
* BVDC_Window_SetDnrFilterLevel and BVDC_Window_GetDnrFilterLevel. Add
* BVDC_Source_SetDnrFilterLevel, BVDC_Source_GetDnrFilterLevel,
* BVDC_Source_SetSplitScreenMode and BVDC_Source_GetSplitScreenMode.
*
* Hydra_Software_Devel/262   12/18/06 11:29p pntruong
* PR22577: Merged back to mainline.
*
* Hydra_Software_Devel/Refsw_Devel_3563/19   12/11/06 5:23p albertl
* PR24587:  Added 3rd GFX Feeder source.
*
* Hydra_Software_Devel/Refsw_Devel_3563/18   12/5/06 6:10p pntruong
* PR 26151: Refactor the shared dnr.
*
* Hydra_Software_Devel/Refsw_Devel_3563/17   11/30/06 11:04a yuxiaz
* PR23638: VDEC sources need drain on 3563.
*
* Hydra_Software_Devel/Refsw_Devel_3563/16   11/29/06 5:37p pntruong
* PR 26151:  Release the shared heap memory if mad previous aquired.
*
* Hydra_Software_Devel/Refsw_Devel_3563/15   11/28/06 3:35p yuxiaz
* PR23638: Rewrite vnet drain code by using shared resource manager.
*
* Hydra_Software_Devel/Refsw_Devel_3563/14   11/22/06 3:52p syang
* PR 26151: re-write shared resource manager. update letter box, mad, dnr
* shared-scl acquire/release code accordingly
*
* Hydra_Software_Devel/Refsw_Devel_3563/13   11/20/06 3:20p tdo
* PR 23174:  Add VDC function to overwrite HD_DVI color matrix to output
* fix color
*
* Hydra_Software_Devel/Refsw_Devel_3563/12   11/15/06 5:54p albertl
* PR24587:  Added downsample source to 7440.
*
* Hydra_Software_Devel/Refsw_Devel_3563/11   11/13/06 3:34p hongtaoz
* PR20716: separate displays/compsoitors for loops;
*
* Hydra_Software_Devel/Refsw_Devel_3563/10   11/8/06 2:26p jessem
* PR 22987, PR 17846: Re-merge in changes that was previously backed out.
*
* Hydra_Software_Devel/Refsw_Devel_3563/9   10/26/06 3:17p yuxiaz
* PR25181: Merge in fix from main branch: VDC Buffer Allocation Fragments
* HD Buffer Pool and Causes Out of Memory Assertion.
*
* Hydra_Software_Devel/Refsw_Devel_3563/8   10/19/06 10:59a pntruong
* PR23225: VDEC - PC input.  Added code to program the pll clock for
* different pc input format.
*
* Hydra_Software_Devel/Refsw_Devel_3563/7   10/11/06 2:01p pntruong
* PR 22987, PR 17846: Back out previous changes as it still need further
* validations.
*
* Hydra_Software_Devel/Refsw_Devel_3563/4   9/29/06 8:31a pntruong
* PR24540, PR20875, PR11739, PR24376, PR18329: Take in changes from
* mainline.
*
* Hydra_Software_Devel/258   9/26/06 10:27a pntruong
* PR24540: Default state of ADCs can disable VDEC.
*
* Hydra_Software_Devel/257   9/20/06 12:20p jessem
* PR 23717: Added support for dynamic MAD buffer allocation.
*
* Hydra_Software_Devel/256   9/14/06 3:40p yuxiaz
* PR24262: Separate VDC 656 code from VDEC for new 656 core.
*
* Hydra_Software_Devel/255   8/30/06 2:44p pntruong
* PR15086:  Fixed typo in hddvi input port and get aspect ratio.
*
* Hydra_Software_Devel/254   8/30/06 10:21a syang
* PR 23977:  avoid some MAD code for 7118
*
* Hydra_Software_Devel/253   8/18/06 4:29p yuxiaz
* PR20875: Added BVDC_P_SUPPORT_10BIT_BVN. Added raster change detection
* for source. Added BVN data mode change detection for window.
*
* Hydra_Software_Devel/252   8/15/06 6:16p hongtaoz
* PR18233, PR20716: added support for mosaic mode - missing channels;
* assume picture list's channel id are in ascending order; single
* channel will not result in full-screen automatically;
*
* Hydra_Software_Devel/251   8/15/06 11:43a syang
* PR 23023: cleanup scale code: align width only when needed, choose
* vertical coeff basing on correct src height, avoid negative lLeft/lTop
* (that causes overflow)
*
* Hydra_Software_Devel/250   8/14/06 7:57p pntruong
* PR23177: RDC module bringup.  More cleanup of nested ifdefs.
*
* Hydra_Software_Devel/249   8/4/06 4:51p yuxiaz
* PR23307: Share vnet drain between 656 and HD_DVI sources.
*
* Hydra_Software_Devel/248   7/31/06 6:11p hongtaoz
* PR22499: added DCR filter setting as a part of DNR block;
*
* Hydra_Software_Devel/247   7/23/06 4:24p hongtaoz
* PR22437: 1st sync-lock source _isr cleans up display slip RUL; when a
* mpeg source's last window is disconnected, clean up source slots
* immediately; when mpeg source is muting, don't perform 50->60
* conversion to avoid video freeze or flash artifact;
*
* Hydra_Software_Devel/246   7/18/06 1:33p syang
* PR 21963: ensure ulWindows copied from src new info to cur info, to
* avoid  win bUserAppliedChanges and src bPictureChanged being always
* set true with ApplyChanges (that causes extra busy with brutus replay
* with gfx updating frequently )
*
* Hydra_Software_Devel/245   7/17/06 3:23p yuxiaz
* PR21489: Bring up 656 in on 7401 B0.
*
* Hydra_Software_Devel/244   7/17/06 1:01p hongtaoz
* PR22713: increased the synlock transfer semaphore count; display _isr
* cleans up source slots only when the semaphore count is 0;
*
* Hydra_Software_Devel/243   6/14/06 1:44p syang
* PR 21689: add support for 7118
*
* Hydra_Software_Devel/242   5/26/06 3:43p pntruong
* PR20642: Refactored handling of hList.
*
* Hydra_Software_Devel/240   5/10/06 3:27p jessem
* PR 17530: Modified calls to BVDC_P_Feeder_Create to reflect new
* function signature.
*
* Hydra_Software_Devel/239   4/24/06 4:04p yuxiaz
* PR15161: Fixed VDC _isr naming violations in bvdc_buffer_priv.c and
* bvdc_bufferheap_priv.c.
*
* Hydra_Software_Devel/238   4/4/06 1:50p hongtaoz
* PR20403: add triple displays for 7400;
*
* Hydra_Software_Devel/237   3/23/06 2:29p jessem
* PR 19517: Removed setting of  bUse24HzOr30HzDisplay flag. Checks for
* 1080p height and display format rate compatibility will be
* incorporated into PsF feature. Please see PR 20384.
*
* Hydra_Software_Devel/236   3/22/06 5:33p jessem
* PR 19517: Added bUse24HzOr30HzDisplay field in BVDC_P_Source_Info to
* use with checking the compatibility of the source height with the
* display format rate found in BVDC_P_Source_RefreshWindow_isr. Also
* enclosed call to BVDC_P_Source_RefreshWindow_isr with critical section
* in BVDC_P_Source_Validate.
*
* Hydra_Software_Devel/235   3/16/06 11:25a syang
* PR 20029: refine src scan out rect width 4 pixels and height 2 pixels
* rounding code (fix capture assert problem)
*
* Hydra_Software_Devel/234   3/1/06 9:28a jessem
* PR 19582: Added BVDC_P_Source_MpegGetStatus_isr to check the status of
* the source whether there is video detected or not.
*
* Hydra_Software_Devel/233   2/2/06 12:12p hongtaoz
* PR19082: fixed sub-pixel panscan assertion;
*
* Hydra_Software_Devel/232   1/31/06 5:20p pntruong
* PR17778: Take in changes for new hddvi.
*
* Hydra_Software_Devel/228   1/25/06 10:58p pntruong
* PR19172: Black-screen with latest VDC on 97398.  The mosaic added 4
* more slots for capture compositing surface  this pushes the hddvi slot
* out, and not all slot has track execution.  This causes the update
* format rul of hddvi not properly execute and cause hddvi not to lock.
* Fixes by freeing non-used frame slot in vdec/656/hddvi, and free up
* more rdc vars to be used for track execution.
*
* Hydra_Software_Devel/227   1/17/06 4:19p hongtaoz
* PR19082: first compiled for 7400;
*
* Hydra_Software_Devel/226   1/12/06 1:34p hongtaoz
* PR18233: added mosaic mode support;
*
* Hydra_Software_Devel/225   1/5/06 2:27p hongtaoz
* PR18753: initiate window shutdown when ADC setting changes to avoid
* color swap with MAD at capture side;
*
* Hydra_Software_Devel/224   11/8/05 12:39p pntruong
* PR15086: Handled generic pixel repeat, add new API to select between
* hdr and external hd_dvi signal.
*
* Hydra_Software_Devel/223   11/3/05 10:21a pntruong
* PR17039: BVDC_Source_SetAutoFormat doesn't work with format changes on
* dp962.
*
* Hydra_Software_Devel/222   11/2/05 4:48p pntruong
* PR15086:  Merged to mainline.
*
* Hydra_Software_Devel/PR15086/2   10/28/05 3:52p pntruong
* PR17568: return CRC callback at the correct vsync;
*
* Hydra_Software_Devel/221   10/28/05 1:52p hongtaoz
* PR17568: return CRC callback at the correct vsync;
*
* Hydra_Software_Devel/220   10/5/05 6:43p syang
* PR 16391: avoid to mess-up RUL when gfx sur is set with gfx win muted
* or with src disconnected from win.
*
* Hydra_Software_Devel/219   9/21/05 6:32p albertl
* PR16459:  Display format code restructured, moved to
* bvdc_displayfmt_priv.c and bvdc_displayfmt.h, and changed to be  table
* driven  to allow ease of adding new formats.
*
* Hydra_Software_Devel/218   9/16/05 2:30p hongtaoz
* PR16812: disables VDEC/656in BVB_OUTPUT if they are not connected with
* any window; clear ulTransferLock at display_isr in case the critical
* mmpeg callback is missing during synclock transfer;
*
* Hydra_Software_Devel/217   9/1/05 6:24p jasonh
* PR 16967: Removing XVD source
*
* Hydra_Software_Devel/216   8/31/05 4:05p syang
* PR 15146: fix a problem with src aspectRatio auto-updated
*
* Hydra_Software_Devel/215   8/31/05 1:34p syang
* PR 15146: clean *pActIsrSet after isr settings are copied to newInfo
*
* Hydra_Software_Devel/214   8/31/05 1:11p syang
* PR 12132, PR 14862, PR 15146: updated func desc in bvdc.h related to
* src/disp videoFmt/aspectRatio; added source aspect ratio override;
* added activated _isr src setting to newInfo copy at ApplyChanges.
*
* Hydra_Software_Devel/213   8/24/05 12:54p syang
* PR 12132, PR 14862, PR 15146: added src and dst aspect ratio canvas
* clip
*
* Hydra_Software_Devel/212   8/19/05 2:54p hongtaoz
* PR15888: adding support for dynamic switch on/off shared scaler on
* bypass window;
*
* Hydra_Software_Devel/211   8/18/05 1:19p pntruong
* PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
* Improved bandwidth for cropping/scaler/capture/playback.   Unified
* window shutdown sequence for destroy, reconfigure mad/scaler, and/or
* reconfigure result of source changes.  And miscellances fixes from
* above PRs.
*
***************************************************************************/
#include "bstd.h"
#include "bvdc_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_dnr_priv.h"
#include "bvdc_feeder_priv.h"
#include "bvdc_gfxfeeder_priv.h"
#include "bvdc_hddvi_priv.h"
#include "bvdc_vdec_priv.h"
#include "bvdc_downsample_priv.h"
#include "bvdc_mad_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_656in_priv.h"
#include "bfmt.h"

#if (BVDC_P_MADR_PICSIZE_WORKAROUND)
#include "bvdc_mcvp_priv.h"
#include "bvdc_mcdi_priv.h"
#endif

BDBG_MODULE(BVDC_SRC);
BDBG_OBJECT_ID(BVDC_SRC);

/****************************************************************************/
/* Number of entries use this to loop, and/or allocate memory.              */
/****************************************************************************/
#define BVDC_P_FRT_COUNT_INTERLACED \
	(sizeof(s_aIntFramerateTbl)/sizeof(BVDC_P_FrameRateEntry))

#define BVDC_P_FRT_COUNT_PROGRESSIVE \
	(sizeof(s_aProFramerateTbl)/sizeof(BVDC_P_FrameRateEntry))

/****************************************************************************/
/* Making an entry for Frame Rate                                           */
/****************************************************************************/
#define BVDC_P_MAKE_FRT(e_frame_rate, vert_freq, ppt)                        \
{                                                                            \
	BAVC_FrameRateCode_##e_frame_rate,                                       \
	((BVDC_P_216_SYSCLK / (vert_freq * 1000)) * (ppt)),                      \
	(#e_frame_rate)                                                          \
}

/****************************************************************************/
/* Making an entry for src params                                           */
/****************************************************************************/
#define BVDC_P_MAKE_SRC_PARAMS(e_source_id, e_trig_0, e_trig_1)              \
{                                                                            \
	BAVC_SourceId_##e_source_id,                                             \
{                                                                        \
	BRDC_Trigger_##e_trig_0,                                             \
	BRDC_Trigger_##e_trig_1,                                             \
	BRDC_Trigger_UNKNOWN                                                 \
}                                                                        \
}

#define BVDC_P_MAKE_SRC_PARAMS_NULL(e_source_id)                             \
{                                                                            \
	(e_source_id),                                                           \
{                                                                        \
	BRDC_Trigger_UNKNOWN,                                                \
	BRDC_Trigger_UNKNOWN,                                                \
	BRDC_Trigger_UNKNOWN                                                 \
}                                                                        \
}

/****************************************************************************/
/* Inline helpers                                                           */
/****************************************************************************/
#define BVDC_P_SRC_RETURN_IF_ERR(result) \
	if ( BERR_SUCCESS != (result)) \
{\
	return BERR_TRACE(result);  \
}

/* Frame rate detection base on clk per vsync */
typedef struct
{
	BAVC_FrameRateCode             eFramerate;
	uint32_t                       ulClkPerVsync;
	const char                    *pchName;

} BVDC_P_FrameRateEntry;

typedef struct
{
	BAVC_SourceId             eSourceId;
	BRDC_Trigger              aeTrigger[BVDC_P_SRC_MAX_SLOT_COUNT];
} BVDC_P_SourceParams;


/* Static frame rate lookup table. */
static const BVDC_P_FrameRateEntry s_aIntFramerateTbl[] =
{
	BVDC_P_MAKE_FRT(e29_97  , 60, 1001),
	BVDC_P_MAKE_FRT(e30     , 60, 1000),
	BVDC_P_MAKE_FRT(e25     , 50, 1000),
	BVDC_P_MAKE_FRT(e23_976 , 48, 1001),
	BVDC_P_MAKE_FRT(e24     , 48, 1000)
};

static const BVDC_P_FrameRateEntry s_aProFramerateTbl[] =
{
	BVDC_P_MAKE_FRT(e59_94  , 60, 1001),
	BVDC_P_MAKE_FRT(e60     , 60, 1000),
	BVDC_P_MAKE_FRT(e29_97  , 30, 1001),
	BVDC_P_MAKE_FRT(e30     , 30, 1000),
	BVDC_P_MAKE_FRT(e25     , 25, 1000),
	BVDC_P_MAKE_FRT(e50     , 50, 1000),
	BVDC_P_MAKE_FRT(e23_976 , 24, 1001),
	BVDC_P_MAKE_FRT(e24     , 24, 1000)
};

/* INDEX: By source id, do not put ifdefs and nested ifdefs around these that
* become impossible to decipher.  The eSourceId != BVDC_P_NULL_SOURCE will
* indicate that this source's trigger (lost) is to be monitor or poll by
* display interrupt. */
static const BVDC_P_SourceParams s_aSrcParams[] =
{
	/* Mpeg feeder 0/1/2/3/4/5 */
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	/* Analog Vdec 0/1 (swapped trigger) */
	BVDC_P_MAKE_SRC_PARAMS(eVdec0, eVdec0Trig1, eVdec0Trig0),
	BVDC_P_MAKE_SRC_PARAMS(eVdec1, eVdec1Trig1, eVdec1Trig0),
	/* Digital ITU-R-656 0/1 (swapped trigger)*/
	BVDC_P_MAKE_SRC_PARAMS(e656In0, e6560Trig1, e6560Trig0),
	BVDC_P_MAKE_SRC_PARAMS(e656In1, e6561Trig1, e6561Trig0),
	/* Gfx feeder 0/1/2/3/4/5/6 */
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	/* HD_DVI input 0/1 */
	BVDC_P_MAKE_SRC_PARAMS(eHdDvi0, eHdDvi0Trig0, eHdDvi0Trig1),
	BVDC_P_MAKE_SRC_PARAMS(eHdDvi1, eHdDvi1Trig0, eHdDvi1Trig1),
	/* Ds0 */
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	/* VFD feeder 0/1/2/3/4/5/6/7 */
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE),
	BVDC_P_MAKE_SRC_PARAMS_NULL(BVDC_P_NULL_SOURCE)
};

static const char s_chExtVideoFmtName[] = "BFMT_VideoFmt: Ext";

/***************************************************************************
* {private}
*
*/
static void BVDC_P_SourceTrigger_isr
( void                            *pvhSource,
 int                              iParam1 )
{
	uint32_t i;
	BVDC_Source_Handle hSource = (BVDC_Source_Handle)pvhSource;
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	BSTD_UNUSED(iParam1);
	/* Test to see if we have reliable trigger source.  Once it's reliable
	* slot done RUL will disable this callback, and vec will enable when
	* it detects source trigger is lost. */
	if((BVDC_P_TriggerCtrl_eDisplay == hSource->eTrigCtrl) &&
		(hSource->ulSrcTrigger))
	{
		hSource->ulSrcTrigger--;
		if(!hSource->ulSrcTrigger)
		{
			hSource->eTrigCtrl    = BVDC_P_TriggerCtrl_eXfering;
			hSource->ulSrcTrigger = BVDC_P_TRIGGER_LOST_THRESHOLD;
			BDBG_WRN((" (S) Source[%d] enters Xfer state", hSource->eId));
		}
	}
	else if(BVDC_P_TriggerCtrl_eSource == hSource->eTrigCtrl)
	{
		/* Disable trigger interrupt (this func). */
		BINT_DisableCallback_isr(hSource->hTrigger0Cb);
		BINT_DisableCallback_isr(hSource->hTrigger1Cb);
		BINT_ClearCallback_isr(hSource->hTrigger0Cb);
		BINT_ClearCallback_isr(hSource->hTrigger1Cb);

		/* Setup slot to receive triggers again. */
		for(i = 0; i < hSource->ulSlotUsed; i++)
		{
			BRDC_Slot_ExecuteOnTrigger_isr(hSource->ahSlot[i],
				hSource->aeTrigger[i], true);
		}
		BDBG_WRN((" (S) Source[%d] re-acquires control of its slots", hSource->eId));
	}

	return;
}


/***************************************************************************
* {private}
*
* (1) combine connected windows' vbi pass thru settings
* (2) combine newly connected windows' vbi pass thru settings
* (3) combine newly dis-connected windows' vbi pass thru settings
*/
static void BVDC_P_Source_RefreshWindow_isr
( BVDC_Source_Handle               hSource,
 bool                             bUpdateApplyChanges )
{
	uint32_t i;
	bool bMosaicMode;
	bool bDeinterlace;
	bool bVdecPhaseAdj;
	bool bTestFeature;
	BVDC_Window_Handle hWindow;
	BVDC_P_Source_Info *pNewInfo;
	BVDC_P_Source_Info *pCurInfo;
	BVDC_P_Source_DirtyBits *pNewDirty;

	BDBG_ENTER(BVDC_P_Source_RefreshWindow_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	pCurInfo  = &hSource->stCurInfo;
	pNewInfo  = &hSource->stNewInfo;
	pNewDirty = &pNewInfo->stDirty;

	/* Initialize as if no vbi pass-thru, etc. */
	bMosaicMode   = false;
	bDeinterlace  = false;
	bVdecPhaseAdj = false;
	bTestFeature  = false;

	/* Search all window connected to this source, and see if it has vbi
	* pass-thru, mad, phase, etc. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if((!hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_DESTROY(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_SHUTDOWNRUL(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_SHUTDOWNPENDING(hSource->ahWindow[i]))
		{
			continue;
		}
		hWindow = hSource->ahWindow[i];
		BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

		/* If this source has a window with deinterlacer */
		if(hWindow->stNewInfo.bMosaicMode)
		{
			bMosaicMode = true;
		}

		/* If this source has a window with deinterlacer */
		if(hWindow->stNewInfo.bDeinterlace)
		{
			bDeinterlace = true;
		}

		/* Changes in the phase adjust mode. */
		if(BVDC_PhaseAdjustMode_eDisable != hWindow->stNewInfo.ePhaseAdjustMode)
		{
			bVdecPhaseAdj = true;
		}

	}

	/* source VBI changes will be reflected into the next RUL */
	if((pCurInfo->bDeinterlace  != bDeinterlace ) ||
		(pCurInfo->bMosaicMode   != bMosaicMode  ) ||
		(pCurInfo->bVdecPhaseAdj != bVdecPhaseAdj))
	{
		/* set new info and dirty bits. */
		pNewInfo->bMosaicMode   = bMosaicMode;
		pNewInfo->bDeinterlace  = bDeinterlace;
		pNewInfo->bVdecPhaseAdj = bVdecPhaseAdj;
		pNewDirty->stBits.bWinChanges  = BVDC_P_DIRTY; /* bWinChanges */

		/* On a connect/disconnect source is already gone thru applychanges
		* that's mean that it didn't detected these change.  We're now
		* mark the apply changes, so it can be pickup in next _isr */
		if(bUpdateApplyChanges)
		{
			hSource->bUserAppliedChanges = true;
		}
	}

	BDBG_LEAVE(BVDC_P_Source_RefreshWindow_isr);
	return;
}


/***************************************************************************
*
*/
static void BVDC_P_Source_Bringup_isr
( BVDC_Source_Handle               hSource )
{
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		BVDC_P_Vdec_Bringup_isr(hSource->hVdec);
	}
	else if(BVDC_P_SRC_IS_ITU656(hSource->eId))
	{
#if (BVDC_P_SUPPORT_NEW_656_IN_VER)
		BVDC_P_656In_Bringup_isr(hSource->h656In);
#else
		BVDC_P_Vdec_Bringup_isr(hSource->hVdec);
#endif
	}
	else if(BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		BVDC_P_HdDvi_Bringup_isr(hSource->hHdDvi);
	}
	else if(BVDC_P_SRC_IS_DS(hSource->eId))
	{
		BVDC_P_DownSample_Bringup_isr(hSource->hDownSample);
	}

	return;
}

/***************************************************************************
* {private}
*
*/
BERR_Code BVDC_P_Source_Create
( BVDC_Handle                      hVdc,
  BVDC_Source_Handle              *phSource,
  BAVC_SourceId                    eSourceId,
  BVDC_P_Resource_Handle           hResource,
  bool                             b3dSrc )
{
	uint32_t i;
	BRDC_SlotId eSlotId;
	BVDC_P_SourceContext *pSource;
	BERR_Code eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Source_Create);
	BDBG_ASSERT(phSource);
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	/* BDBG_SetModuleLevel("BVDC_SRC", BDBG_eMsg); */

	/* (1) Alloc the window context. */
	pSource = (BVDC_P_SourceContext*)
		(BKNI_Malloc(sizeof(BVDC_P_SourceContext)));
	if(!pSource)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pSource, 0x0, sizeof(BVDC_P_SourceContext));
	BDBG_OBJECT_SET(pSource, BVDC_SRC);

	/* Initialize non-changing states.  These are not to be changed by runtime. */
	pSource->eId           = eSourceId;
	pSource->hVdc          = hVdc;

	/* Note non-mpeg source only uses 2 slots */
	pSource->ulSlotUsed    = BVDC_P_SRC_IS_MPEG(pSource->eId)
		? (BVDC_P_SRC_MAX_SLOT_COUNT) : (BVDC_P_SRC_MAX_SLOT_COUNT - 1);

	/* Triggers for each source . */
	for(i = 0; i < pSource->ulSlotUsed; i++)
	{
		pSource->aeTrigger[i] = s_aSrcParams[eSourceId].aeTrigger[i];
	}

	/* (2) Create RDC List & Slot */
	if(BVDC_P_SRC_IS_VIDEO(pSource->eId) && !BVDC_P_SRC_IS_VFD(pSource->eId))
	{
		for(i = 0; i < (pSource->ulSlotUsed * BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT); i++)
		{
			BRDC_List_Create(hVdc->hRdc, BVDC_P_MAX_ENTRY_PER_RUL, &pSource->ahList[i]);
		}

		for(i = 0; i < pSource->ulSlotUsed; i++)
		{
			BRDC_Slot_Create(hVdc->hRdc, &pSource->ahSlot[i]);
			/* Slot's force trigger reg addr. */
			BRDC_Slot_GetId(pSource->ahSlot[i], &eSlotId);
			pSource->aulImmTriggerAddr[i] = BCHP_RDC_desc_0_immediate +
				(eSlotId * (BCHP_RDC_desc_1_immediate - BCHP_RDC_desc_0_immediate));
			BDBG_MSG(("Source[%d] uses slot[%d]", pSource->eId, eSlotId));
		}

		if(!pSource->hVdc->stSettings.bDisableMosaic)
		{
			/* MosaicMode: create slave slot and list for mpeg source; */
			if(BVDC_P_SRC_IS_MPEG(pSource->eId))
			{
				/* Only need top/bottom slave slots and lists */
				for(i = 0; i < 2 * BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT; i++)
				{
					BRDC_List_Create(hVdc->hRdc, BVDC_P_MAX_ENTRY_PER_MPEG_RUL,
						&pSource->ahListSlave[i]);
				}

				BRDC_Slot_Create(hVdc->hRdc, &pSource->hSlotSlave);
				BRDC_Slot_GetId(pSource->hSlotSlave, &eSlotId);
				BDBG_MSG(("Source[%d] uses slave slot[%d]", pSource->eId, eSlotId));

				/* frame src shares the same combined trigger as top field */
				/* mpeg0: combo0; mpeg1: combo1; mpeg2: combo2 */
				pSource->eCombTrigger = BRDC_Trigger_eComboTrig0 + pSource->eId;
			}
		}
	}

	/* (3) For analog/656/HdDvi sources VDC will handle the callback. */
	if(BVDC_P_SRC_IS_ANALOG(pSource->eId) ||
	   BVDC_P_SRC_IS_ITU656(pSource->eId) ||
	   BVDC_P_SRC_IS_HDDVI(pSource->eId) ||
	   BVDC_P_SRC_IS_DS(pSource->eId) ||
	   BVDC_P_SRC_IS_MPEG(pSource->eId))
	{
		/* RUL done execution callbacks. */
		for(i = 0; i < pSource->ulSlotUsed; i++)
		{
			BERR_TRACE(BINT_CreateCallback(&pSource->ahCallback[i], hVdc->hInterrupt,
				BRDC_Slot_GetIntId(pSource->ahSlot[i]),
				BVDC_P_SRC_IS_HDDVI(pSource->eId) ? BVDC_P_Source_HdDviDataReady_isr
				: BVDC_P_SRC_IS_DS(pSource->eId) ? BVDC_P_Source_DownSampleDataReady_isr
				: BVDC_P_SRC_IS_MPEG(pSource->eId) ? BVDC_P_Source_MfdGfxCallback_isr
				: BVDC_P_Source_AnalogDataReady_isr,
				(void*)pSource, i));
		}
	}

	/* (4) Triggers detection callback. */
	if(BVDC_P_NULL_SOURCE != s_aSrcParams[eSourceId].eSourceId)
	{
		const BRDC_TrigInfo *pTrig0Info, *pTrig1Info;

		/* Trigger information. */
		pTrig0Info = BRDC_Trigger_GetInfo(hVdc->hRdc, s_aSrcParams[eSourceId].aeTrigger[0]);
		pTrig1Info = BRDC_Trigger_GetInfo(hVdc->hRdc, s_aSrcParams[eSourceId].aeTrigger[1]);

		BDBG_ASSERT(pTrig0Info && pTrig1Info);
		BDBG_MSG(("\tTrigger lost detection for source[%d]: %s, %s",
			pSource->eId, pTrig0Info->pchTrigName, pTrig0Info->pchTrigName));

		/* Make need to update table due to source enum update. */
		BDBG_ASSERT(s_aSrcParams[eSourceId].eSourceId == eSourceId);
		eStatus = BINT_CreateCallback(&pSource->hTrigger0Cb, hVdc->hInterrupt,
			pTrig0Info->TrigIntId, BVDC_P_SourceTrigger_isr, (void*)pSource,
			pTrig0Info->TrigIntId);
		if(BERR_SUCCESS != eStatus)
		{
			return BERR_TRACE(eStatus);
		}
		eStatus = BINT_CreateCallback(&pSource->hTrigger1Cb, hVdc->hInterrupt,
			pTrig1Info->TrigIntId, BVDC_P_SourceTrigger_isr, (void*)pSource,
			pTrig1Info->TrigIntId);
		if(BERR_SUCCESS != eStatus)
		{
			return BERR_TRACE(eStatus);
		}
	}

	/* (5) Created a specific source handle. */
	switch(eSourceId)
	{
	case BAVC_SourceId_eMpeg0:
	case BAVC_SourceId_eMpeg1:
	case BAVC_SourceId_eMpeg2:
	case BAVC_SourceId_eMpeg3:
	case BAVC_SourceId_eMpeg4:
	case BAVC_SourceId_eMpeg5:
		BVDC_P_Feeder_Create(&pSource->hMpegFeeder, hVdc->hRdc, hVdc->hRegister,
			BVDC_P_FeederId_eMfd0 + eSourceId - BAVC_SourceId_eMpeg0, hVdc->hTimer,
			pSource, hResource, b3dSrc);
		break;

	case BAVC_SourceId_eGfx0:
	case BAVC_SourceId_eGfx1:
	case BAVC_SourceId_eGfx2:
	case BAVC_SourceId_eGfx3:
	case BAVC_SourceId_eGfx4:
	case BAVC_SourceId_eGfx5:
	case BAVC_SourceId_eGfx6:
		BVDC_P_GfxFeeder_Create(
			&pSource->hGfxFeeder, hVdc->hRegister, hVdc->hRdc, eSourceId, b3dSrc);
		break;

	case BAVC_SourceId_eVdec0:
	case BAVC_SourceId_eVdec1:
		BVDC_P_Vdec_Create(&pSource->hVdec,
			BVDC_P_VdecId_eVdec0 + eSourceId - BAVC_SourceId_eVdec0, hVdc->hRegister, pSource);
		break;

#if (BVDC_P_SUPPORT_NEW_656_IN_VER)

	case BAVC_SourceId_e656In0:
	case BAVC_SourceId_e656In1:
		BVDC_P_656In_Create(&pSource->h656In,
			BVDC_P_656Id_e656In0 + eSourceId - BAVC_SourceId_e656In0, pSource);
		break;
#else
	case BAVC_SourceId_e656In0:
	case BAVC_SourceId_e656In1:
		BVDC_P_Vdec_Create(&pSource->hVdec,
			BVDC_P_VdecId_eVdec0 + eSourceId - BAVC_SourceId_e656In0, hVdc->hRegister, pSource);
		break;
#endif

	case BAVC_SourceId_eHdDvi0:
	case BAVC_SourceId_eHdDvi1:
		BVDC_P_HdDvi_Create(&pSource->hHdDvi,
			BVDC_P_HdDviId_eHdDvi0 + eSourceId - BAVC_SourceId_eHdDvi0, hVdc->hRegister, pSource);
		break;

	case BAVC_SourceId_eDs0:
		BVDC_P_DownSample_Create(&pSource->hDownSample, BVDC_P_DownSampleId_eDs0, hVdc->hRegister,
			pSource);
		break;

	case BAVC_SourceId_eVfd0:
	case BAVC_SourceId_eVfd1:
	case BAVC_SourceId_eVfd2:
	case BAVC_SourceId_eVfd3:
	case BAVC_SourceId_eVfd4:
	case BAVC_SourceId_eVfd5:
	case BAVC_SourceId_eVfd6:
	case BAVC_SourceId_eVfd7:
		BVDC_P_Feeder_Create(&pSource->hVfdFeeder, hVdc->hRdc, hVdc->hRegister,
			BVDC_P_FeederId_eVfd0 + eSourceId - BAVC_SourceId_eVfd0,
			hVdc->hTimer, pSource, hVdc->hResource, b3dSrc);
		break;

	default:
		BDBG_ERR(("Unknown source to create."));
		BDBG_ASSERT(false);
	}

	/* (6) create a AppliedDone event. */
	BKNI_CreateEvent(&pSource->hAppliedDoneEvent);

	/* (7) Added this compositor to hVdc */
	hVdc->ahSource[eSourceId] = (BVDC_Source_Handle)pSource;

	/* All done. now return the new fresh context to user. */
	*phSource = (BVDC_Source_Handle)pSource;

	BDBG_LEAVE(BVDC_P_Source_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
* {private}
*
*/
void BVDC_P_Source_Destroy
( BVDC_Source_Handle               hSource )
{
	uint32_t i;

	BDBG_ENTER(BVDC_P_Source_Destroy);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* At this point application should have disable all the
	* callbacks &slots */

	/* [7] Removed this source from hVdc */
	hSource->hVdc->ahSource[hSource->eId] = NULL;

	/* [6] Destroy event */
	BKNI_DestroyEvent(hSource->hAppliedDoneEvent);

	/* [5] Destroy a specific source handle. */
#if (BVDC_P_SUPPORT_NEW_656_IN_VER)
	if(hSource->h656In)
	{
		BVDC_P_656In_Destroy(hSource->h656In);
	}
#endif
	if(hSource->hVdec)
	{
		BVDC_P_Vdec_Destroy(hSource->hVdec);
	}
	if(hSource->hHdDvi)
	{
		BVDC_P_HdDvi_Destroy(hSource->hHdDvi);
	}
	if(hSource->hMpegFeeder)
	{
		BVDC_P_Feeder_Destroy(hSource->hMpegFeeder);
	}
	if(hSource->hGfxFeeder)
	{
		BVDC_P_GfxFeeder_Destroy(hSource->hGfxFeeder);
	}

	if(hSource->hDownSample)
	{
		BVDC_P_DownSample_Destroy(hSource->hDownSample);
	}

	if(hSource->hVfdFeeder)
	{
		BVDC_P_Feeder_Destroy(hSource->hVfdFeeder);
	}

	/* [4] Trigger detection callback. */
	if(hSource->hTrigger0Cb)
	{
		BINT_DestroyCallback(hSource->hTrigger0Cb);
	}
	if(hSource->hTrigger1Cb)
	{
		BINT_DestroyCallback(hSource->hTrigger1Cb);
	}

	/* [3] Its callbacks. */
	if(BVDC_P_SRC_IS_ANALOG(hSource->eId) ||
	   BVDC_P_SRC_IS_ITU656(hSource->eId) ||
	   BVDC_P_SRC_IS_HDDVI(hSource->eId) ||
	   BVDC_P_SRC_IS_DS(hSource->eId) ||
	   BVDC_P_SRC_IS_MPEG(hSource->eId))
	{
		for(i = 0; i < hSource->ulSlotUsed; i++)
		{
			BINT_DestroyCallback(hSource->ahCallback[i]);
		}
	}

	/* [2] Destroy RDC List & Slot */
	if(BVDC_P_SRC_IS_VIDEO(hSource->eId) && !BVDC_P_SRC_IS_VFD(hSource->eId))
	{
		if(!hSource->hVdc->stSettings.bDisableMosaic)
		{
			/* MosaicMode: create slave slot and list for mpeg source; */
			if(BVDC_P_SRC_IS_MPEG(hSource->eId))
			{
				BRDC_Slot_Destroy(hSource->hSlotSlave);
				for(i = 0; i < 2 * BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT; i++)
				{
					BRDC_List_Destroy(hSource->ahListSlave[i]);
				}
			}
		}

		for(i = 0; i < hSource->ulSlotUsed; i++)
		{
			BRDC_Slot_Destroy(hSource->ahSlot[i]);
		}

		for(i = 0; i < (hSource->ulSlotUsed * BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT); i++)
		{
			BRDC_List_Destroy(hSource->ahList[i]);
		}
	}

	BDBG_OBJECT_DESTROY(hSource, BVDC_SRC);
	/* [1] Free the context. */
	BKNI_Free((void*)hSource);

	BDBG_LEAVE(BVDC_P_Source_Destroy);
	return;
}


/***************************************************************************
* {private}
*
*/
void BVDC_P_Source_Init
( BVDC_Source_Handle               hSource,
  const BVDC_Source_Settings      *pDefSettings )
{
	uint32_t i;
	const BFMT_VideoInfo *pDefFmt;
	BVDC_P_Source_Info *pNewInfo;
	BVDC_P_Source_Info *pCurInfo;
	BVDC_P_Source_IsrInfo *pIsrInfo;
	BVDC_P_Source_DirtyBits *pNewDirty;
	bool bGfxSrc;

	BDBG_ENTER(BVDC_P_Source_Init);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* Which heap to use? */
	hSource->hHeap = ((pDefSettings) && (pDefSettings->hHeap))
		? pDefSettings->hHeap : hSource->hVdc->hBufferHeap;

	bGfxSrc = (pDefSettings)? pDefSettings->bGfxSrc : false;

	/* New/Cur/Isr Info */
	pNewInfo = &hSource->stNewInfo;
	pCurInfo = &hSource->stCurInfo;
	pIsrInfo = &hSource->stIsrInfo;

	/* Zero out the previous field. */
	BKNI_Memset((void*)&(hSource->stPrevMvdField), 0x0, sizeof(BAVC_MVD_Field));
	BKNI_Memset((void*)&(hSource->stNewXvdField), 0x0, sizeof(BAVC_VDC_HdDvi_Picture));
	BKNI_Memset((void*)&(hSource->stScanOut), 0x0, sizeof(BVDC_P_Rect));
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#if BVDC_P_SUPPORT_MOSAIC_MODE
	for(i = 0; i < BAVC_MOSAIC_MAX; i++)
		BKNI_Memset((void*)&(hSource->stNewPic[i]), 0x0, sizeof(BAVC_MVD_Field));
#else
	BKNI_Memset((void*)&(hSource->stNewPic), 0x0, sizeof(BAVC_MVD_Field));
#endif
#endif

	pNewDirty = &pNewInfo->stDirty;
	BDBG_ASSERT(sizeof(pNewDirty->stBits) <= sizeof(pNewDirty->aulInts));

	/* Default format */
	pDefFmt = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_eNTSC);

	/* Initialize states can be changed by runtime. */
	hSource->bInitial             = true;
	hSource->bCaptureCrc          = false;
	hSource->bPictureChanged      = true;
	hSource->bRasterChanged       = true;
	hSource->bPrevFieldSwap       = false;
	hSource->bDeferSrcPendingCb   = false;
	hSource->ulTransferLock       = 0;
	hSource->ulConnectedWindow    = 0;
	hSource->hCmpToBeLocked       = NULL;
	hSource->hSyncLockCompositor  = NULL;
	hSource->ulSrcTrigger         = BVDC_P_TRIGGER_LOST_THRESHOLD;
	hSource->ulVecTrigger         = BVDC_P_TRIGGER_LOST_THRESHOLD;
	hSource->eNextFieldId         = BAVC_Polarity_eTopField;
	hSource->eNextFieldIntP       = BAVC_Polarity_eTopField;
	hSource->eState               = BVDC_P_State_eInactive;
	hSource->eTrigCtrl            = BVDC_P_TriggerCtrl_eSource;
	hSource->eMatrixCoefficients  = BAVC_MatrixCoefficients_eItu_R_BT_709;
	hSource->bNewUserModeAspRatio = false;
	hSource->ulVertFreq           = pDefFmt->ulVertFreq;
	hSource->ulStreamVertFreq     = pDefFmt->ulVertFreq;
	hSource->bStartFeed           = true; /* always true for MPEG, reset every vsync for other src */
	hSource->ulSampleFactor       = 0;    /* (1+0)x time */
	hSource->bPsfScanout          = false;/* no PsF scanout */
	hSource->ulDropFramesCnt      = 0;    /* no PsF drop */
	hSource->ulDispVsyncFreq      = pDefFmt->ulVertFreq;
	hSource->ePcFieldId           = BAVC_Polarity_eTopField;
	hSource->bSrcIs444            = false;
	hSource->bSrcInterlaced       = true;
	hSource->bTestFeature1        = false;
	hSource->bWait4ReconfigVnet   = false;
	hSource->stExtVideoFmtInfo.eVideoFmt = BFMT_VideoFmt_eNTSC;
	hSource->stExtVideoFmtInfo.pchFormatStr = &s_chExtVideoFmtName[0];
	hSource->ulMosaicFirstUnmuteRectIndex = 0;
	hSource->bMosaicFirstUnmuteRectIndexSet = true;
#if BVDC_P_STG_NRT_CADENCE_WORKAROUND
	hSource->bPrevIgnorePicture   = false;
#endif

	/* Reset done events */
	BKNI_ResetEvent(hSource->hAppliedDoneEvent);

	/* Clear out user's states. */
	BKNI_Memset((void*)pNewInfo, 0x0, sizeof(BVDC_P_Source_Info));
	BKNI_Memset((void*)pCurInfo, 0x0, sizeof(BVDC_P_Source_Info));
	BKNI_Memset((void*)pIsrInfo, 0x0, sizeof(BVDC_P_Source_IsrInfo));

	/* Initialize all common fields */
	pNewInfo->pFmtInfo          = pDefFmt;
	pNewInfo->pVdcFmt           = BVDC_P_GetFormatInfoPtr_isr(pNewInfo->pFmtInfo);
	pNewInfo->eAspectRatio      = pNewInfo->pFmtInfo->eAspectRatio;
	pNewInfo->ulMuteColorYCrCb  = BVDC_P_YCRCB_BLACK;
	pNewInfo->eMuteMode         = BVDC_MuteMode_eDisable;
	pNewInfo->pfPicCallbackFunc = NULL;
	pNewInfo->bEnable3DComb     = false;
	pNewInfo->bAutoFmtDetection = false;
	pNewInfo->bAutoDetect       = false;
	pNewInfo->ulNumFormats      = 0;
	pNewInfo->bErrorLastSetting = false;
	pNewInfo->ulInputPort       = BVDC_HdDviInput_Hdr;

	/* hddvi */
	pNewInfo->stHdDviSetting.stFmtTolerence.ulWidth  = BVDC_P_HDDVI_FORMAT_TOLER_WIDTH;
	pNewInfo->stHdDviSetting.stFmtTolerence.ulHeight = BVDC_P_HDDVI_FORMAT_TOLER_HEIGHT;

	/* dnr */
	pNewInfo->bDnr                     = false;
	pNewInfo->stDnrSettings.eBnrMode   = BVDC_FilterMode_eDisable;
	pNewInfo->stDnrSettings.eMnrMode   = BVDC_FilterMode_eDisable;
	pNewInfo->stDnrSettings.eDcrMode   = BVDC_FilterMode_eDisable;
	pNewInfo->stDnrSettings.iBnrLevel  = 0;
	pNewInfo->stDnrSettings.iMnrLevel  = 0;
	pNewInfo->stDnrSettings.iDcrLevel  = 0;
	pNewInfo->stDnrSettings.ulQp       = 0;
	pNewInfo->stDnrSettings.pvUserInfo = NULL;

	/* Demo mode */
	pNewInfo->stSplitScreenSetting.eDnr = BVDC_SplitScreenMode_eDisable;

	/* PC remap */
	pNewInfo->bReMapFormats  = false;
	pNewInfo->ulReMapFormats = 0;

	/* Intialize format list */
	for(i = 0; i < BFMT_VideoFmt_eMaxCount; i++)
	{
		pNewInfo->aeFormats[i] = BFMT_VideoFmt_eMaxCount;
		pCurInfo->aeFormats[i] = BFMT_VideoFmt_eMaxCount;
	}

	/* When there no interrupt to update we need to at least has
	* the fixed color set to a valid color. */
	pCurInfo->ulMuteColorYCrCb = BVDC_P_YCRCB_BLACK;
	pCurInfo->pFmtInfo         = BFMT_GetVideoFormatInfoPtr(BFMT_VideoFmt_eNTSC);
	pCurInfo->pVdcFmt          = BVDC_P_GetFormatInfoPtr_isr(pCurInfo->pFmtInfo);
	pCurInfo->eAspectRatio     = pCurInfo->pFmtInfo->eAspectRatio;
	pCurInfo->ulInputPort      = BVDC_HdDviInput_Hdr;

	/* Initialize user CSC settings */
	pNewInfo->bUserCsc = false;
	pNewInfo->ulUserShift = 16;
	for(i = 0; i < BVDC_CSC_COEFF_COUNT; i++)
	{
		pNewInfo->pl32_Matrix[i] = 0;
	}

	/* Initial fix color */
	pNewInfo->bFixColorEnable = false;
	for(i = 0; i < 3; i++)
	{
		pNewInfo->aulFixColorYCrCb[i] = BVDC_P_YCRCB_BLACK;
	}

	/* Initialized which ADC get use by which vdec. */
	for(i = 0; i < BVDC_P_MAX_ADC; i++)
	{
		pNewInfo->aeAdcInput[i] = BVDC_AdcInput_eUnused;
	}

	/* Initialized window handle*/
	for(i=0; i<BVDC_P_MAX_WINDOW_COUNT;i++)
		hSource->ahWindow[i] = 0;

	if(BVDC_P_SRC_IS_VIDEO(hSource->eId) && !BVDC_P_SRC_IS_VFD(hSource->eId))
	{
		/* reinitialized ruls. */
		for(i = 0; i < (hSource->ulSlotUsed * BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT); i++)
		{
			BRDC_List_SetNumEntries_isr(hSource->ahList[i], 0);
			BVDC_P_BuildNoOpsRul_isr(hSource->ahList[i]);
		}

		/* Assign fresh new no-op list. */
		for(i = 0; i < hSource->ulSlotUsed; i++)
		{
			/* Initialized rul indexing. */
			hSource->aulRulIdx[i] = 0;
			BRDC_Slot_SetList_isr(hSource->ahSlot[i],
				hSource->ahList[i * BVDC_P_MAX_MULTI_RUL_BUFFER_COUNT]);
		}
	}

	/* Initialize specific source. */
#if (BVDC_P_SUPPORT_NEW_656_IN_VER)
	if(hSource->h656In)
	{
		BVDC_P_656In_Init(hSource->h656In);
		pNewInfo->eCtInputType = BVDC_P_CtInput_eItu656;
	}
#endif
	if(hSource->hVdec)
	{
		BVDC_P_Vdec_Init(hSource->hVdec);
		pNewInfo->eCtInputType = BVDC_P_CtInput_eCvbs;
	}
	if(hSource->hHdDvi)
	{
		BVDC_P_HdDvi_Init(hSource->hHdDvi);
		pNewInfo->eCtInputType = BVDC_P_CtInput_eHdDvi;
	}
	if(hSource->hMpegFeeder)
	{
		BVDC_P_Feeder_Init(hSource->hMpegFeeder, hSource->hHeap->hMem, bGfxSrc);
		pNewInfo->eCtInputType = BVDC_P_CtInput_eMpeg;
	}
	if(hSource->hVfdFeeder)
	{
		BVDC_P_Feeder_Init(hSource->hVfdFeeder, hSource->hHeap->hMem, bGfxSrc);
		pNewInfo->eCtInputType = BVDC_P_CtInput_eMpeg;
	}
	if(hSource->hGfxFeeder)
	{
		BVDC_P_GfxFeeder_Init(hSource->hGfxFeeder, hSource->hHeap->hMem);
		pNewInfo->eCtInputType = BVDC_P_CtInput_eUnknown;
	}
	if(hSource->hDownSample)
	{
		BVDC_P_DownSample_Init(hSource->hDownSample);
		pNewInfo->eCtInputType = BVDC_P_CtInput_eUnknown;
	}

	BDBG_LEAVE(BVDC_P_Source_Init);
	return;
}



/***************************************************************************
* {private}
*
*/
static BERR_Code BVDC_P_Source_Validate
( BVDC_Source_Handle         hSource )
{
	BVDC_P_Source_Info   *pNewInfo;
	BVDC_P_Source_DirtyBits *pNewDirty;
	const BFMT_VideoInfo *pSrcFmtInfo;
	BERR_Code  eStatus = BERR_SUCCESS;

	/* Get new source info and make sure everything is good. */
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	pNewInfo = &hSource->stNewInfo;
	pNewDirty = &pNewInfo->stDirty;

	/* If error happens, this will flagged, and will accept new user settings. */
	pNewInfo->bErrorLastSetting = true;

	if(BVDC_P_SRC_IS_GFX(hSource->eId))
	{
		/* validate Gfx feeder, surface and window combination,
		 * note: Gfx feeder can not be shared by more than one window */
		return BERR_TRACE(BVDC_P_GfxFeeder_ValidateChanges(
			hSource->hGfxFeeder, pNewInfo->pfPicCallbackFunc));
	}
	else if(BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		/* VDEC specific validation. */
		eStatus = BERR_TRACE(BVDC_P_Vdec_ValidateChanges(hSource->hVdec));
		if(BERR_SUCCESS != eStatus)
		{
			return eStatus;
		}
		pNewInfo->eCtInputType = pNewInfo->eInputType;
	}
	else if(BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		/* DVI specific validation. */
		BERR_Code eStatus;
		eStatus = BERR_TRACE(BVDC_P_HdDvi_ValidateChanges(hSource->hHdDvi));
		if(BERR_SUCCESS != eStatus)
		{
			return eStatus;
		}
	}
	else if(BVDC_P_SRC_IS_VFD(hSource->eId))
	{
		/* VFD Gfx Src specific validation. */
		eStatus = BERR_TRACE(BVDC_P_Feeder_ValidateChanges(
			hSource->hVfdFeeder, pNewInfo->pfPicCallbackFunc));
		if(BERR_SUCCESS != eStatus)
		{
			return eStatus;
		}
	}
	else if(BVDC_P_SRC_IS_MPEG(hSource->eId) && hSource->hMpegFeeder->bGfxSrc)
	{
		/* MPEG Gfx Src specific validation. */
		eStatus = BERR_TRACE(BVDC_P_Feeder_ValidateChanges(
			hSource->hMpegFeeder, pNewInfo->pfPicCallbackFunc));
		if(BERR_SUCCESS != eStatus)
		{
			return eStatus;
		}
	}
	/*
	else if(BVDC_P_SRC_IS_ITU656(hSource->eId))
	{
	ITU656 specific validation.
	}
	else if(BVDC_P_SRC_IS_DS(hSource->eId))
	{
	DS specific validation.
	}
	*/

	/* Refresh to see if any of the window toggle vbi/mad. */
	if(BVDC_P_SRC_IS_VIDEO(hSource->eId))
	{
		/* aspect ratio canvas clip validation */
		pSrcFmtInfo =
			((hSource->stNewInfo.bAutoFmtDetection) &&
			(hSource->stCurInfo.bAutoFmtDetection))
			? hSource->stCurInfo.pFmtInfo : hSource->stNewInfo.pFmtInfo;

		if((2 * (pNewInfo->stAspRatRectClip.ulLeft + pNewInfo->stAspRatRectClip.ulRight) > pSrcFmtInfo->ulWidth) ||
			(2 * (pNewInfo->stAspRatRectClip.ulTop + pNewInfo->stAspRatRectClip.ulBottom) > pSrcFmtInfo->ulHeight))
		{
			return BERR_TRACE(BVDC_ERR_INVALID_SRC_ASPECT_RATIO_RECT);
		}

		/* (1) Update connected windows that may have vbi-passthru */
		BKNI_EnterCriticalSection();
		BVDC_P_Source_RefreshWindow_isr(hSource, false);
		BKNI_LeaveCriticalSection();
	}

	/* If source pending is enabled, and user generated an artifictial
	* source change.  Put source into pending. */
	if(BVDC_ResumeMode_eManual == pNewInfo->eResumeMode)
	{
		pNewDirty->stBits.bInputFormat   |= pNewInfo->bForceSrcPending;
		pNewInfo->bForceSrcPending = false;
	}

	pNewInfo->bErrorLastSetting = false;
	return BERR_SUCCESS;
}


/***************************************************************************
* {private}
*
*/
BERR_Code BVDC_P_Source_ValidateChanges
( const BVDC_Source_Handle         ahSource[] )
{
	int i;
	BERR_Code eStatus;

	BDBG_ENTER(BVDC_P_Source_ValidateChanges);

	for(i = 0; i < BVDC_P_MAX_SOURCE_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(ahSource[i]) ||
			BVDC_P_STATE_IS_CREATE(ahSource[i]))
		{
			eStatus = BVDC_P_Source_Validate(ahSource[i]);
			if(BERR_SUCCESS != eStatus)
			{
				return BERR_TRACE(eStatus);
			}
		}
	}

	/* Second pass validation of ADC duplicate uses!  Don't need it for
	* non vdec chip. */
#if (BVDC_P_SUPPORT_VDEC)
	eStatus = BVDC_P_Vdec_ValidateAdc(ahSource);
	if(BERR_SUCCESS != eStatus)
	{
		return BERR_TRACE(eStatus);
	}
#endif

	BDBG_LEAVE(BVDC_P_Source_ValidateChanges);
	return BERR_SUCCESS;
}


/***************************************************************************
* {private}
*
*/
void BVDC_P_Source_ApplyChanges_isr
( BVDC_Source_Handle               hSource )
{
	BVDC_P_Source_Info *pNewInfo;
	const BVDC_P_Source_Info *pCurInfo;
	BVDC_P_Source_DirtyBits *pNewDirty;
	BVDC_P_Source_DirtyBits *pActIsrSet;
	uint32_t i;

	BDBG_ENTER(BVDC_P_Source_ApplyChanges_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	pNewInfo = &hSource->stNewInfo;
	pCurInfo = &hSource->stCurInfo;
	pNewDirty = &pNewInfo->stDirty;

	/* Did any of the dirty got set. */
	if(BVDC_P_IS_DIRTY(pNewDirty))
	{
		hSource->bUserAppliedChanges = true;
	}

	/* State transition for source. */
	if(BVDC_P_STATE_IS_CREATE(hSource))
	{
		BDBG_MSG(("Source[%d] activated.", hSource->eId));
		hSource->eState = BVDC_P_State_eActive;
		hSource->bUserAppliedChanges = true;
		BVDC_P_SET_ALL_DIRTY(pNewDirty);

		/* this dirty bit should set by user to resume the video */
		pNewDirty->stBits.bResume = BVDC_P_CLEAN;

		/* Re-enable source with callbacks. */
		if(BVDC_P_SRC_IS_ANALOG(hSource->eId) ||
		   BVDC_P_SRC_IS_ITU656(hSource->eId) ||
		   BVDC_P_SRC_IS_HDDVI (hSource->eId) ||
		   BVDC_P_SRC_IS_DS(hSource->eId) ||
		   (BVDC_P_SRC_IS_MPEG(hSource->eId) && hSource->hMpegFeeder->bGfxSrc))
		{
			for(i = 0; i < hSource->ulSlotUsed; i++)
			{
				BINT_ClearCallback_isr(hSource->ahCallback[i]);
				BINT_EnableCallback_isr(hSource->ahCallback[i]);
			}
		}
		if(BVDC_P_SRC_IS_MPEG(hSource->eId))
		{
			for(i = 0; i < hSource->ulSlotUsed; i++)
			{
				BRDC_Slot_ExecuteOnTrigger_isr(hSource->ahSlot[i],
					BRDC_Trigger_UNKNOWN, true);
			}
		}
		/* PR31679, PR46514: Shut off unused adc for power saving as well. */
		if(BVDC_P_SRC_IS_ANALOG(hSource->eId))
		{
			BVDC_P_Vdec_SetPowerMode_isr(hSource->hVdec, true);
		}
	}
	else if(BVDC_P_STATE_IS_DESTROY(hSource))
	{
		BDBG_MSG(("Source[%d] de-activated.", hSource->eId));
		hSource->eState = BVDC_P_State_eInactive;

		if(BVDC_P_SRC_IS_VIDEO(hSource->eId) && !BVDC_P_SRC_IS_VFD(hSource->eId))
		{
			for(i = 0; i < hSource->ulSlotUsed; i++)
			{
				BRDC_Slot_Disable_isr(hSource->ahSlot[i]);
			}
		}
		if(BVDC_P_SRC_IS_ANALOG(hSource->eId) ||
		   BVDC_P_SRC_IS_ITU656(hSource->eId) ||
		   BVDC_P_SRC_IS_HDDVI(hSource->eId)  ||
		   BVDC_P_SRC_IS_DS (hSource->eId) ||
		   (BVDC_P_SRC_IS_MPEG(hSource->eId) && hSource->hMpegFeeder->bGfxSrc))
		{
			for(i = 0; i < hSource->ulSlotUsed; i++)
			{
				BINT_DisableCallback_isr(hSource->ahCallback[i]);
				BINT_ClearCallback_isr(hSource->ahCallback[i]);
			}
		}
		/* PR31679, PR46514: Shut off unused adc for power saving as well. */
		if(BVDC_P_SRC_IS_ANALOG(hSource->eId))
		{
			BVDC_P_Vdec_SetPowerMode_isr(hSource->hVdec, false);
		}
	}

	if(BVDC_P_SRC_IS_GFX(hSource->eId))
	{
		if(hSource->bUserAppliedChanges)
		{
			BKNI_ResetEvent(hSource->hAppliedDoneEvent);
			BVDC_P_Source_UpdateSrcState_isr(hSource);
		}
		BVDC_P_GfxFeeder_ApplyChanges_isr(hSource->hGfxFeeder);
	}
	else
	{
		/* Copy activated _isr setting into newinfo to avoid overriding in
		 * UpdateSrcState_isr, iff the setting does not change after ApplyChanges */
		pActIsrSet = &hSource->stIsrInfo.stActivated;
		if(pActIsrSet->stBits.bInputFormat && !pNewDirty->stBits.bInputFormat)
		{
			pNewInfo->pFdEntry = pCurInfo->pFdEntry;
			pNewInfo->pFmtInfo = pCurInfo->pFmtInfo;
		}
		if(pActIsrSet->stBits.bAspectRatio && !pNewDirty->stBits.bAspectRatio)
		{
			pNewInfo->eAspectRatio = pCurInfo->eAspectRatio;
		}
		if(pActIsrSet->stBits.bAspectRatioClip && !pNewDirty->stBits.bAspectRatioClip)
		{
			pNewInfo->stAspRatRectClip = pCurInfo->stAspRatRectClip;
		}
		BVDC_P_CLEAN_ALL_DIRTY(pActIsrSet);

		/* Auto-update aspect-ratio according to source video format change if
		* user did not override at the same time */
		if(pNewDirty->stBits.bInputFormat && !hSource->bNewUserModeAspRatio)
		{
			pNewInfo->eAspectRatio = pNewInfo->pFmtInfo->eAspectRatio;
			pNewDirty->stBits.bAspectRatio = BVDC_P_DIRTY;
			/* Clean out-dated _isr setting which has never be used */
			hSource->stIsrInfo.stDirty.stBits.bAspectRatio = BVDC_P_CLEAN;
		}

		/* Allow auto-updating src aspect ratio in next ApplyChanges,
		* note: bNewUserModeAspRatio is only used here, it is set by
		* Source_OverrideAspectRatio */
		hSource->bNewUserModeAspRatio = false;

		/* Check raster change from user */
		if(pCurInfo->pFmtInfo->bInterlaced != pNewInfo->pFmtInfo->bInterlaced)
		{
			hSource->bRasterChanged = true;
		}

		/* Isr will set event to notify apply done. */
		if(hSource->bUserAppliedChanges)
		{
			BKNI_ResetEvent(hSource->hAppliedDoneEvent);
		}

		if (BVDC_P_SRC_IS_VFD(hSource->eId))
		{
			BVDC_P_Feeder_ApplyChanges_isr(hSource->hVfdFeeder);
		}
		else if (BVDC_P_SRC_IS_MPEG(hSource->eId) && hSource->hMpegFeeder->bGfxSrc)
		{
			BVDC_P_Feeder_ApplyChanges_isr(hSource->hMpegFeeder);
		}

		/* If no window is connected just update the source cur/new. */
		if(!hSource->ulConnectedWindow)
		{
			BVDC_P_Source_UpdateSrcState_isr(hSource);
		}
	}

	/* First time bring up the source, kick start the initial trigger. */
	if(hSource->bInitial)
	{
		BVDC_P_Source_UpdateSrcState_isr(hSource);
		BVDC_P_Source_Bringup_isr(hSource);
		hSource->bInitial = false;
	}

	BDBG_LEAVE(BVDC_P_Source_ApplyChanges_isr);
	return;
}


/***************************************************************************
* {private}
*
*/
void BVDC_P_Source_AbortChanges
( BVDC_Source_Handle               hSource )
{
	BDBG_ENTER(BVDC_P_Source_AbortChanges);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* Cancel the setting user set to the new state. */
	hSource->stNewInfo = hSource->stCurInfo;

	/* Gfx source has its own abort changes function. */
	if(BVDC_P_SRC_IS_GFX(hSource->eId))
	{
		BVDC_P_GfxFeeder_AbortChanges(hSource->hGfxFeeder);
	}

	BDBG_LEAVE(BVDC_P_Source_AbortChanges);
	return;
}


/***************************************************************************
* {private}
*
*/
void BVDC_P_Source_UpdateSrcState_isr
( BVDC_Source_Handle               hSource )
{
	BVDC_P_Source_DirtyBits *pNewDirty, *pCurDirty;

	BDBG_ENTER(BVDC_P_Source_UpdateSrcState_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* Get source applied informations */
	if(hSource->bUserAppliedChanges)
	{
		pCurDirty = &hSource->stCurInfo.stDirty;
		pNewDirty = &hSource->stNewInfo.stDirty;

		/* Copying the new info to the current info.  Must be careful here
		* of not globble current dirty bits set by source, but rather ORed
		* them together. */
		BVDC_P_OR_ALL_DIRTY(pNewDirty, pCurDirty);
		hSource->stCurInfo = hSource->stNewInfo;

		/* Clear dirty bits since it's already OR'ed into current.  Notes
		* the it might not apply until next vysnc, so we're defering
		* setting the event until next vsync. */
		BVDC_P_CLEAN_ALL_DIRTY(pNewDirty);
		hSource->bUserAppliedChanges = false;

		/* Notify applychanges that source has been updated. */
		BKNI_SetEvent_isr(hSource->hAppliedDoneEvent);
	}

	/* get isr set info */
	if(BVDC_P_IS_DIRTY(&hSource->stIsrInfo.stDirty))
	{
		BVDC_P_Source_DirtyBits *pIsrDirty, *pCurDirty;

		pCurDirty = &hSource->stCurInfo.stDirty;
		pIsrDirty = &hSource->stIsrInfo.stDirty;

		/* Copying the isr info to the current info */
		if(pIsrDirty->stBits.bAspectRatioClip)
		{
			BVDC_P_ClipRect *pClip = &hSource->stIsrInfo.stAspRatRectClip;
			const BFMT_VideoInfo *pSrcFmtInfo = hSource->stCurInfo.pFmtInfo;

			if((2 * (pClip->ulLeft + pClip->ulRight) <= pSrcFmtInfo->ulWidth) &&
				(2 * (pClip->ulTop + pClip->ulBottom) <= pSrcFmtInfo->ulHeight))
			{
				hSource->stCurInfo.stAspRatRectClip = hSource->stIsrInfo.stAspRatRectClip;
				pCurDirty->stBits.bAspectRatioClip = BVDC_P_DIRTY;
			}
		}

		/* note: if aspectRatio is set in user mode and activated by AppyChanges after
		* an _isr setting, we will not see isr dirty here */
		if(pIsrDirty->stBits.bAspectRatio)
		{
			hSource->stCurInfo.eAspectRatio = hSource->stIsrInfo.eAspectRatio;
			pCurDirty->stBits.bAspectRatio = BVDC_P_DIRTY;
		}

		/* inform next ApplyChanges to copy activated isr setting into new info */
		BVDC_P_OR_ALL_DIRTY(&hSource->stIsrInfo.stActivated, pIsrDirty);

		/* Clear dirty bits since it's already OR'ed into current */
		BVDC_P_CLEAN_ALL_DIRTY(pIsrDirty);
	}

	/* Inform windows to re-adjust rectangles */
	hSource->bPictureChanged = (
		hSource->bPictureChanged ||
		hSource->stCurInfo.stDirty.stBits.bAspectRatio ||
		hSource->stCurInfo.stDirty.stBits.bAspectRatioClip);

	/* Reset trigger by vec, telling that we receiving trigger. */
	hSource->ulVecTrigger = BVDC_P_TRIGGER_LOST_THRESHOLD;

	BDBG_LEAVE(BVDC_P_Source_UpdateSrcState_isr);
	return;
}


/***************************************************************************
*
*/
void BVDC_P_Source_UpdateStatus_isr
( BVDC_Source_Handle               hSource )
{
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		BVDC_P_Vdec_UpdateStatus_isr(hSource->hVdec);
	}
	else
	{
#if (BVDC_P_SUPPORT_NEW_656_IN_VER)
		BVDC_P_656In_UpdateStatus_isr(hSource->h656In);
#else
		BVDC_P_Vdec_UpdateStatus_isr(hSource->hVdec);
#endif
	}

	return;
}

/***************************************************************************
*
*/
void BVDC_P_Source_CleanupSlots_isr
( BVDC_Source_Handle               hSource )
{
	uint32_t i;
	BRDC_Slot_Handle hSlot;
	BRDC_List_Handle hList;

	BDBG_ENTER(BVDC_P_Source_CleanupSlots_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	for(i = 0; i < hSource->ulSlotUsed; i++)
	{
		BVDC_P_SRC_NEXT_RUL(hSource, i);
		hSlot = BVDC_P_SRC_GET_SLOT(hSource, i);
		hList = BVDC_P_SRC_GET_LIST(hSource, i);

		/* turn off slots exec tracking to not to lose the critical RUL
		in the source slots before the cleanup; */
		BRDC_Slot_UpdateLastRulStatus_isr(hSlot, hList, false);

		BRDC_List_SetNumEntries_isr(hList, 0);
		BVDC_P_BuildNoOpsRul_isr(hList);
		BRDC_Slot_SetList_isr(hSlot, hList);
	}
	BDBG_LEAVE(BVDC_P_Source_CleanupSlots_isr);
	return;
}


/***************************************************************************
* {private}
*/
void BVDC_P_Source_FindLockWindow_isr
( BVDC_Source_Handle               hSource,
 bool                             bUpdate )
{
	uint32_t i;
	BVDC_Window_Handle hTmpWindow;
	BVDC_Compositor_Handle hTmpCompositor;

	BDBG_ENTER(BVDC_P_Source_FindLockWindow_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_ASSERT(!hSource->hSyncLockCompositor);

	/* Trying to find a window/compositor that isn't lock */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if((!hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
		{
			continue;
		}

		hTmpWindow = hSource->ahWindow[i];
		BDBG_OBJECT_ASSERT(hTmpWindow, BVDC_WIN);
		hTmpCompositor = hTmpWindow->hCompositor;
		BDBG_OBJECT_ASSERT(hTmpCompositor, BVDC_CMP);
		/* if mpeg src transfers the trigger base, delay the triggering for clean transition;
		* Note, it makes sense for both sync-locked window and slipped PIP window; */
		if(BVDC_P_SRC_IS_MPEG(hSource->eId))
		{
			/* don't need to be active to acquire lock since new window might just be
			* destroyed and pending; */
			if(!hTmpCompositor->hSyncLockSrc)
			{
				if(bUpdate)
				{
					/* only lock on the to-be-locked */
					if(hSource->hCmpToBeLocked == hTmpCompositor)
					{
						BDBG_MSG(("Making other cmp's window[%d] synclock(3)", hTmpWindow->eId));
						hSource->hSyncLockCompositor   = hTmpWindow->hCompositor;
						hTmpWindow->bSyncLockSrc       = true;
						hTmpWindow->stCallbackData.stMask.bSyncLock = BVDC_P_DIRTY;
						hTmpWindow->hBuffer->bSyncLock = true;
						hTmpWindow->stCurInfo.stDirty.stBits.bReallocBuffers = true;
						hTmpCompositor->ulSlip2Lock    = 1;/* count */
						hTmpCompositor->hSyncLockWin   = hTmpWindow;
						hTmpCompositor->hSyncLockSrc   = hSource;
						hTmpCompositor->hSrcToBeLocked = NULL;
						hSource->hCmpToBeLocked        = NULL;
					}
					else
					{
						BDBG_MSG(("hTmpCompositor %d != the To-Be-Locked Cmp %d",
							hTmpCompositor->eId, hSource->hCmpToBeLocked->eId));
						/* continue the search */
						continue;
					}
				}
				else
				{
					hSource->ulTransferLock        = 3;
					hTmpCompositor->hSrcToBeLocked = hSource;
					hSource->hCmpToBeLocked        = hTmpWindow->hCompositor;
					BDBG_MSG(("SRC%d to transfer lock to CMP%d=%p", hSource->eId, hTmpCompositor->eId, hTmpCompositor));
				}
				/* find one to-be-locked window then break in case of triple displays */
				break;
			}
			else if(hTmpCompositor->hSyncLockSrc != hSource)
			{
				BDBG_MSG(("Transfer trigger source to cmp%d for PIP window[%d]",
					hTmpCompositor->eId, hTmpWindow->eId));
				hSource->ulTransferLock            = 3;
				break;
			}
		}
	}

	BDBG_ENTER(BVDC_P_Source_FindLockWindow_isr);
	return;
}


/***************************************************************************
* {private}
*
* For Mpeg source we need to determine if it's neccessary for the
* source slot isr to build the RUL (Synclock).
*/
void BVDC_P_Source_ConnectWindow_isr
( BVDC_Source_Handle               hSource,
 BVDC_Window_Handle               hWindow )
{
	BDBG_ENTER(BVDC_P_Source_ConnectWindow_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
	BDBG_ASSERT(!hSource->ahWindow[hWindow->eId]);

	hSource->ahWindow[hWindow->eId] = hWindow;

	/* Update the window count in compositor. */
	if(BVDC_P_WIN_IS_VIDEO_WINDOW(hWindow->eId))
	{
		/* (2) Update newly connected windows that may have vbi-passthru */
		hWindow->hCompositor->ulActiveVideoWindow++;
		if(hSource->ulConnectedWindow++ == 0)
		{
			hSource->bFieldSwap = false;
		}

		BVDC_P_Source_RefreshWindow_isr(hSource, true);
	}
	else
	{
		hWindow->hCompositor->ulActiveGfxWindow++;
	}

	/* If the window is mpeg and it's first connected to this source then
	* it will be a synclock window.  Provided that its compositor is not
	* already has a synclock window or in middle of transition to lock. */
	if((BVDC_P_SRC_IS_MPEG(hSource->eId)) &&
	   (hSource->ulConnectedWindow == 1) &&
	   (!hSource->hSyncLockCompositor) &&
	   (!hWindow->hCompositor->hSyncLockSrc) &&
	   (!hWindow->hCompositor->hSrcToBeLocked))
	{
		BDBG_MSG(("Making initial window[%d] synclock(1)", hWindow->eId));
		hSource->hSyncLockCompositor       = hWindow->hCompositor;
		hWindow->bSyncLockSrc              = true;
		hWindow->stCallbackData.stMask.bSyncLock = BVDC_P_DIRTY;
		hWindow->hBuffer->bSyncLock        = true;
		hWindow->hCompositor->ulSlip2Lock  = 1;
		hWindow->hCompositor->hSyncLockWin = hWindow;
		hWindow->hCompositor->hSyncLockSrc = hSource;
	}

	BDBG_LEAVE(BVDC_P_Source_ConnectWindow_isr);
	return;
}

/***************************************************************************
* {private}
*
*/
void BVDC_P_Source_DisconnectWindow_isr
( BVDC_Source_Handle               hSource,
 BVDC_Window_Handle               hWindow )
{
	int i;
	bool bCmpWasLocked;

	BDBG_ENTER(BVDC_P_Source_DisconnectWindow_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	BDBG_ASSERT(hSource->ahWindow[hWindow->eId]);
	hSource->ahWindow[hWindow->eId] = NULL;
	bCmpWasLocked = (hWindow->hCompositor->hSyncLockSrc != NULL);

	if(BVDC_P_WIN_IS_VIDEO_WINDOW(hWindow->eId))
	{
		uint32_t ulCount;

		hWindow->hCompositor->ulActiveVideoWindow--;
		hSource->ulConnectedWindow--;

		/* retest the source's vbi pass thru property since the disconnected
		* window could have control the vdec source's vbi pass through
		* settings. */
		/* (3) Update newly dis-connected windows that may have vbi-passthru */
		BVDC_P_Source_RefreshWindow_isr(hSource, true);

		/* Release memory for capture */
		ulCount = hWindow->hBuffer->ulActiveBufCnt;
		if(ulCount)
		{
			BVDC_P_HeapNodePtr apHeapNode[BVDC_P_MAX_MULTI_BUFFER_COUNT];
#if (BVDC_P_SUPPORT_3D_VIDEO)
			BVDC_P_HeapNodePtr apHeapNode_R[BVDC_P_MAX_MULTI_BUFFER_COUNT];
#endif

			BVDC_P_BUF_MSG(("Win[%d] free  %d buffers (%s)", hWindow->eId, ulCount,
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdRequest)));

#if (BVDC_P_SUPPORT_3D_VIDEO)
			if(hWindow->eBufAllocMode == BVDC_P_BufHeapAllocMode_eLRSeparate)
			{
				BVDC_P_BUF_MSG(("Win[%d] free  %d buffers (%s) for right", hWindow->eId, ulCount,
					BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdRequest)));

				BVDC_P_Buffer_ReleasePictureNodes_isr(hWindow->hBuffer,
					apHeapNode, apHeapNode_R, ulCount, 0);
				BVDC_P_BufferHeap_FreeBuffers_isr(hWindow->hHeap, apHeapNode_R,
					ulCount, false);
			}
			else
#endif
			{
				BVDC_P_Buffer_ReleasePictureNodes_isr(hWindow->hBuffer,
					apHeapNode, NULL, ulCount, 0);
			}

			BVDC_P_BufferHeap_FreeBuffers_isr(hWindow->hHeap, apHeapNode,
				ulCount, false);
			hWindow->ulBufCntAllocated -= ulCount;
		}
	}
	else
	{
		hWindow->hCompositor->ulActiveGfxWindow--;
	}

	/* If there is a Mpeg PIP that was depended on the Sync-lock to drive
	* the Mpeg feeder, we're now make that Mpeg feeder source a sync-lock
	* source which will taken over the responsibility of build the
	* vec/compositor as well. */
	if(hWindow->bSyncLockSrc)
	{
		hSource->hSyncLockCompositor       = NULL;
		hWindow->bSyncLockSrc              = false;
		hWindow->stCallbackData.stMask.bSyncLock = BVDC_P_DIRTY;
		hWindow->hCompositor->hSyncLockWin = NULL;
		hWindow->hCompositor->hSyncLockSrc = NULL;

		for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
		{
			BVDC_Window_Handle hTmpWindow;
			BVDC_Source_Handle hTmpSource;
			/* SKIP: If it's just created or inactive no need to build ruls. */
			if((!hWindow->hCompositor->ahWindow[i]) ||
				BVDC_P_STATE_IS_CREATE(hWindow->hCompositor->ahWindow[i]) ||
				BVDC_P_STATE_IS_INACTIVE(hWindow->hCompositor->ahWindow[i]))
			{
				continue;
			}

			hTmpWindow = hWindow->hCompositor->ahWindow[i];
			BDBG_OBJECT_ASSERT(hTmpWindow, BVDC_WIN);
			hTmpSource = hTmpWindow->stCurInfo.hSource;
			BDBG_OBJECT_ASSERT(hTmpSource, BVDC_SRC);
			if((BVDC_P_SRC_IS_MPEG(hTmpSource->eId)) &&
			   (!hTmpSource->hSyncLockCompositor) &&
			   (!hWindow->hCompositor->hSyncLockSrc) &&
			   (hTmpWindow != hWindow))
			{
				BDBG_MSG(("Making PIP window[%d] synclock(2)", hTmpWindow->eId));
				hTmpSource->hSyncLockCompositor    = hTmpWindow->hCompositor;
				hTmpWindow->bSyncLockSrc           = true;
				hTmpWindow->stCallbackData.stMask.bSyncLock = BVDC_P_DIRTY;
				hTmpWindow->hBuffer->bSyncLock     = true;
				hTmpWindow->stCurInfo.stDirty.stBits.bReallocBuffers = true;
				hWindow->hCompositor->hSyncLockWin = hTmpWindow;
				hWindow->hCompositor->hSyncLockSrc = hTmpSource;
				break;
			}
		}
	}

	/* There are windows on the other compositor that uses this source, but
	* was a sync slip.  Which could run at a different clock.  In order to
	* has it trigger this source we'd need properly transfering the
	* responsibility.  Otherwise FEEDER_ERROR_INTERRUPT_STATUS would ocurr.
	*
	* This is destroying synclock window with existing syncslip windows.
	* Delay one vsync to let the feeder drain by the old compositor. */
	if((hSource->ulConnectedWindow) &&
		(!hSource->hSyncLockCompositor) && bCmpWasLocked)
	{
		BVDC_P_Source_FindLockWindow_isr(hSource, false);
	}

	/* if mpeg source is disconnected with all windows, clean up source slots here,
	not in source callback in case immediate next applychange increment
	connected window count; */
	if(!hSource->ulConnectedWindow && BVDC_P_SRC_IS_MPEG(hSource->eId))
	{
		BVDC_P_Source_CleanupSlots_isr(hSource);
		BDBG_MSG(("Clean up mfd[%d] slots 'cuz last window disconnected",
			hSource->eId));
		hSource->ulTransferLock = 0;
	}
	BDBG_LEAVE(BVDC_P_Source_DisconnectWindow_isr);
	return;
}

/***************************************************************************
* {private}
*
* Return scan out rectangle for feeder, and update source content rect for all the
* connected scalers.
*
* pScanOutRect should always within source rectangle of the source, and in the
* format described below. Pan scan is calculated for macroblock, pixel and sub-pixel
* because of the frame buffer organization in MVD.
*
*   pScanOutRect->lLeft:
*      This is the horizontal pan scan for feeder, a positive number in the format of U32,
*      where bits 0-3 are pixel pan scan in pixel grid, bits 4-31 are macroblock pan
*      scan in macroblock grid.
*  pScanOutRect->lTop:
*      This is the vertical pan scan for feeder, a positive number in the format of U32,
*      where bits 0-3 are pixel pan scan in pixel grid, bits 4-31 are macroblock pan scan
*      in macroblock grid.
*  pScanOutRect->ulWidth:
*      This is the horizontal scan out size, a positive number in units of pixel.
*  pScanOutRect->ulHeight:
*      This is the vertical scan out size, a positive number in units of pixel.
*
* ----------------------------------------------------------------------
*  Window source clipping or pan/scan diagram:
*
*       +---------------------------------------+
*       |                SourceRect             |
*       |    +-----------------------------+    |
*       |    | +---------+    ScanOutRect  |    |
*       |    | |  win1   |                 |    |
*       |    | | Source  |                 |    |
*       |    | | Content |                 |    |
*       |    | |     +---|---------------+ |    |
*       |    | |     |   |   win2        | |    |
*       |    | +-----|---+  source       | |    |
*       |    |       |      content      | |    |
*       |    |       +-------------------+ |    |
*       |    +-----------------------------+    |
*       +---------------------------------------+
* Note:
*   - the feeder will handle the source clipping from SourceRect to ScanOutRect;
*   - the window scaler will handle source clipping from ScanOutRect to the
*     individual windows source content;
*   - since the Mpeg Feeder has 420 format chroma source data, it can do
*     clipping or pan/scan down to even line and even colomn only; the rest of
*     source clipping or pan/scan will be handled by scaler;
*   - we always pass non-negative pan/scan vector to scaler;
*/
void BVDC_P_Source_GetScanOutRect_isr
( BVDC_Source_Handle               hSource,
 const BAVC_MVD_Field            *pMvdFieldData,
 const BAVC_VDC_HdDvi_Picture    *pXvdFieldData,
 BVDC_P_Rect                     *pScanOutRect )
{
	int i;
#if (BVDC_P_MADR_PICSIZE_WORKAROUND)
	uint32_t    ulBitsPerPixel = 0, ulMadrCnt = 0;
	BVDC_P_Mcvp_Handle hMcvp;
#endif

	BDBG_ENTER(BVDC_P_Source_GetScanOutRect_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	BDBG_ASSERT(pScanOutRect);
	BSTD_UNUSED(pXvdFieldData);
	/* itu656/analog scanout everything. */
	if(BVDC_P_SRC_IS_ANALOG(hSource->eId) ||
	   BVDC_P_SRC_IS_ITU656(hSource->eId) ||
	   BVDC_P_SRC_IS_DS(hSource->eId))
	{
		const BFMT_VideoInfo *pFmtInfo = hSource->stCurInfo.pFmtInfo;
		pScanOutRect->lLeft    = 0;
		pScanOutRect->lLeft_R  = 0;
		pScanOutRect->lTop     = 0;
		pScanOutRect->ulWidth  = pFmtInfo->ulWidth;
		pScanOutRect->ulHeight = pFmtInfo->ulHeight;

		/* Adjust for oversample */
		pScanOutRect->ulWidth =
			BVDC_P_OVER_SAMPLE(hSource->ulSampleFactor, pScanOutRect->ulWidth);
	}
	else if(BVDC_P_SRC_IS_HDDVI(hSource->eId))
	{
		const BFMT_VideoInfo *pFmtInfo = hSource->stCurInfo.pFmtInfo;
		pScanOutRect->lLeft    = 0;
		pScanOutRect->lLeft_R  = 0;
		pScanOutRect->lTop     = 0;
		pScanOutRect->ulWidth  = pFmtInfo->ulDigitalWidth;

#if BFMT_LEGACY_3DTV_SUPPORT
		if(BFMT_IS_3D_MODE(pFmtInfo->eVideoFmt)) /* 3D format */
		{
			if(hSource->stCurInfo.stHdDviSetting.bEnableDe &&
				hSource->stCurInfo.bHVStartOverride)
			{
				pScanOutRect->ulHeight = pFmtInfo->ulDigitalHeight - hSource->stCurInfo.ulVstart + pFmtInfo->ulTopActive - 1;
			}
			else
			{
				pScanOutRect->ulHeight = pFmtInfo->ulDigitalHeight;
			}
		}
		else
#endif
		{
			pScanOutRect->ulHeight = pFmtInfo->ulDigitalHeight;

#if (BVDC_P_SUPPORT_3D_VIDEO)
			/* bOrientationOverride only valid when original
			* orientation from FMT is 2D */
			if(pFmtInfo->eOrientation == BFMT_Orientation_e2D)
			{
				if(hSource->stCurInfo.bOrientationOverride)
				{
					if(hSource->stCurInfo.eOrientation == BFMT_Orientation_e3D_LeftRight)
					{
						pScanOutRect->ulWidth = pScanOutRect->ulWidth / 2;
					}
					else if(hSource->stCurInfo.eOrientation == BFMT_Orientation_e3D_OverUnder)
					{
						pScanOutRect->ulHeight = pScanOutRect->ulHeight / 2;
					}
				}
			}
#endif
		}
	}
	else if (BVDC_P_SRC_IS_VFD(hSource->eId))
	{
		BVDC_P_SurfaceInfo  *pCurSur;
		int32_t  lWinXMin, lWinYMin, lWinXMax, lWinYMax;
		int32_t  lWinXMin_R;
		int32_t  lXSize, lYSize;

		BVDC_P_Window_GetSourceContentRect_isr(
			hSource->hVfdFeeder->hWindow, NULL, NULL,
			&lWinXMin, &lWinYMin, &lWinXMin_R, &lWinXMax, &lWinYMax);

		/* Note: BVDC_P_16TH_PIXEL_SHIFT = 4 */
		lXSize = (lWinXMax - lWinXMin + 0xF) >> 4;
		lYSize = (lWinYMax - lWinYMin + 0xF) >> 4;

		pCurSur = &(hSource->hVfdFeeder->stGfxSurface.stCurSurInfo);
		BVDC_P_Window_AlignPreMadRect_isr(lWinXMin, lWinXMin_R, lXSize, pCurSur->ulWidth,
			4, 2, &(pScanOutRect->lLeft), &(pScanOutRect->lLeft_R),
			&(pScanOutRect->ulWidth));
		BVDC_P_Window_AlignPreMadRect_isr(lWinYMin, lWinYMin, lYSize, pCurSur->ulHeight,
			4, 2, &(pScanOutRect->lTop), NULL,
			&(pScanOutRect->ulHeight));

		pScanOutRect->ulWidth = (pScanOutRect->ulWidth + 1) & ~1;
		pScanOutRect->ulHeight = (pScanOutRect->ulHeight + 1) & ~1;
	}
	else /* MFD */
	{
		int32_t  lXMin, lYMin, lXMax, lYMax;
		int32_t  lWinXMin, lWinYMin, lWinXMax, lWinYMax;
		int32_t  lFullSrcWidth, lFullSrcHeight;
		int32_t  lXSize, lYSize;
		int32_t  ulAlignUnit;
		int32_t  lWinXMin_R, lXMin_R;

		/* lXMin is the left, lYMin is the top, lXMax is the right, and lYMax is the bettom
		* of the scan out rectangle. All are signed number in the format of S27.4, where
		* bits 0-3 are sub-pixel values in 1/16 pixel grid, bits 4-30 are pixel values in
		* pixel grid, bit 31 is the sign bit.*/
		/* Initialize the scan out rectangle to the full source size */
		BDBG_ASSERT(pMvdFieldData);
		lFullSrcWidth  = pMvdFieldData->ulSourceHorizontalSize;
		lFullSrcHeight = pMvdFieldData->ulSourceVerticalSize;
		lXMin = lFullSrcWidth  << BVDC_P_16TH_PIXEL_SHIFT;
		lYMin = lFullSrcHeight << BVDC_P_16TH_PIXEL_SHIFT;
		lXMin_R = lXMin;
		lXMax = 0;
		lYMax = 0;

		/* Find the union of user pan scan of all the windows this source connected to */
		for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
		{
			/* SKIP: If it's just created or inactive no need to build ruls. */
			if(!hSource->ahWindow[i] ||
				BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
				BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
			{
				continue;
			}

#if (BVDC_P_MADR_PICSIZE_WORKAROUND)
			hMcvp = hSource->ahWindow[i]->stCurResource.hMcvp;

			if(hMcvp && hMcvp->hMcdi)
			{
				if(hMcvp->hMcdi->bMadr)
				{
					ulMadrCnt++;
					ulBitsPerPixel = hSource->ahWindow[i]->stCurInfo.stMadSettings.stTestFeature1.ulBitsPerPixel;
					/* Assert for more than 1 MADR */
					BDBG_ASSERT(ulMadrCnt <= 1);
				}
			}
#endif

			/* Get the source content rectangle for the window.
			* lWinXMin, lWinYMin, lWinXMax and lWinYMax in S27.4 format;
			* Note: the returned source content rect will be bound by source rect.  */
			BVDC_P_Window_GetSourceContentRect_isr(hSource->ahWindow[i],
				pMvdFieldData, NULL, &lWinXMin, &lWinYMin, &lWinXMin_R,
				&lWinXMax, &lWinYMax);

			/* Combine it with previous value */
			lXMin = BVDC_P_MIN(lXMin, lWinXMin);
			lYMin = BVDC_P_MIN(lYMin, lWinYMin);
			lXMin_R = BVDC_P_MIN(lXMin_R, lWinXMin_R);
			lXMax = BVDC_P_MAX(lXMax, lWinXMax);
			lYMax = BVDC_P_MAX(lYMax, lWinYMax);
		}

		/* Handle the case when no window connect to source. Make sure
		* lXMax is greater or equal to lXMin, and lYMax is greater or
		* equal to lYMin. */
		if( lXMin > lXMax )
		{
			lXMax = lXMin;
			lXMin = 0;
		}
		if( lYMin > lYMax )
		{
			lYMax = lYMin;
			lYMin = 0;
		}

		/* Make sure lXMin_R is not too big */
		if( lXMin_R > lXMax )
		{
			lXMin_R = 0;
		}

		/* Currently MVD/XVD always pass 4:2:0 format to VDC. Therefore scanOut (left,
		* right) are expanded to multiple of 2 pixel boundary, (top, bot) are expanded
		* to multiple of 2 line in the progressive src case, and multiple of 4 line in
		* the interlaced src case.
		*
		* We expand here, rather than shrink, to avoid losing of video info. Down stream
		* CAP and/or SCL will clip up to sub-pixel position.
		*
		* Note: BVDC_P_16TH_PIXEL_SHIFT = 4 */
		lXSize = (lXMax - lXMin + 0xF) >> 4;
		lYSize = (lYMax - lYMin + 0xF) >> 4;
		/* if only one win is using this src, zoom and pan will get the same
		* scan size, so MAD don't need hard-start */
#if (BVDC_P_DCX_HSIZE_WORKAROUND || BVDC_P_MADR_HSIZE_WORKAROUND)
		BVDC_P_Window_AlignPreMadRect_isr(lXMin, lXMin_R, lXSize, lFullSrcWidth,
			4, 4, &(pScanOutRect->lLeft), &(pScanOutRect->lLeft_R),
			&(pScanOutRect->ulWidth));

#else
		BVDC_P_Window_AlignPreMadRect_isr(lXMin, lXMin_R, lXSize, lFullSrcWidth,
			4, 2, &(pScanOutRect->lLeft), &(pScanOutRect->lLeft_R),
			&(pScanOutRect->ulWidth));
#endif
		ulAlignUnit = (((BAVC_Polarity_eFrame != pMvdFieldData->eSourcePolarity) &&
			(BAVC_YCbCrType_e4_2_0 == pMvdFieldData->eYCbCrType))? 4 :
			((BAVC_Polarity_eFrame != pMvdFieldData->eSourcePolarity) ||
			(BAVC_YCbCrType_e4_2_0 == pMvdFieldData->eYCbCrType))? 2 : 1);
		BVDC_P_Window_AlignPreMadRect_isr(lYMin, lYMin, lYSize, lFullSrcHeight,
			4, ulAlignUnit, &(pScanOutRect->lTop), NULL,
			&(pScanOutRect->ulHeight));

		/* AlignPreMadRect_isr will not expands to more than original width from XMD,
		* and the original width might be odd number for aspect ratio rounding purpose */
		pScanOutRect->ulWidth = (pScanOutRect->ulWidth + 1) & ~1;
		pScanOutRect->ulHeight = (pScanOutRect->ulHeight + 1) & ~1;

#if (BVDC_P_MVFD_ALIGNMENT_WORKAROUND)
		if((hSource->hMpegFeeder->bGfxSrc && (pMvdFieldData->ePxlFmt == BPXL_INVALID)) ||
		   BPXL_IS_YCbCr422_FORMAT(pMvdFieldData->ePxlFmt))
		{
			if(pScanOutRect->ulWidth % BVDC_P_SCB_BURST_SIZE == 2)
			{
				/* Crop */
				pScanOutRect->ulWidth -= 2;
			}
		}
#endif

#if (BVDC_P_MADR_PICSIZE_WORKAROUND)
		BVDC_P_Window_PreMadAdjustWidth_isr(pScanOutRect->ulWidth,
			(pMvdFieldData->eSourcePolarity == BAVC_Polarity_eFrame) ?
			pScanOutRect->ulHeight : pScanOutRect->ulHeight /2,
			BVDC_P_MADR_DCXS_COMPRESSION(ulBitsPerPixel),
			&pScanOutRect->ulWidth);
#endif

	}

	BDBG_LEAVE(BVDC_P_Source_GetScanOutRect_isr);
	return;
}


/***************************************************************************
* {private}
*
* Collect the window related information based on vnetmode for all the
* windows connected to the source. Need to make sure vnetmode.is
* determined.
*/
void BVDC_P_Source_GetWindowVnetmodeInfo_isr
( BVDC_Source_Handle               hSource )
{
	int   i;
	bool  bTestFeature1 = false;
	BVDC_Window_Handle hWindow;

	BDBG_ENTER(BVDC_P_Source_GetWindowVnetmodeInfo_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	hSource->bWait4ReconfigVnet = false;
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if((!hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_DESTROY(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_SHUTDOWNRUL(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_SHUTDOWNPENDING(hSource->ahWindow[i]))
		{
			continue;
		}

		hWindow = hSource->ahWindow[i];
		BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

		if( hWindow->stCurInfo.stDirty.stBits.bReConfigVnet ||
			hWindow->stCurInfo.stDirty.stBits.bReDetVnet)
		{
			/* Wait until reconfig vnet is done */
			hSource->bWait4ReconfigVnet = true;
			BDBG_MSG(("Src[%d] Wait for win%d recofig vnet", hSource->eId, hWindow->eId));
		}
		else
		{
			/* If Test Feature is enabled for AND or MAD or CAP */
			if((BVDC_P_VNET_USED_CAPTURE(hWindow->stVnetMode) &&
				hWindow->stCurInfo.stCapTestFeature1.bEnable) ||
				(BVDC_P_VNET_USED_ANR(hWindow->stVnetMode) &&
				hWindow->stCurInfo.stAnrSettings.stVideoTestFeature1.bEnable) ||
				(BVDC_P_VNET_USED_MAD(hWindow->stVnetMode) &&
				hWindow->stCurInfo.stMadSettings.stTestFeature1.bEnable))
			{
				bTestFeature1 = true;
			}
		}
	}

	if(!hSource->bWait4ReconfigVnet &&
		(hSource->bTestFeature1 != bTestFeature1))
	{
		BDBG_MSG(("Src[%d] Switch bTestFeature1 %d->%d", hSource->eId,
			hSource->bTestFeature1, bTestFeature1));

		hSource->bTestFeature1 = bTestFeature1;
		hSource->stCurInfo.stDirty.stBits.bWinChanges = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_P_Source_GetWindowVnetmodeInfo_isr);
	return;
}

/***************************************************************************
*
*/
void BVDC_P_Source_MpegGetStatus_isr
( const BVDC_Source_Handle        hSource,
 bool                            *pbVideoDetected )
{
	/* Sanity check and get context */
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(pbVideoDetected)
	{
		*pbVideoDetected = hSource->stPrevMvdField.bMute ? false : true;
	}

	return;
}


/***************************************************************************
*
*/
void BVDC_P_Source_BuildRul_isr
( const BVDC_Source_Handle         hSource,
 BVDC_P_ListInfo                 *pList )
{
	/* hush compiler */
	BSTD_UNUSED(pList);

	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	if(BVDC_P_SRC_IS_ANALOG(hSource->eId))
	{
		BVDC_P_Vdec_BuildRul_isr(hSource->hVdec, pList, hSource->eNextFieldIntP);
	}
	else
	{
		BDBG_ASSERT(BVDC_P_SRC_IS_ITU656(hSource->eId));
#if (BVDC_P_SUPPORT_NEW_656_IN_VER)
		BVDC_P_656In_BuildRul_isr(hSource->h656In, pList, hSource->eNextFieldIntP);
#else
		BVDC_P_Vdec_BuildRul_isr(hSource->hVdec, pList, hSource->eNextFieldIntP);
#endif
	}

	return;
}


/***************************************************************************
*
*/
void BVDC_P_Source_UpdateFrameRate_isr
( const BFMT_VideoInfo            *pFmtInfo,
 uint32_t                         ulClkPerVsync,
 uint32_t                         ulDelta,
 BAVC_FrameRateCode              *peFrameRateCode )
{
	uint32_t i;
	uint32_t ulCount;
	const BVDC_P_FrameRateEntry *pFrTbl;

	if(pFmtInfo->bInterlaced)
	{
		ulCount = BVDC_P_FRT_COUNT_INTERLACED;
		pFrTbl  = s_aIntFramerateTbl;
	}
	else
	{
		ulCount = BVDC_P_FRT_COUNT_PROGRESSIVE;
		pFrTbl  = s_aProFramerateTbl;
	}

	for(i = 0; i < ulCount; i++)
	{
		if(BVDC_P_EQ_DELTA(ulClkPerVsync, pFrTbl[i].ulClkPerVsync, ulDelta))
		{
			*peFrameRateCode = pFrTbl[i].eFramerate;
			break;
		}
	}

	return;
}

/***************************************************************************
*
*/
void BVDC_P_Source_CheckAndIssueCallback
( BVDC_Source_Handle               hSource,
 BVDC_Source_DirtyBits           *pCbDirty )
{
	uint32_t i;

	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if(!hSource->ahWindow[i] ||
			BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
		{
			continue;
		}

		BDBG_OBJECT_ASSERT(hSource->ahWindow[i], BVDC_WIN);

		if(hSource->ahWindow[i]->stCurInfo.stDirty.stBits.bShutdown)
		{
			break;
		}
	}

	/* any window incompletes shutdown, don't callback source pending; */
	if(i == BVDC_P_MAX_WINDOW_COUNT)
	{
		BDBG_MSG(("hSource[%d] issues source pending callback!", hSource->eId));
		pCbDirty->bSrcPending       = true;

		/* clear the defer flag */
		hSource->bDeferSrcPendingCb = false;
	}

	return;
}

/***************************************************************************
*
*/
void BVDC_P_Source_AnalogDataReady_isr
( void                            *pvSourceHandle,
 int                              iParam2 )
{
	int i;
	BVDC_P_ListInfo stList;
	BVDC_Source_Handle hSource = (BVDC_Source_Handle)pvSourceHandle;
	BVDC_P_Source_Info *pCurInfo;
	BRDC_Slot_Handle hSlot, hOtherSlot;
	BRDC_List_Handle hList;

	BDBG_ENTER(BVDC_P_Source_AnalogDataReady_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* We should not even get the Gfx or mpeg source here. */
	BDBG_ASSERT(
		BVDC_P_SRC_IS_ANALOG(hSource->eId) ||
		BVDC_P_SRC_IS_ITU656(hSource->eId));

	/* Analog/656 source is always sync-slip with a compositor/vec. */
	BDBG_ASSERT(!hSource->hSyncLockCompositor);

	/* Make sure the BKNI enter/leave critical section works. */
	BDBG_ASSERT(0 == hSource->hVdc->ulInsideCs);
	hSource->hVdc->ulInsideCs ++;

	/* Update source user info */
	BVDC_P_Source_UpdateSrcState_isr(hSource);

	/* Update source info */
	BVDC_P_Source_UpdateStatus_isr(hSource);

	/* Get current settings */
	pCurInfo = &hSource->stCurInfo;

	/* for progressive src format, we always expect Top slot interrupt */
	hSource->eNextFieldIntP = iParam2;

	/* this is actually the current slot */
	hSource->eNextFieldId   = (pCurInfo->pFmtInfo->bInterlaced)
		? BVDC_P_NEXT_POLARITY(iParam2) : BAVC_Polarity_eFrame;

#if (BVDC_P_SUPPORT_VDEC_VER < BVDC_P_VDEC_VER_4)
	/* for interlaced pc input, hijack the field id */
	if((pCurInfo->bPcInput) &&
		(pCurInfo->pFmtInfo->bInterlaced))
	{
		hSource->eNextFieldIntP = hSource->ePcFieldId;
		hSource->eNextFieldId   = BVDC_P_NEXT_POLARITY(hSource->ePcFieldId);
	}
#endif

	/* Get the approriate slot/list for building RUL. */
	BVDC_P_SRC_NEXT_RUL(hSource, hSource->eNextFieldIntP);
	hSlot = BVDC_P_SRC_GET_SLOT(hSource, hSource->eNextFieldIntP);
	hList = BVDC_P_SRC_GET_LIST(hSource, hSource->eNextFieldIntP);

	/* Update the status of last executed RUL. */
	BRDC_Slot_UpdateLastRulStatus_isr(hSlot, hList, true);

	/* Assert: list is not connected to any slot */
	BRDC_List_SetNumEntries_isr(hList, 0);

	/* Get current pointer to RUL and info. */
	BVDC_P_ReadListInfo_isr(&stList, hList);
	stList.bMasterList = true;

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

#if (BVDC_P_CLEANUP_VNET)
		if(BVDC_MuteMode_eRepeat != pCurInfo->eMuteMode)
		{
			/* Need to initiate window shutdown when video detect
			* status toggles to drain VNET switches and reset BVN blocks to clean
			* up BVB error;  for this new VDEC.  Resetting vnet has side effects
			* blank screen with background color mix with source mute color. */
			/* currently we have to reconfigure vnet to correct AND after 3D comb
			* turns from off to on*/
			if((BVDC_P_SRC_IS_ANALOG(hSource->eId)) &&
				(hSource->bStartFeed != hSource->bPrevStartFeed))
			{
				if(hSource->bStartFeed)
				{
					BDBG_MSG(("Source[%d] Reconfigs vnet!", hSource->eId));
					BVDC_P_Window_SetReconfiguring_isr(hSource->ahWindow[i], false, true);
				}
			}
		}
#endif

		BVDC_P_Window_AdjustRectangles_isr(hSource->ahWindow[i], NULL, NULL, 0);
	}

	/* Defer update to make sure all windows get a chance of reconfiguring */
#if (BVDC_P_CLEANUP_VNET)
	if(BVDC_MuteMode_eRepeat != pCurInfo->eMuteMode)
	{
		hSource->bPrevStartFeed = hSource->bStartFeed;
	}
#endif

	/* Get the source scan out rectangle. Combine the user pan-scan info
	* from all the window that uses this source;
	* Note: pMvdField, pXvdField = NULL for analog video source. */
	if(pCurInfo->stDirty.stBits.bRecAdjust)
	{
		BVDC_P_Source_GetScanOutRect_isr(hSource, NULL, NULL, &hSource->stScanOut);
	}

	/* For each window using this source do the following. */
	for(i = 0; i < BVDC_P_MAX_WINDOW_COUNT; i++)
	{
		/* SKIP: If it's just created or inactive no need to build ruls. */
		if(!hSource->ahWindow[i] ||
			BVDC_P_STATE_IS_CREATE(hSource->ahWindow[i]) ||
			BVDC_P_STATE_IS_INACTIVE(hSource->ahWindow[i]))
		{
			continue;
		}

		BVDC_P_Window_Writer_isr(hSource->ahWindow[i], NULL, NULL,
			hSource->eNextFieldId, &stList, 0);

		/* skip window RUL if shutdown state */
		if(BVDC_P_State_eShutDown != hSource->ahWindow[i]->stCurInfo.eWriterState)
		{
			BVDC_P_Window_BuildRul_isr(hSource->ahWindow[i], &stList, hSource->eNextFieldId,
				true,  /* writer*/
				false, /* reader */
				false  /* canvasCtrl */ );
		}

		/* Window needs to adjust CSC because pedestal may have changed. */
		hSource->ahWindow[i]->stCurInfo.stDirty.stBits.bCscAdjust |=
			pCurInfo->stDirty.stBits.bMiscCtrl;
	}

	if(pCurInfo->stDirty.stBits.bRecAdjust || hSource->bWait4ReconfigVnet)
	{
		/* Gather window information after vnetmode is determined */
		BVDC_P_Source_GetWindowVnetmodeInfo_isr(hSource);
	}

	/* Now build the Vdec/656 block. */
	BVDC_P_Source_BuildRul_isr(hSource, &stList);

	/* UPdate current pictures, the eSourcePolarity must update every field. */
	hSource->bPictureChanged = false;
	hSource->bRasterChanged = false;

	/* Update entries count */
	BVDC_P_WriteListInfo_isr(&stList, hList);

	/* This programs the slot after next. This is needed in conjunction with
	* the programming of the next slot above and for accommodating same field
	* polarity sources. Moreover, this is so to make the 4 buffer algorithm
	* work correctly. */
	hOtherSlot = BVDC_P_SRC_GET_SLOT(hSource,
		BVDC_P_NEXT_POLARITY(hSource->eNextFieldIntP));

	/* Always assign single RUL to two slots to avoid uncovered transition
	* from dual to one; */
	/* Note: to flush the cached RUL only once, call the Dual function
	instead of two individual slot functions; */
	BRDC_Slot_SetListDual_isr(hSlot, hOtherSlot, hList);
	hSource->hVdc->ulInsideCs --;
	BDBG_LEAVE(BVDC_P_Source_AnalogDataReady_isr);
	return;
}

/***************************************************************************
*
*/
void BVDC_P_Source_VfdGfxDataReady_isr
	( BVDC_Source_Handle               hSource,
	  BVDC_Window_Handle               hWindow,
	  BVDC_P_ListInfo                 *pList,
	  int                              iParam2 )
{
	BVDC_P_Source_Info  *pCurInfo;
	BVDC_P_Feeder_Handle  hVfdFeeder;

	BDBG_ENTER(BVDC_P_Source_VfdGfxDataReady_isr);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* We should not even get the Gfx or mpeg source here. */
	BDBG_ASSERT(BVDC_P_SRC_IS_VFD(hSource->eId));

	/* Update source user info */
	BVDC_P_Source_UpdateSrcState_isr(hSource);

	/* Get current settings */
	pCurInfo = &hSource->stCurInfo;

	/* for progressive src format, we always expect Top slot interrupt */
	hSource->eNextFieldIntP = iParam2;

	/* this is actually the current slot */
	hSource->eNextFieldId = BVDC_P_NEXT_POLARITY(iParam2);

	/* If surface has been set by *SetSurface_isr, or src pic call back func is
	 * installed, we activate it now */
	hVfdFeeder = hSource->hVfdFeeder;
	BVDC_P_Feeder_HandleIsrGfxSurface_isr(hVfdFeeder, pCurInfo, hSource->eNextFieldId);
	hSource->bPictureChanged = hVfdFeeder->stGfxSurface.stCurSurInfo.bChangeSurface;

	if (hVfdFeeder->stGfxSurface.stCurSurInfo.ulAddress)
	{
		/* ajust rectangles */
		BVDC_P_Window_AdjustRectangles_isr(hWindow, NULL, NULL, 0);

		if (hVfdFeeder->hWindow)
		{
			BVDC_P_Source_GetScanOutRect_isr(hSource, NULL, NULL, &hSource->stScanOut);
		}

		/* we only call BVDC_P_Window_Writer_isr to setup vnet and picture node
		 * RUL will be built with reader in display interrupt _isr */
		if( !BVDC_P_STATE_IS_CREATE(hWindow) &&
			!BVDC_P_STATE_IS_INACTIVE(hWindow) )
		{
			BVDC_P_Window_Writer_isr(hWindow, NULL, NULL, hSource->eNextFieldId, pList, 0);
		}

		/* Update current pictures, the eSourcePolarity must update every field. */
		hSource->bPictureChanged = false;
		hSource->bRasterChanged = false;
		hVfdFeeder->stGfxSurface.stCurSurInfo.bChangeSurface = false;
	}
	else
	{
		BVDC_P_CLEAN_ALL_DIRTY(&hWindow->stCurInfo.stDirty);
	}

	BDBG_LEAVE(BVDC_P_Source_VfdGfxDataReady_isr);
	return;
}

/***************************************************************************
*
*/
void BVDC_P_Source_MfdGfxCallback_isr(void *pvSourceHandle, int iField)
{
	BAVC_MVD_Field  stNewPic;
	BVDC_Source_Handle  hSource;
	BVDC_P_Source_Info  *pCurInfo;
	BVDC_P_Feeder_Handle  hMpegFeeder;
	BAVC_Polarity  eNextFieldId;

	hSource = (BVDC_Source_Handle) pvSourceHandle;
	pCurInfo = &hSource->stCurInfo;

	/* this is actually the current slot */
	eNextFieldId = BVDC_P_NEXT_POLARITY(iField);

	/* If surface has been set by *SetSurface_isr, or src pic call back func is
	 * installed, we activate it now */
	hMpegFeeder = hSource->hMpegFeeder;
	BVDC_P_Feeder_HandleIsrGfxSurface_isr(hMpegFeeder, pCurInfo, eNextFieldId);
	hSource->bPictureChanged = hMpegFeeder->stGfxSurface.stCurSurInfo.bChangeSurface;

	BKNI_Memset((void*)&stNewPic, 0x0, sizeof(BAVC_MVD_Field));

	stNewPic.eInterruptPolarity =
		(NULL!=hSource->hSyncLockCompositor)? eNextFieldId : (BAVC_Polarity) iField;
	if (hMpegFeeder->stGfxSurface.stCurSurInfo.ulAddress)
	{
		stNewPic.eSourcePolarity = BAVC_Polarity_eFrame;
		stNewPic.bStreamProgressive = true;
		stNewPic.bFrameProgressive = true;
		stNewPic.eMpegType = BAVC_MpegType_eMpeg2;
		stNewPic.eYCbCrType = BAVC_YCbCrType_e4_2_2;
		stNewPic.eChrominanceInterpolationMode = BAVC_InterpolationMode_eFrame;
	
		stNewPic.ulSourceHorizontalSize  = hMpegFeeder->stGfxSurface.stCurSurInfo.ulWidth;
		stNewPic.ulSourceVerticalSize    = hMpegFeeder->stGfxSurface.stCurSurInfo.ulHeight;
		stNewPic.ulDisplayHorizontalSize = hMpegFeeder->stGfxSurface.stCurSurInfo.ulWidth;
		stNewPic.ulDisplayVerticalSize   = hMpegFeeder->stGfxSurface.stCurSurInfo.ulHeight;
		stNewPic.ulRowStride = hMpegFeeder->stGfxSurface.stCurSurInfo.ulPitch;

		/* Jira SW7405-4239: in case surface has odd width/height */
		stNewPic.ulSourceHorizontalSize = stNewPic.ulSourceHorizontalSize & ~1;
		stNewPic.ulSourceVerticalSize   = stNewPic.ulSourceVerticalSize & ~1;

		/* mark this MpegDataReady call as from BVDC_P_Source_MfdGfxCallback_isr */
		stNewPic.pLuminanceFrameBufferAddress = (void *)hMpegFeeder->stGfxSurface.stCurSurInfo.ulAddress;

		/* Check if fields that are to be written to registers fit within their field sizes. */
		if(!BVDC_P_SRC_VALIDATE_FIELD(MFD_0_STRIDE, PACKED_LINE_STRIDE, stNewPic.ulRowStride))
		{
			BDBG_ERR(("ulRowStride is invalid"));
			stNewPic.ulRowStride = BVDC_P_SRC_FIELD_DATA_MASK(MFD_0_STRIDE,
				PACKED_LINE_STRIDE, stNewPic.ulRowStride);
		}

		stNewPic.ePxlFmt = hMpegFeeder->stGfxSurface.stCurSurInfo.eInputPxlFmt;
		stNewPic.eOrientation = hMpegFeeder->stGfxSurface.stCurSurInfo.stAvcPic.eInOrientation;
		stNewPic.ulOrigPTS = hMpegFeeder->stGfxSurface.stCurSurInfo.stAvcPic.ulOrigPTS;

		/* TODO: should get hMpegFeeder->stGfxSurface.stCurSurInfo.stAvcPic.hSurface->hMem */
		stNewPic.hHeap = hSource->hVdc->hMemory;

#if 0
		/* currently BVDC_Source_SetSurface can not use interlaced surface */
		/* top fld sur only has even lines (including line 0) of the frame,
		 * and bot sur only has odd lines */
		if(BAVC_Polarity_eFrame != stNewPic.eSourcePolarity)
		{
			stNewPic.ulSourceVerticalSize *= 2;
		}
#endif
	}

	else /* no valid surface yet */
	{
		stNewPic.bMute = true;
		stNewPic.bIgnorePicture = true;
	}

	/* call BVDC_Source_MpegDataReady_isr: get the surface built into the RUL which will
	 * be executed after current field/frame display is done
	 * note: must keep calling even if hMpegFeeder->hWindow becomes NULL, otherwise window
	 * shutdown will time out */
	BVDC_Source_MpegDataReady_isr( pvSourceHandle, iField, &stNewPic );
}

/* End of file */
