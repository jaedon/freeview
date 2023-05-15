/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_gfxfeeder_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/203 $
 * $brcm_Date: 9/13/12 5:05p $
 *
 * Module Description:
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_gfxfeeder_priv.c $
 * 
 * Hydra_Software_Devel/203   9/13/12 5:05p syang
 * SW7425-2093: set W even without separate alpha
 * 
 * Hydra_Software_Devel/202   8/27/12 5:27p tdo
 * SW7445-8: Add missing include file
 *
 * Hydra_Software_Devel/201   8/16/12 2:42p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/200   6/14/12 3:42p syang
 * SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
 * prepare to add SetSurface support to MFD
 *
 * Hydra_Software_Devel/199   6/12/12 6:50p darnstein
 * SW7425-3226: fix Coverity errors involving suspicious tests for NULL
 * pointer.
 *
 * Hydra_Software_Devel/198   5/30/12 4:54p syang
 * SW7425-2093: extra ApplyChange with 1st SetSurface no longer needed;
 * vfd/cap time stamp related scratch registers now share with vfd's
 * surface manager's; added vfd resource check; more clean up
 *
 * Hydra_Software_Devel/197   5/23/12 6:46p syang
 * SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
 * use gfxsurface sub-module
 *
 * Hydra_Software_Devel/196   5/15/12 3:00p syang
 * SW7425-2093:don't free scratch register until BVDC_Close
 *
 * Hydra_Software_Devel/195   5/9/12 5:31p syang
 * SW7425-2093: fix coverity error in _Init()
 *
 * Hydra_Software_Devel/194   5/9/12 2:55p syang
 * SW7425-2093:  fix compile error for diff chips
 *
 * Hydra_Software_Devel/191   5/9/12 10:53a syang
 * SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
 * error fix with GetSurface; use shared gfxsurface to manage gfx surface
 *
 * Hydra_Software_Devel/190   3/8/12 11:50a yuxiaz
 * SW7425-2546: Fixed warning msg.
 *
 * Hydra_Software_Devel/189   1/31/12 5:46p vanessah
 * SW7425-2080: return with BERR_TRACE
 *
 * Hydra_Software_Devel/188   1/30/12 1:55p vanessah
 * SW7425-2287: roll back 7425/7231/7346 Bx GFX version due to incomplete
 * HW fixes set
 *
 * Hydra_Software_Devel/187   1/29/12 3:45p vanessah
 * SW7425-2287:  rollback workaround for HW7425-385 for GFD_VER_5
 *
 * Hydra_Software_Devel/186   1/27/12 11:21p vanessah
 * SW7425-2287: remove workaround for HW7425-385 for GFD_VER_5
 *
 * Hydra_Software_Devel/185   1/26/12 12:05p vanessah
 * SW7429-42: read vscl line buffer length from HW_conf register
 *
 * Hydra_Software_Devel/184   11/23/11 4:25p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/183   11/17/11 2:01p pntruong
 * SW7425-1078: Addressed warnings.
 *
 * Hydra_Software_Devel/182   9/29/11 3:20p vanessah
 * SW7425-1078: add warning msg on 3D OU for A_0 and P_0 format
 *
 * Hydra_Software_Devel/181   7/7/11 7:17p vanessah
 * SW7231-279: fix current info and new info confusion
 *
 * Hydra_Software_Devel/180   6/30/11 4:48p vanessah
 * SW7425-800: fix buffer offset bug
 *
 * Hydra_Software_Devel/179   6/9/11 10:29a vanessah
 * SW7231-187: workaround for HW init when orientation does not match with
 * the previous and the following one.
 *
 * Hydra_Software_Devel/178   6/8/11 5:33p vanessah
 * SW7231-188: Adding Orientation restriction
 *
 * Hydra_Software_Devel/177   6/8/11 4:16p vanessah
 * SW7231-188: fix GFX mis-configuration
 *
 * Hydra_Software_Devel/176   5/20/11 12:03p vanessah
 * SW7231-142: SW7425-603:  fix surface buffer update timing error
 *
 * Hydra_Software_Devel/175   5/17/11 5:14p vanessah
 * SW7425-404: further code clean-up on GFX Display format update
 *
 * Hydra_Software_Devel/174   5/17/11 4:12p vanessah
 * SW7425-404: update GFD display orientation correctly.
 *
 * Hydra_Software_Devel/173   5/17/11 10:16a vanessah
 * SW7425-404: Get rid of err msg
 *
 * Hydra_Software_Devel/172   5/16/11 6:31p vanessah
 * SW7425-404: update horizontal and verital scale flag
 *
 * Hydra_Software_Devel/171   4/29/11 3:03p vanessah
 * SW7422-118: add fix for vdc_test gfx
 *
 * Hydra_Software_Devel/170   4/29/11 1:33p vanessah
 * SW7422-118: add GFD cold-start
 *
 * Hydra_Software_Devel/169   4/28/11 7:15p vanessah
 * SW7422-118: fix double buffer setting error
 *
 * Hydra_Software_Devel/168   4/27/11 4:59p vanessah
 * SW7425-404: enable HSCL and VSCL for 3D due to jira HW7425-385
 *
 * Hydra_Software_Devel/167   4/25/11 1:38p pntruong
 * SW7125-919: Fixed leak scratch registers resources.  Also check for
 * error when doing scratch registers allocation.
 *
 * Hydra_Software_Devel/166   4/4/11 5:20p vanessah
 * SW7425-234: provide error msg for incorrect GFD parameters settings
 *
 * Hydra_Software_Devel/165   3/31/11 8:22p vanessah
 * SW7422-118: double buffer PI support
 *
 * Hydra_Software_Devel/164   3/23/11 4:50p vanessah
 * SW7550-704: set IOBuf_Enable always.
 *
 * Hydra_Software_Devel/163   3/22/11 8:03p vanessah
 * SW7231-69: fix src surface error in the bottom field calcualation.
 *
 * Hydra_Software_Devel/162   1/20/11 2:34p vanessah
 * SW7422-43:  GFD interlace issue with VSCL setting always true for 3D_OU
 * and 3D_LR.
 *
 * Hydra_Software_Devel/161   1/20/11 10:19a vanessah
 * SW7422-43:  Force the usage of HSCL and VSCL due to HW bug. Fix is
 * ready for B0 tapeout. (See HW7425-385)
 *
 * Hydra_Software_Devel/160   1/19/11 8:32a vanessah
 * SW7422-43:
 * coverity error fix
 *
 * Hydra_Software_Devel/159   1/18/11 4:53p vanessah
 * SW7422-43:  3D graphics support
 *
 * Hydra_Software_Devel/158   11/21/10 4:09p tdo
 * SW7422-43:  Fix GISB error with nexus framebuffer app, correctly set
 * START_R address
 *
 * Hydra_Software_Devel/157   11/19/10 10:35a pntruong
 * SW7550-610: Allowed filter when sharpness is selected.  Used applied
 * context.
 *
 * Hydra_Software_Devel/156   11/16/10 4:07p hongtaoz
 * SW7425-33: fixed compile error on legacy chips;
 *
 * Hydra_Software_Devel/155   11/16/10 3:05p hongtaoz
 * SW7425-33: added gfd3 support and vscl detection;
 *
 * Hydra_Software_Devel/154   11/11/10 7:28p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/153   11/10/10 4:36p albertl
 * SW7550-610: Extended BVDC_Window_SetCoefficientIndex to work for
 * graphics windows.
 *
 * Hydra_Software_Devel/152   11/4/10 9:40a vanessah
 * SW7422-43:  bug fix for 7425
 *
 * Hydra_Software_Devel/151   11/1/10 1:13p vanessah
 * SW7422-43:  merge 2D into 3D with a unified interface, change input
 * data structure as const
 *
 * Hydra_Software_Devel/150   10/29/10 5:07p vanessah
 * SW7422-43:  3D graphics render,merge 2D into 3D with a unified
 * interface
 *
 * Hydra_Software_Devel/149   10/25/10 7:20p vanessah
 * SW7422-43:  Handle GFD programming for 3D.
 *
 * Hydra_Software_Devel/148   10/11/10 12:34p jessem
 * SW7420-173: Added support for VDC as source feature.
 *
 * Hydra_Software_Devel/147   8/26/10 7:40p albertl
 * SW7405-4556, SW7405-4515: Changed RGB attenuation to use inverse of
 * user matrix C.
 *
 * Hydra_Software_Devel/146   8/26/10 5:27p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/145   6/16/10 3:05p yuxiaz
 * SW7405-4503, SW3548-2976: Black video on component and hdmi outputs in
 * the latest VDC.
 *
 * Hydra_Software_Devel/144   6/14/10 3:37p hongtaoz
 * SW3548-2976: soft init GFD when DCX switches from ON to OFF, and when
 * GFD source is created; added GFD BVB error monitoring to debug build;
 *
 * Hydra_Software_Devel/143   5/7/10 7:10p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/142   4/19/10 10:15p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/141   4/7/10 11:30a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/140   4/5/10 4:06p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/139   3/18/10 5:09p syang
 * SW7550-271: fixed compile arn for 3549
 *
 * Hydra_Software_Devel/138   3/18/10 4:00p syang
 * SW7550-271: added error check for unsupported 7550 GFD1 features
 *
 * Hydra_Software_Devel/137   3/17/10 12:02p syang
 * SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
 * same as video; ensure that reader and writer vnetState match in sync-
 * locked case; aligned width up to even for stride in feeder and cap;
 * assert mosaic mode doesn't co-exist with dest cut;
 *
 * Hydra_Software_Devel/136   2/3/10 6:23p albertl
 * SW7125-180: Updated TESTFEATURE1 defines for 7125.
 *
 * Hydra_Software_Devel/135   1/12/10 8:42p albertl
 * SW3548-2700: Changed RGB attenuation to use DVO matrix as YCbCr->RGB
 * matrix and Matrix C as RGB->YCbCr matrix.
 *
 * Hydra_Software_Devel/134   12/18/09 10:29a yuxiaz
 * SW7405-3633: Fix Coverity warings in VDC
 *
 * Hydra_Software_Devel/133   9/11/09 5:09p tdo
 * SW7420-338: Fix compiling error
 *
 * Hydra_Software_Devel/132   9/11/09 4:34p tdo
 * SW7420-338: Need to init correct GFD with correct capability
 *
 * Hydra_Software_Devel/131   9/4/09 4:59p darnstein
 * SW7342-36: Computation of GFD_0_SRC_VSIZE now similar to that of
 * GFD_0_DISP_PIC_SIZE.VSIZE.
 *
 * Hydra_Software_Devel/130   4/27/09 7:18p syang
 * PR 54634: 1) build dirty item twice; 2) set NewDirty.bChangeOutRect
 * when disp format changes between  interlaced  and progressive
 *
 * Hydra_Software_Devel/129   4/24/09 12:04a tdo
 * PR45785, PR45789: Clean up dirty bits after build RUL so that RUL will
 * not be rebuilt every vsync
 *
 * Hydra_Software_Devel/128   3/17/09 12:39p tdo
 * PR45785, PR45789: Fix problem with graphic pitch programming
 *
 * Hydra_Software_Devel/127   3/16/09 10:46p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/5   3/5/09 1:44p tdo
 * PR45785, PR45789: merge from main branch on 3/05/09
 *
 * Hydra_Software_Devel/126   3/5/09 11:52a yuxiaz
 * PR48447: Updated GFD for DCE fix on B2.
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/18/09 3:39p syang
 * PR 45796: fix 7400 compiling
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/14/09 12:40p syang
 * PR 45796:   work-around neg vscl phase problem --- VSCL works now
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/13/09 5:54p syang
 * PR 45796:  added some vscl coeffs, GFD VSCL does show gfx, but with
 * some gabage at top, and some non-linear effect.
 *
 * Hydra_Software_Devel/7420_mcvp/1   2/13/09 10:03a syang
 * PR 45796: first time check in for GFD VSCL support
 *
 * Hydra_Software_Devel/125   12/3/08 4:39p rpan
 * PR45804: Initial effort for the orthogonal VEC support.
 *
 * Hydra_Software_Devel/124   10/27/08 11:25a yuxiaz
 * PR46309: GFD testfeature1 bringup.
 *
 * Hydra_Software_Devel/123   10/23/08 5:50p albertl
 * PR47318, PR47814: Fixed overflow when calculating RGB attenuation
 * offsets.  Rewrote portions of RGB attenuation code and fixed various
 * offset related issues.
 *
 * Hydra_Software_Devel/122   10/19/08 1:35p yuxiaz
 * PR46309: GFD testfeature1 bringup.
 *
 * Hydra_Software_Devel/121   10/9/08 5:28p syang
 * PR 46891: add _isr to the name of func used in _isr context
 *
 * Hydra_Software_Devel/120   10/3/08 11:00a yuxiaz
 * PR46305, PR46307, PR46309: Use new TestFeature1 function from BSUR.
 * Change BVDC_P_SUPPORT_VIDEO_TestFeature1 to
 * BVDC_P_SUPPORT_VIDEO_TESTFEATURE1.
 *
 * Hydra_Software_Devel/119   10/3/08 9:24a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/118   10/1/08 5:15p yuxiaz
 * PR47303, PR46309: Add video TestFeature1 support for GFD on 3548 B0.
 *
 * Hydra_Software_Devel/117   5/8/08 12:28p syang
 * PR 39206: small clean up in GetHwUsingSurs_isr
 *
 * Hydra_Software_Devel/116   5/8/08 12:23p syang
 * PR 39206: remove alpha surface for 3548 and newer chips
 *
 * Hydra_Software_Devel/115   4/17/08 4:36p yuxiaz
 * PR37121: Remove workaround of MTH overflow for CbAlpha and CrAlpha.
 *
 * Hydra_Software_Devel/114   4/11/08 2:31p pntruong
 * PR38929: Really silenced the warnings now.
 *
 * Hydra_Software_Devel/113   4/11/08 2:30p pntruong
 * PR38929: Silenced compiler.  Need to support no alpha surface.
 *
 * Hydra_Software_Devel/112   2/12/08 8:18p pntruong
 * PR38929: Make more files compile.
 *
 * Hydra_Software_Devel/111   1/3/08 6:30p syang
 * PR 38492: avoid pGfxTmp->pulRulAddrIndex to be NULL
 *
 * Hydra_Software_Devel/110   12/14/07 11:23a tdo
 * PR36898: Remove build warning
 *
 * Hydra_Software_Devel/109   12/14/07 11:16a tdo
 * PR36898: Bringup 7335
 *
 * Hydra_Software_Devel/108   11/27/07 11:25a pntruong
 * PR35411: Need to honor user matrix.
 *
 * Hydra_Software_Devel/107   11/20/07 11:06p tdo
 * PR36889: Remove strictly alias warnings
 *
 * Hydra_Software_Devel/106   11/12/07 5:20p yuxiaz
 * PR34523: 7325 bring up.
 *
 * Hydra_Software_Devel/105   10/30/07 7:34p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/105   10/30/07 7:18p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/104   10/11/07 6:03p syang
 * PR 35032: coverity fix
 *
 * Hydra_Software_Devel/103   10/11/07 2:07p albertl
 * PR35135:  Fixed user csc macros and added 10-bit/8-bit video data
 * conversion.
 *
 * Hydra_Software_Devel/102   10/8/07 2:40p pntruong
 * PR34855: Initial VDC bringup.
 *
 * Hydra_Software_Devel/101   10/3/07 11:55a pntruong
 * PR35411: Extend support for user specify gfd csc.
 *
 * Hydra_Software_Devel/100   9/27/07 2:01p yuxiaz
 * PR35013, PR34523: Add new CSC support on 7325.
 *
 * Hydra_Software_Devel/99   9/20/07 1:17a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/98   1/24/07 9:06p albertl
 * PR22237:  Updated BMEM calls to use new BMEM_Heap functions.
 *
 * Hydra_Software_Devel/97   12/18/06 11:26p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/3   12/15/06 6:18p albertl
 * PR24587:  Rewrote window offset and index calculations.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   12/12/06 2:27a albertl
 * PR24587:  Added support for multiple graphics feeders and windows on
 * same compositor.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 12:55p pntruong
 * PR23222, PR23225:  Defer window and source heap assignments until user
 * create.
 *
 * Hydra_Software_Devel/96   8/10/06 9:57a syang
 * PR 22567: added support for 3563
 *
 * Hydra_Software_Devel/95   6/30/06 5:28p syang
 * PR 22413: added pointsample support for gfx horizontal scaling to
 * 7401B0, 7400B0, and 7118
 *
 * Hydra_Software_Devel/94   6/28/06 4:26p syang
 * PR 22208: use non-cashed RUL addr to write gfx sur start addr again (it
 * was accidently changed to cashed addr)
 *
 * Hydra_Software_Devel/93   5/31/06 12:51p syang
 * PR 21846: fix compiling warning caused by gcc option "-O3"
 *
 * Hydra_Software_Devel/92   5/26/06 3:40p pntruong
 * PR20642: Refactored handling of hList.
 *
 * Hydra_Software_Devel/91   4/18/06 11:48a tdo
 * PR 21012: remove compiler warnings
 *
 * Hydra_Software_Devel/90   4/4/06 1:48p hongtaoz
 * PR20403: add triple displays for 7400;
 *
 * Hydra_Software_Devel/89   3/27/06 5:20p syang
 * PR 20331: use BSUR_Surface_GetOffset in order to support multi-heap
 *
 * Hydra_Software_Devel/88   3/24/06 4:48p syang
 * PR 20381: not use mem flush anymore, now use noncached addr for RUL
 * override (due to sur set)
 *
 * Hydra_Software_Devel/87   3/24/06 3:32p syang
 * PR 20381: temp fix, still flushMem, but now use the right memHandle of
 * RUL.
 *
 * Hydra_Software_Devel/86   3/7/06 4:14p syang
 * PR 19670: added 7438 support
 *
 * Hydra_Software_Devel/85   12/21/05 1:33p syang
 * PR 15191: upadated gfd color matrix according to main video win's color
 * matrix
 *
 * Hydra_Software_Devel/84   11/23/05 5:28p hongtaoz
 * PR18180: added 7401 support for 2 CMP's;
 *
 * Hydra_Software_Devel/83   11/10/05 3:21p syang
 * PR 12244: move alpha-only pixel constant color setting from BSUR to
 * BVDC
 *
 * Hydra_Software_Devel/82   10/5/05 6:43p syang
 * PR 16391: avoid to mess-up RUL when gfx sur is set with gfx win muted
 * or with src disconnected from win.
 *
 * Hydra_Software_Devel/81   9/9/05 3:21p syang
 * PR 16943:  stop using private macro BPXL_P_CMP_SIZE
 *
 * Hydra_Software_Devel/80   8/1/05 5:34p syang
 * PR 16163: updated 7401 gfx horizontal scale coff in gfx feeder
 *
 * Hydra_Software_Devel/79   7/21/05 3:46p hongtaoz
 * PR16218, PR15630: removed gfd scratch register write from gfd's RUL;
 *
 * Hydra_Software_Devel/78   5/19/05 4:17p syang
 * PR 15500: change gfx filter coeff default value to bilinear
 *
 * Hydra_Software_Devel/77   5/11/05 7:08p syang
 * PR 15202: change to only override the curr field, but increase
 * sur_rec_nodes num to 6, added memory flush after override RUL for
 * sur_start. No more error message is seen so far.
 *
 * Hydra_Software_Devel/76   5/5/05 6:06p syang
 * PR 15202: avoid to use wrong  RUL addr to write HW sur start
 *
 * Hydra_Software_Devel/75   5/5/05 3:56p syang
 * PR 15202: re-organize for better readability and debugability
 *
 * Hydra_Software_Devel/74   4/27/05 2:46p syang
 * PR 14722: re-organize gfx-win dst cut and support sub-pixel clip (due
 * to dst cut)
 *
 * Hydra_Software_Devel/73   4/22/05 11:24a syang
 * PR 14692: xfer dst cut to src clip for more cases to avoid bandwidth
 * problem
 *
 * Hydra_Software_Devel/72   3/22/05 12:40p pntruong
 * PR14494: Fixed initialization of register offset for gfd_1.  The init
 * function zero out everything that was set in the create.
 *
 * Hydra_Software_Devel/71   3/17/05 6:38p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/70   3/17/05 5:45p syang
 * PR 14087: fix a compile warning with release build
 *
 * Hydra_Software_Devel/69   3/11/05 3:14p syang
 * PR 13282: added more error message to detect value 0 of src_start
 * reading
 *
 * Hydra_Software_Devel/68   3/4/05 4:53p syang
 * PR 13282: work-around when hw start addr reading returns 0
 *
 * Hydra_Software_Devel/67   3/4/05 3:06p syang
 * PR 13282: init pNextFrame to NULL before call callback_func; and  add
 * some msg
 *
 ***************************************************************************/
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_gfxfeeder_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_priv.h"
#include "bchp.h"
#include "bchp_gfd_0.h"
#include "bchp_cmp_0.h"
#include "bchp_bmisc.h"

#if (BVDC_P_MAX_GFX_WINDOWS >= 2)
#include "bchp_gfd_1.h"
#endif
#if (BVDC_P_MAX_GFX_WINDOWS >= 3)
#include "bchp_gfd_2.h"
#endif
#if (BVDC_P_MAX_GFX_WINDOWS >= 4)
#include "bchp_gfd_3.h"
#endif
#if (BVDC_P_MAX_GFX_WINDOWS >= 5)
#include "bchp_gfd_4.h"
#endif
#if (BVDC_P_MAX_GFX_WINDOWS >= 6)
#include "bchp_gfd_5.h"
#endif
#if (BVDC_P_MAX_GFX_WINDOWS >= 7)
#include "bchp_gfd_6.h"
#endif


BDBG_MODULE(BVDC_GFX);
BDBG_OBJECT_ID(BVDC_GFX);


#define BVDC_P_GFD_MSG_ON              0
#if (BVDC_P_GFD_MSG_ON==1)
#define BDBG_P_GFD_MSG    BDBG_ERR
#else
#define BDBG_P_GFD_MSG    BDBG_MSG
#endif

#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
#define BVDC_P_GFD_VSCL_LINE_BUFFER         (1280)
#endif

#define GFX_ALPHA_FULL   255
#define BPXL_eInvalid    ((BPXL_Format) 0)

#define BVDC_P_GFXFD_RETURN_IF_ERR(result) \
	if ( BERR_SUCCESS != (result)) \
	{\
		return BERR_TRACE(result);  \
	}

#define BVDC_P_GFXFD_END_IF_ERR(success, result, label) \
	if ( false == (success) ) \
	{\
		eResult = BERR_TRACE(result);  \
		goto (label);  \
	}

#define BVDC_P_GFX_RDC_OP_FIXED             0

#define BVDC_P_GFX_WIN_MAX_EDGE             (2047)

#define  GFD_NUM_BITS_PER_BYTE              8
#define  GFD_NUM_BITS_FIR_STEP_FRAC         17
#define  GFD_MASK_FIR_STEP_LOW              ((1<<(GFD_NUM_BITS_FIR_STEP_FRAC+1))-1)
#define  GFD_SHIFT_FIR_STEP_INT             (GFD_NUM_BITS_FIR_STEP_FRAC+1)
#define  GFD_MASK_FIR_STEP_INT              ((7)<<GFD_SHIFT_FIR_STEP_INT)
#define  GFD_MAX_FIR_STEP                   (GFD_MASK_FIR_STEP_INT | GFD_MASK_FIR_STEP_LOW)
#define  GFD_NUM_BITS_FIR_INIT_PHASE_FRAC   7
#define  GFD_MASK_FIR_INIT_PHASE_FRAC       ((1<<GFD_NUM_BITS_FIR_INIT_PHASE_FRAC)-1)
#define  GFD_HSCL_FIR_STEP_1                (1<<GFD_NUM_BITS_FIR_STEP_FRAC)
#define  GFD_HSCL_FIR_PHASE_1               (1<<GFD_NUM_BITS_FIR_INIT_PHASE_FRAC)

#define  GFD_VSCL_NUM_BITS_FIR_STEP_FRAC    17
#define  GFD_VSCL_MASK_FIR_STEP_FRAC        ((1<<GFD_VSCL_NUM_BITS_FIR_STEP_FRAC)-1)
#define  GFD_VSCL_NUM_BITS_FIR_INIT_PHASE_FRAC   6
#define  GFD_VSCL_MASK_FIR_INIT_PHASE_FRAC  ((1<<GFD_NUM_BITS_FIR_INIT_PHASE_FRAC)-1)
#define  GFD_VSCL_FIR_STEP_1                (1<<GFD_VSCL_NUM_BITS_FIR_STEP_FRAC)
#define  GFD_VSCL_FIR_PHASE_1               (1<<GFD_VSCL_NUM_BITS_FIR_INIT_PHASE_FRAC)

#define  GFD_VSCL_MAX_BLK_AVG_SIZE          2
#define  GFD_VSCL_BLK_AVG_THD               (4<<GFD_VSCL_NUM_BITS_FIR_STEP_FRAC)

#define  GFD_VSCL_FIR_STEP_TO_PHASE(s)      \
   ((s)>>(GFD_VSCL_NUM_BITS_FIR_STEP_FRAC - GFD_VSCL_NUM_BITS_FIR_INIT_PHASE_FRAC))

#define  GFD_NUM_REGS_ENABLE                1
#define  GFD_NUM_REGS_LOAD_PALETTE          1
#define  GFD_NUM_REGS_CTRL                  1
#define  GFD_NUM_REGS_PIXEL_FORMAT          2
#define  GFD_NUM_REGS_W_ALPHA               1
#define  GFD_NUM_REGS_KEY                   4
#define  GFD_NUM_REGS_ALPHA                 2
#define  GFD_NUM_REGS_PALETTE               2
#define  GFD_NUM_REGS_DISP                  1
#define  GFD_NUM_REGS_COLOR_MATRIX          9
#if (BVDC_P_SUPPORT_GFD_VER>=BVDC_P_SUPPORT_GFD_VER_4)
#define  GFD_NUM_REGS_SRC                   5
#else
#define  GFD_NUM_REGS_SRC                   4
#endif

#define  GFD_NUM_REGS_HSCL_COEFF            4
#define  GFD_NUM_REGS_VSCL_COEFF            2


/***************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_Create
 *
 * Called by BVDC_P_Source_Create to create gfx feeder specific context
 * when BVDC_Handle is openned with BVDC_Open
 *
 * Note: assume parameter eSourceId are valid for gfx feeder
 */
BERR_Code BVDC_P_GfxFeeder_Create
	( BVDC_P_GfxFeeder_Handle         *phGfxFeeder,
	  BREG_Handle                      hRegister,
	  BRDC_Handle                      hRdc,
	  BAVC_SourceId                    eGfdId,
	  bool                             b3dSrc )
{
	BVDC_P_GfxFeederContext *pGfxFeeder = NULL;
#ifdef BCHP_GFD_0_HW_CONFIGURATION
#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
	uint32_t  ulHwCfg;
#endif
#endif

	/* Use: to see messages */
	/*BDBG_SetModuleLevel("BVDC_GFX", BDBG_eMsg);*/

	BDBG_ENTER(BVDC_P_GfxFeeder_Create);

	BDBG_ASSERT( NULL != phGfxFeeder );
	BDBG_ASSERT( NULL != hRegister );

	/* The handle will be NULL if create fails */
	if ( NULL == phGfxFeeder )
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	*phGfxFeeder = NULL;

	/* Alloc and clear the gfxFeeder main context */
	pGfxFeeder = (BVDC_P_GfxFeederContext*)(
		BKNI_Malloc(sizeof(BVDC_P_GfxFeederContext)));
	if ( NULL == pGfxFeeder )
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BKNI_Memset((void*)pGfxFeeder, 0x0, sizeof(BVDC_P_GfxFeederContext));
	BDBG_OBJECT_SET(pGfxFeeder, BVDC_GFX);

	/* initialize stuff that will never change */
	pGfxFeeder->hRegister = hRegister;
	pGfxFeeder->eId = eGfdId;
	pGfxFeeder->hRdc = hRdc;

#if BVDC_P_SUPPORT_NEW_SW_INIT
	pGfxFeeder->ulResetRegAddr = BCHP_BMISC_SW_INIT;
	pGfxFeeder->ulResetMask    =
		BCHP_BMISC_SW_INIT_GFD_0_MASK << (eGfdId - BAVC_SourceId_eGfx0);
#else
	pGfxFeeder->ulResetRegAddr = BCHP_BMISC_SOFT_RESET;
	pGfxFeeder->ulResetMask    =
		BCHP_BMISC_SOFT_RESET_GFD_0_MASK << (eGfdId - BAVC_SourceId_eGfx0);
#endif

#if (BVDC_P_MAX_GFX_WINDOWS >= 2)
	/* register offset into which gfxfeeder */
	pGfxFeeder->ulRegOffset =
#if (BVDC_P_MAX_GFX_WINDOWS >= 3)
		(BAVC_SourceId_eGfx2 == pGfxFeeder->eId)?
		(BCHP_GFD_2_REVISION - BCHP_GFD_0_REVISION) :
#endif
#if (BVDC_P_MAX_GFX_WINDOWS >= 4)
		(BAVC_SourceId_eGfx3 == pGfxFeeder->eId)?
		(BCHP_GFD_3_REVISION - BCHP_GFD_0_REVISION) :
#endif
#if (BVDC_P_MAX_GFX_WINDOWS >= 5)
		(BAVC_SourceId_eGfx4 == pGfxFeeder->eId)?
		(BCHP_GFD_4_REVISION - BCHP_GFD_0_REVISION) :
#endif
#if (BVDC_P_MAX_GFX_WINDOWS >= 6)
		(BAVC_SourceId_eGfx5 == pGfxFeeder->eId)?
		(BCHP_GFD_5_REVISION - BCHP_GFD_0_REVISION) :
#endif
#if (BVDC_P_MAX_GFX_WINDOWS >= 7)
		(BAVC_SourceId_eGfx6 == pGfxFeeder->eId)?
		(BCHP_GFD_6_REVISION - BCHP_GFD_0_REVISION) :
#endif
		((BAVC_SourceId_eGfx1 == pGfxFeeder->eId)?
		 (BCHP_GFD_1_REVISION - BCHP_GFD_0_REVISION) : 0);
#else
	pGfxFeeder->ulRegOffset = 0;
#endif

	/* HW feature related to vertical scaling */
	pGfxFeeder->ulVertLineBuf = 0;
	pGfxFeeder->bSupportVertScl = false;
#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
#ifdef BCHP_GFD_0_HW_CONFIGURATION
	ulHwCfg = BREG_Read32(pGfxFeeder->hRegister,
		BCHP_GFD_0_HW_CONFIGURATION + pGfxFeeder->ulRegOffset);
	pGfxFeeder->bSupportVertScl = BVDC_P_GET_FIELD(
		ulHwCfg, GFD_0_HW_CONFIGURATION, VSCL);
#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_5)
	pGfxFeeder->ulVertLineBuf =
		BVDC_P_GET_FIELD(ulHwCfg, GFD_0_HW_CONFIGURATION, VSCL_LSBF_SIZE);
#else
	pGfxFeeder->ulVertLineBuf =
		pGfxFeeder->bSupportVertScl? BVDC_P_GFD_VSCL_LINE_BUFFER : 0;
#endif
#else
	pGfxFeeder->bSupportVertScl = (pGfxFeeder->eId == BAVC_SourceId_eGfx0);
	pGfxFeeder->ulVertLineBuf =
		pGfxFeeder->bSupportVertScl? BVDC_P_GFD_VSCL_LINE_BUFFER : 0;
#endif
#endif /* (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3) */

	/* init stuff in surface manager */
	pGfxFeeder->stGfxSurface.hRegister = hRegister;
	pGfxFeeder->stGfxSurface.eSrcId = eGfdId;
	pGfxFeeder->stGfxSurface.ulHwMainSurAddrReg =
		BCHP_GFD_0_SRC_START + pGfxFeeder->ulRegOffset;

	/* allocate shadow registers for surface manager */
	BVDC_P_GfxSurface_AllocShadowRegs(
		&pGfxFeeder->stGfxSurface, pGfxFeeder->hRdc, b3dSrc);

	*phGfxFeeder = (BVDC_P_GfxFeeder_Handle) pGfxFeeder;

	BDBG_LEAVE(BVDC_P_GfxFeeder_Create);
	return BERR_SUCCESS;
}

/*************************************************************************
 * {private}
 *
 * Called by BVDC_P_Source_Destroy to destroy gfx feeder specific context
 * when BVDC_Handle is closed with BVDC_Close
 */
BERR_Code BVDC_P_GfxFeeder_Destroy
	( BVDC_P_GfxFeeder_Handle          hGfxFeeder )
{
	BDBG_ENTER(BVDC_P_GfxFeeder_Destroy);
	BDBG_OBJECT_ASSERT(hGfxFeeder, BVDC_GFX);

	/* free surface address shadow registers back to scratch pool */
	BVDC_P_GfxSurface_FreeShadowRegs(
		&hGfxFeeder->stGfxSurface, hGfxFeeder->hRdc);

	BDBG_OBJECT_DESTROY(hGfxFeeder, BVDC_GFX);

	BKNI_Free((void*)hGfxFeeder);

	BDBG_LEAVE(BVDC_P_GfxFeeder_Destroy);

	return BERR_SUCCESS;
}


/*--------------------------------------------------------------------------
 * {private}
 *
 */
static void BVDC_P_GfxFeeder_SetAllDirty(
	BVDC_P_GfxDirtyBits             *pDirty )
{
	int  iGammaTable, iPaletteTable;

	/* every thing need reset at init except tables */
	iGammaTable = pDirty->stBits.bGammaTable;
	iPaletteTable = pDirty->stBits.bPaletteTable;
	BVDC_P_SET_ALL_DIRTY(pDirty);
	pDirty->stBits.bGammaTable = iGammaTable;
	pDirty->stBits.bPaletteTable = iPaletteTable;
}

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_Init
 *
 * initialize stuff that will change after destory and re-create. It also
 * allocate surface address shadow registers. We don't want to allocate them
 * until the GFD is really going to be used.
 */
void BVDC_P_GfxFeeder_Init(
	BVDC_P_GfxFeeder_Handle          hGfxFeeder,
	BMEM_Heap_Handle                 hMemory )
{
	BDBG_ENTER(BVDC_P_GfxFeeder_Init);
	BDBG_OBJECT_ASSERT(hGfxFeeder, BVDC_GFX);

	hGfxFeeder->hMemory = hMemory;

	/* set defaults. */
	hGfxFeeder->pNewSur = NULL;
	BKNI_Memset((void*)&hGfxFeeder->stNewCfgInfo, 0x0, sizeof(BVDC_P_GfxFeederCfgInfo));
	BKNI_Memset((void*)&hGfxFeeder->stCurCfgInfo, 0x0, sizeof(BVDC_P_GfxFeederCfgInfo));
	hGfxFeeder->stNewCfgInfo.stFlags.bEnGfdHwAlphaPreMultiply = 1;
	hGfxFeeder->stNewCfgInfo.eScaleCoeffs = BVDC_FilterCoeffs_eBilinear;

#ifdef BVDC_P_SUPPORT_OLD_SET_ALPHA_SUR
	BKNI_Memset((void*)&hGfxFeeder->stTmpNewAvcPic, 0x0, sizeof(BAVC_Gfx_Picture));
	BKNI_Memset((void*)&hGfxFeeder->stTmpIsrAvcPic, 0x0, sizeof(BAVC_Gfx_Picture));
#endif

	BVDC_P_GfxFeeder_SetAllDirty(&hGfxFeeder->stNewCfgInfo.stDirty);

	/* init GFD HW block when gfd source is created by user */
	hGfxFeeder->ulInitVsyncCntr = BVDC_P_GFX_INIT_CNTR;

	/* init surface manager */
	BVDC_P_GfxSurface_Init(&hGfxFeeder->stGfxSurface);

	BDBG_LEAVE(BVDC_P_GfxFeeder_Init);
	return;
}

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_GetAdjSclOutRect_isr
 *
 * Called by BVDC_P_Window_ApplyChanges to get the adjusted scal-out rect
 * as it SetSurfaceSize in compositor,
 * It should match the design of BVDC_P_GfxFeeder_ValidateSurAndRects
 */
BERR_Code BVDC_P_GfxFeeder_GetAdjSclOutRect_isr
	( const BVDC_P_ClipRect           *pClipRect,            /* in */
	  const BVDC_P_Rect               *pSclOutRect,          /* in */
	  const BVDC_P_Rect               *pDstRect,             /* in */
	  BVDC_P_Rect                     *pAdjSclOutRect )      /* out */
{
	BSTD_UNUSED(pClipRect);
	BSTD_UNUSED(pSclOutRect);

	pAdjSclOutRect->lTop = 0;
	pAdjSclOutRect->lLeft = 0;
	pAdjSclOutRect->ulWidth = pDstRect->ulWidth;
	pAdjSclOutRect->ulHeight = pDstRect->ulHeight;

	return BERR_SUCCESS;
}

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_ValidateSurAndRects_isr
 *
 * Called for isr set surface validation during RUL build
 */
#define BVDC_P_GfxFeeder_ValidateSurAndRects_isr  \
	    BVDC_P_GfxFeeder_ValidateSurAndRects

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_ValidateSurAndRects
 *
 * It validates the combination of clip, scaler-out and dest rectangles
 * and surface size, and records the adjusted clip and output info and
 * scale related intermediate values
 *
 * It could be called by BVDC_P_GfxFeeder_ValidateChanges in user mode,
 * or during RUL build for isr surface.
 *
 * If it is called from ValidateChanges, pCfg is &hGfxFeeder->stNewCfgInfo,
 * therefore change to *pCfg is stored in hGfxFeeder->stNewCfgInfo, it will
 * not affect RUL build before GfxFeeder_ApplyChanges is called, and is ok
 * even if ValidateChanges failed in some other modules.
 *
 * If it is called from BuildRul, we must have a new surface set in _isr
 * mode, as long as we passed the error check at the first part of this
 * func, the new surface will be used. So it is right to change *pCfg
 * (i.e. hGfxFeeder->stCurCfgInfo), so that RUL build is affected right
 * now.
 */
static BERR_Code BVDC_P_GfxFeeder_ValidateSurAndRects
	( BVDC_P_GfxFeeder_Handle          hGfxFeeder,
	  BVDC_P_SurfaceInfo              *pSur, /* new or isr */
	  BVDC_P_GfxFeederCfgInfo         *pCfg, /* new or cur */
	  const BVDC_P_ClipRect           *pClipRect,
	  const BVDC_P_Rect               *pSclOutRect,
	  const BVDC_P_Rect               *pDstRect)
{
	BVDC_P_GfxFeederCfgInfo  *pCurCfg;
	BVDC_P_GfxDirtyBits  stDirty;
	BVDC_P_SurfaceInfo  *pCurSur;
	uint32_t  ulCntWidth, ulOutWidth, ulCntHeight, ulOutHeight;
	uint32_t  ulAdjCntWidth, ulAdjCntLeft, ulAdjCntHeight, ulAdjCntTopInt;
#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
	uint32_t  ulOutOrientLR;
	uint32_t  ulVsclSrcStep, ulBlkAvgSize;
	uint32_t  ulCntTop, ulVsclInitPhase, ulVsclInitPhaseBot;
#endif

	/* Note: since this func could be called during BuildRul, which
	 * would pass CurCfg in, hence pCfg->ulCnt* should not be updated until
	 * all error cheking is completed.
	 * Note: the combination of scale-out with dst rect and canvas is checked
	 * by BVDC_P_Window_ValidateChanges */

	/* check if clut is used by both palette and gamma correction */
	if ( (BPXL_IS_PALETTE_FORMAT(pSur->eInputPxlFmt)) &&
		 (pCfg->stFlags.bEnableGammaCorrection) )
	{
		return BERR_TRACE(BVDC_ERR_GFX_CLUT_REUSE);
	}

	/* check the combination of main surface with clip rectange */
	if ( ((pClipRect->ulLeft + pClipRect->ulRight)  > pSur->ulWidth) ||
		 ((pClipRect->ulTop  + pClipRect->ulBottom) > pSur->ulHeight) )
	{
		return BERR_TRACE(BVDC_ERR_GFX_SRC_OVER_CLIP);
	}

	/* check the combination of clip and scale-out */
	ulCntWidth  = (pSur->ulWidth  - (pClipRect->ulLeft + pClipRect->ulRight));
	ulCntHeight = (pSur->ulHeight - (pClipRect->ulTop  + pClipRect->ulBottom));
	ulOutWidth  = pSclOutRect->ulWidth;
	ulOutHeight = pSclOutRect->ulHeight;
#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
	if(hGfxFeeder->bSupportVertScl)
	{
		if ((0 == ulOutWidth) || (0 == ulCntWidth) ||
			(4 * ulCntWidth < ulOutWidth) || (ulCntWidth > 4 * ulOutWidth))
		{
			return BERR_TRACE(BVDC_ERR_GFX_HSCL_OUT_OF_RANGE);
		}
		if ((0 == ulOutHeight) || (0 == ulCntHeight) ||
			(4 * ulCntHeight < ulOutHeight) || (ulCntHeight > 6 * ulOutHeight))
		{
			return BERR_TRACE(BVDC_ERR_GFX_VSCL_OUT_OF_RANGE);
		}
	}
	else
	{
		if ( ulCntHeight < ulOutHeight )
		{
			return BERR_TRACE(BVDC_ERR_GFX_VERTICAL_SCALE);
		}
		if ( (0 == ulCntWidth) && (0 != ulOutWidth) )
		{
			return BERR_TRACE(BVDC_ERR_GFX_INFINITE_SCALE);
		}
	}
#else
	if ( ulCntHeight < ulOutHeight )
	{
		return BERR_TRACE(BVDC_ERR_GFX_VERTICAL_SCALE);
	}
	if ( (0 == ulCntWidth) && (0 != ulOutWidth) )
	{
		return BERR_TRACE(BVDC_ERR_GFX_INFINITE_SCALE);
	}
#endif /* #if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3) */

#if (BVDC_P_SUPPORT_GFD_VER_0 == BVDC_P_SUPPORT_GFD1_VER)
	if ((BAVC_SourceId_eGfx1 == hGfxFeeder->eId) &&
		((ulOutWidth != ulCntWidth) || (ulOutHeight != ulCntHeight)))
	{
		return BERR_TRACE(BVDC_ERR_GFX_SUR_SIZE_MISMATCH);
	}
#endif

	ulAdjCntWidth = (pDstRect->ulWidth * ulCntWidth + (ulOutWidth - 1)) / ulOutWidth;
#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
	if(hGfxFeeder->bSupportVertScl && (ulCntHeight != ulOutHeight))
	{
		/* we will do vertical scale */
		ulOutOrientLR = (pCfg->eOutOrientation==BFMT_Orientation_e3D_LeftRight)? 1 : 0;
		if (((ulAdjCntWidth     << ulOutOrientLR) > hGfxFeeder->ulVertLineBuf)
			&&
			((pDstRect->ulWidth << ulOutOrientLR) > hGfxFeeder->ulVertLineBuf))
		{
			BDBG_ERR(("GFD line buffer length cannot be larger than %d when VSCL enabled",
					  hGfxFeeder->ulVertLineBuf));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}
#endif

	/* after this point, no more error is possible */

	/* note: dest cut is xfered into src clip to save bandwidth; CntLeft and SrcStep
	 * are fixed point number with precision provided by HW; cntWidth is rounded to
	 * ceiling */
	pCfg->ulHsclSrcStep =
		BVDC_P_MIN(GFD_MAX_FIR_STEP,
			(ulCntWidth << GFD_NUM_BITS_FIR_STEP_FRAC) / ulOutWidth);
	ulAdjCntLeft =
		(pClipRect->ulLeft << GFD_NUM_BITS_FIR_INIT_PHASE_FRAC) +
		(pSclOutRect->lLeft * ulCntWidth << GFD_NUM_BITS_FIR_INIT_PHASE_FRAC) / ulOutWidth;
	/* ulAdjCntWidth = (pDstRect->ulWidth * ulCntWidth + (ulOutWidth - 1)) / ulOutWidth;
	 * would not be outside of original src size */

	/* OutWidth and ulOutHeight are used to set GFD_0_DISP_PIC_SIZE */
	pCfg->ulOutWidth  = pDstRect->ulWidth;

#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
	/* note: dest cut is xfered into src clip to save bandwidth; CntTop and SrcStep
	 * are fixed point number with precision provided by HW; cntHeight is rounded to
	 * ceiling */
	if(hGfxFeeder->bSupportVertScl)
	{
		if (ulCntHeight != ulOutHeight)
		{
			ulVsclSrcStep = (ulCntHeight << GFD_VSCL_NUM_BITS_FIR_STEP_FRAC) / ulOutHeight;
			ulCntTop =
				(pClipRect->ulTop << GFD_VSCL_NUM_BITS_FIR_INIT_PHASE_FRAC) +
				(pSclOutRect->lTop * ulCntHeight << GFD_VSCL_NUM_BITS_FIR_INIT_PHASE_FRAC) / ulOutHeight;
			ulAdjCntHeight = (pDstRect->ulHeight * ulCntHeight + ulOutHeight - 1) / ulOutHeight;
			BDBG_P_GFD_MSG(("vld ajust ulCntTop = 0x%x and %d/64 height = %d",
							ulCntTop>>6, ulCntTop & 63, ulAdjCntHeight));
			/* would not be outside of orig src size */
		}
		else /* VSCL is turned off */
		{
			ulVsclSrcStep = GFD_VSCL_FIR_STEP_1;
			ulCntTop = (pClipRect->ulTop + pSclOutRect->lTop) <<
				GFD_VSCL_NUM_BITS_FIR_INIT_PHASE_FRAC;
			ulAdjCntHeight = ulCntHeight;
		}

		/* vertical init phase for frame/top-filed and bottom-field
		 * note: vertical phase can be decided according to surface width, height,
		 * clip-rect, out size, but horizontal init phase also depends on the
		 * surface pixel format */
		ulAdjCntTopInt = ulCntTop >> GFD_VSCL_NUM_BITS_FIR_INIT_PHASE_FRAC;
		ulVsclInitPhase = ulCntTop & (GFD_VSCL_FIR_PHASE_1 - 1);
		BDBG_P_GFD_MSG(("orig InitPhase 0x%x and %d/64", ulVsclInitPhase >> 6, ulVsclInitPhase & 63));
		ulVsclInitPhase += GFD_VSCL_FIR_STEP_TO_PHASE(ulVsclSrcStep >> 1);
		BDBG_P_GFD_MSG(("after +srcStep/2, InitPhase 0x%x and %d/64", ulVsclInitPhase >> 6, ulVsclInitPhase & 63));
		ulVsclInitPhase = (ulVsclInitPhase > (GFD_VSCL_FIR_PHASE_1 >> 1))?
			(ulVsclInitPhase - (GFD_VSCL_FIR_PHASE_1 >> 1)) : 0;
		BDBG_P_GFD_MSG(("after -1/2, InitPhase 0x%x and %d/64", ulVsclInitPhase >> 6, ulVsclInitPhase & 63));
		ulVsclInitPhaseBot = ulVsclInitPhase + GFD_VSCL_FIR_STEP_TO_PHASE(ulVsclSrcStep);

		/* block average for vscl is needed if srcStep is bigger than vscl tap number */
		ulBlkAvgSize = 0;
		if (ulVsclSrcStep > GFD_VSCL_BLK_AVG_THD)
		{
			while((ulBlkAvgSize < GFD_VSCL_MAX_BLK_AVG_SIZE) &&
				  (ulVsclSrcStep > GFD_VSCL_BLK_AVG_THD * (ulBlkAvgSize + 1)))
			{
				++ulBlkAvgSize;
			}
			ulVsclSrcStep /= (ulBlkAvgSize + 1);
			ulVsclInitPhase /= (ulBlkAvgSize + 1);
			ulVsclInitPhaseBot /= (ulBlkAvgSize + 1);
		}
		pCfg->ulVsclBlkAvgSize = ulBlkAvgSize;
		pCfg->ulVsclSrcStep = ulVsclSrcStep;
		pCfg->ulVsclInitPhase = ulVsclInitPhase;
		pCfg->ulVsclInitPhaseBot = ulVsclInitPhaseBot;
	}
	else
	{
		pCfg->ulVsclBlkAvgSize = 0;
		pCfg->ulVsclSrcStep = GFD_VSCL_FIR_STEP_1;
		ulAdjCntTopInt = pClipRect->ulTop + pSclOutRect->lTop;
		ulAdjCntHeight = pDstRect->ulHeight; /* not used */
	}
#else
	pCfg->ulVsclBlkAvgSize = 0;
	pCfg->ulVsclSrcStep = GFD_VSCL_FIR_STEP_1;
	ulAdjCntTopInt = pClipRect->ulTop + pSclOutRect->lTop;
	ulAdjCntHeight = pDstRect->ulHeight; /* not used */
#endif /* #if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_3)	 */

	/* OutWidth and ulOutHeight are used to set GFD_0_DISP_PIC_SIZE */
	pCfg->ulOutHeight = pDstRect->ulHeight;

	pCfg->stFlags.bNeedHorizScale =
		(GFD_HSCL_FIR_STEP_1 != pCfg->ulHsclSrcStep) ||
		(BVDC_FilterCoeffs_eSharp == pCfg->eScaleCoeffs);
	pCfg->stFlags.bNeedVertScale =
		(hGfxFeeder->bSupportVertScl &&
		((GFD_VSCL_FIR_STEP_1 != pCfg->ulVsclSrcStep) ||
		 (0 != pCfg->ulVsclBlkAvgSize)));
#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_4)
	pCfg->stFlags.bNeedHorizScale |= (pCfg->eOutOrientation==BFMT_Orientation_e3D_LeftRight);
	pCfg->stFlags.bNeedVertScale  |= (pCfg->eOutOrientation==BFMT_Orientation_e3D_OverUnder);
#endif

	/* set the dirty bits due to combined surface or configure change
	 * note: pCfg could be NewCfg or CurCfg */
	stDirty = pCfg->stDirty;
	pCurCfg = &(hGfxFeeder->stCurCfgInfo);
	pCurSur = &(hGfxFeeder->stGfxSurface.stCurSurInfo);

	if ((pSur->eInputPxlFmt != pCurSur->eInputPxlFmt) ||
		(NULL==pSur->stAvcPic.hAlphaSurface) != (NULL==pCurSur->stAvcPic.hAlphaSurface))
	{
		stDirty.stBits.bPxlFmt = true;
	}

	if (pSur->stAvcPic.eInOrientation != pCurSur->stAvcPic.eInOrientation)
	{
		stDirty.stBits.bOrientation = BVDC_P_DIRTY;
	}

	if ( (ulAdjCntLeft   != pCurCfg->ulCntLeft)  ||
		 (ulAdjCntTopInt != pCurCfg->ulCntTopInt) ||
		 (ulAdjCntWidth  != pCurCfg->ulCntWidth) ||
		 (ulAdjCntHeight != pCurCfg->ulCntHeight) )
	{
		/* note: dest clip is also transfered into src clip */
		stDirty.stBits.bSrcClip   = BVDC_P_DIRTY;
		stDirty.stBits.bClipOrOut = BVDC_P_DIRTY;
	}

	if ( (stDirty.stBits.bPxlFmt) ||
		 (stDirty.stBits.bSrcClip) ||
#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1)
		 (pSur->ulAlphaPitch != pCurSur->ulAlphaPitch) ||
#endif
		 (pSur->ulPitch      != pCurSur->ulPitch))
	{
		stDirty.stBits.bSurOffset = true;
	}

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_GFD)
	/* catch DCX mode switch here before copying New to Cur! */
	/* SW3548-2976 workaround: reset GFD if DCX is siwtched ON->OFF */
	if((!pSur->stTestFeature1.bEnable && pCurSur->stTestFeature1.bEnable))
	{
		BVDC_P_GfxFeeder_SetAllDirty(&stDirty);
		hGfxFeeder->ulInitVsyncCntr = BVDC_P_GFX_INIT_CNTR;
	}
#endif

	/* since pCfg could be pCurCfg, we can not store new cnt rect values
	 * to pCfg until the comparing of those values in pCfg and pCurCfg are
	 * done */
	pCfg->ulCntLeft = ulAdjCntLeft;
	pCfg->ulCntWidth = ulAdjCntWidth;
	pCfg->ulCntTopInt = ulAdjCntTopInt;
	pCfg->ulCntHeight = ulAdjCntHeight;

	pCfg->stDirty = stDirty;

	return BERR_SUCCESS;
}

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_ValidateChanges
 *
 * To be called by BVDC_ApplyChanges, to check whether there is conflict
 * between settings related to gfx feeder.
 *
 */
BERR_Code BVDC_P_GfxFeeder_ValidateChanges
	( BVDC_P_GfxFeeder_Handle          hGfxFeeder,
	  BVDC_Source_PictureCallback_isr  pfPicCallbackFunc )
{
	BVDC_P_SurfaceInfo  *pNewSur;
	BVDC_P_GfxFeederCfgInfo  *pNewCfg, *pCurCfg;
	BVDC_P_GfxCfgFlags stNewFlags, stCurFlags;
	BVDC_P_GfxDirtyBits stNewDirty;
	const BVDC_P_ClipRect  *pNewClip;
	const BVDC_P_Rect  *pNewDst, *pNewSclOut;
	BVDC_BlendFactor  eSrcFactor, eDstFactor;
	uint8_t  ucConstant;
	bool  bInterlaced;
	BVDC_P_Window_Info *pWinNewInfo;
	const BVDC_P_Window_Info *pWinCurInfo;
	BFMT_Orientation  eOrientation;
	BVDC_Window_Handle  hWindow;
	BERR_Code eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_GfxFeeder_ValidateChanges);
	BDBG_OBJECT_ASSERT(hGfxFeeder, BVDC_GFX);

	if (NULL == hGfxFeeder->hWindow)
	{
		/* this gfd src is not used by gfx window yet */
		return BERR_SUCCESS;
	}

	hWindow = hGfxFeeder->hWindow;
	pNewCfg = &(hGfxFeeder->stNewCfgInfo);
	pCurCfg = &(hGfxFeeder->stCurCfgInfo);
	pWinNewInfo = &hWindow->stNewInfo;
	pWinCurInfo = &hWindow->stCurInfo;

	/* whether user set new surface, isr set surface, or current surface
	 * will be used to be validated against configure ? */
	if ( true == hGfxFeeder->stGfxSurface.stNewSurInfo.bChangeSurface )
	{
		pNewSur = &(hGfxFeeder->stGfxSurface.stNewSurInfo);
	}
	else if ( true == hGfxFeeder->stGfxSurface.stIsrSurInfo.bChangeSurface )
	{
		pNewSur = &(hGfxFeeder->stGfxSurface.stIsrSurInfo);
	}
	else /* no new user or isr set to surface, contunue to use cur surface */
	{
		pNewSur = &(hGfxFeeder->stGfxSurface.stCurSurInfo);
	}
	hGfxFeeder->pNewSur = pNewSur;

	/* prepare to check the combination of the three rect and surface size,
	 * note: the combination of scale-out with dst rect and canvas is checked
	 * by BVDC_P_Window_ValidateChanges */
	BVDC_P_Window_GetNewScanType( hWindow, &bInterlaced );
	BVDC_P_Window_GetNewRectangles( hWindow, &pNewClip, &pNewSclOut, &pNewDst );
	BVDC_P_Window_GetNewWindowAlpha(hWindow, &(pNewCfg->ucWindowAlpha) );
	BVDC_P_Window_GetNewDispOrientation(hWindow, &eOrientation);
	pNewCfg->eOutOrientation = eOrientation;

	/* eOrientation conflicts with input pixel format? */
#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_4)
	if ((BPXL_eA0 == pNewSur->eInputPxlFmt ||
	     BPXL_eP0 == pNewSur->eInputPxlFmt) &&
	    (BFMT_Orientation_e3D_OverUnder == eOrientation) &&
		(NULL == pfPicCallbackFunc))
	{
		BDBG_ERR(("A_0 and P_0 support 2D + 3D LR only!"));
		return BERR_TRACE((BERR_INVALID_PARAMETER));
	}
#endif

	/* validate surface size and rectangles, adjust rectangles and do some
	 * scale related computation. The intermediate values are stored in
	 * pNewCfg, and will not affect RUL build until ApplyChanges is called
	 */
	eResult = BVDC_P_GfxFeeder_ValidateSurAndRects(
		hGfxFeeder, pNewSur, pNewCfg, pNewClip, pNewSclOut, pNewDst);
	if ((BERR_SUCCESS != eResult) && (NULL == pfPicCallbackFunc))
	{
		return BERR_TRACE(eResult);
	}

	/* check the combination of window blending factor, gfd color key and
	 * window alpha
	 * Note: equation balance is already checked as setting blending factor,
	 * refer to BVDC_P_GfxFeeder_ValidateBlend for detail
	 * Note: bit filed access is slow if it is not in cpu register */
	stNewFlags = pNewCfg->stFlags;  /* note: ValidateSurAndRects changes Flags */
	BVDC_P_Window_GetNewBlendFactor(
		hWindow, &eSrcFactor, &eDstFactor, &ucConstant );
	if ( (BVDC_BlendFactor_eSrcAlpha         == eSrcFactor) &&
		 (BVDC_BlendFactor_eOneMinusSrcAlpha == eDstFactor) )
	{
		/* case 1): user means that alpha pre-multiply has NOT been
		 * performed in graphics source surface, but src alpha should be
		 * used to blend. We should internally turn on alpha pre-multiply
		 * in gfd HW, and change SrcFactor to use constant 1.0. This is
		 * because the alpha output from color key stage is always
		 * pre-multiplied to pixel color by HW. The blending setting adjust
		 * is done in BVDC_P_GfxFeeder_AdjustBlend */
		stNewFlags.bEnGfdHwAlphaPreMultiply = 1;
		stNewFlags.bConstantBlending = 0;
	}
	else if ( (BVDC_BlendFactor_eConstantAlpha    == eSrcFactor) &&
			  (BVDC_BlendFactor_eOneMinusSrcAlpha == eDstFactor) &&
			  (GFX_ALPHA_FULL                     == ucConstant) )
	{
		/* Case 2): user means that alpha pre-multiply has been performed
		 * in graphics source surface. */
		stNewFlags.bEnGfdHwAlphaPreMultiply = 0;
		stNewFlags.bConstantBlending = 0;
	}
	else
	{
		/* Case 3): User means to ignore src pixel alpha and to use
		 * constant for blending if it is not case 1) or 2). Since
		 * in HW the two-way-choice alpha output from key stage is
		 * always  multiplied to pixel color after the pixel-alpha-pre-
		 * multiply stage, case 3) must have 0xff output from color key.
		 * Therefore we should turn off colorkey INTERNALLY and set key
		 * default aplha to be 0xff. That is OK, because from API level
		 * the pixel alpha meant to be ignored anyway */
		stNewFlags.bEnGfdHwAlphaPreMultiply = 0;
		stNewFlags.bConstantBlending = 1;
	}

	stNewFlags.bInterlaced = (bInterlaced)? 1 : 0;

	pNewCfg->stFlags = stNewFlags;

	/* dirty bits due to configure change only */
	pCurCfg = &(hGfxFeeder->stCurCfgInfo);
	stCurFlags = pCurCfg->stFlags;
	stNewDirty = pNewCfg->stDirty;
	if ( stNewFlags.bInterlaced != stCurFlags.bInterlaced )
	{
		stNewDirty.stBits.bScanType  = BVDC_P_DIRTY;
		stNewDirty.stBits.bOutRect   = BVDC_P_DIRTY;
		stNewDirty.stBits.bClipOrOut = BVDC_P_DIRTY;
	}

	if ( (pNewCfg->ulOutWidth  != pCurCfg->ulOutWidth) ||
		 (pNewCfg->ulOutHeight != pCurCfg->ulOutHeight) )
	{
		stNewDirty.stBits.bOutRect   = BVDC_P_DIRTY;
		stNewDirty.stBits.bClipOrOut = BVDC_P_DIRTY;
	}

	if( pNewCfg->eOutOrientation != pCurCfg->eOutOrientation )
	{
		stNewDirty.stBits.bOrientation = BVDC_P_DIRTY;
	}

	if ( (pNewCfg->ucWindowAlpha       != pCurCfg->ucWindowAlpha) ||
		 (stNewFlags.bConstantBlending != stCurFlags.bConstantBlending) )
	{
		stNewDirty.stBits.bKey = BVDC_P_DIRTY;
	}

	/* set picture adjust dirty bit if picture adjustment values
	 * have changed */
	if((pWinNewInfo->stDirty.stBits.bCscAdjust) ||
	   (pWinNewInfo->sHue                 != pWinCurInfo->sHue           ) ||
	   (pWinNewInfo->sContrast            != pWinCurInfo->sContrast      ) ||
	   (pWinNewInfo->sBrightness          != pWinCurInfo->sBrightness    ) ||
	   (pWinNewInfo->sSaturation          != pWinCurInfo->sSaturation    ) ||
	   (pWinNewInfo->sColorTemp           != pWinCurInfo->sColorTemp     ) ||
	   (pWinNewInfo->lAttenuationR        != pWinCurInfo->lAttenuationR  ) ||
	   (pWinNewInfo->lAttenuationG        != pWinCurInfo->lAttenuationG  ) ||
	   (pWinNewInfo->lAttenuationB        != pWinCurInfo->lAttenuationB  ) ||
	   (pWinNewInfo->lOffsetR             != pWinCurInfo->lOffsetR       ) ||
	   (pWinNewInfo->lOffsetG             != pWinCurInfo->lOffsetG       ) ||
	   (pWinNewInfo->lOffsetB             != pWinCurInfo->lOffsetB       ) ||
	   (pNewCfg->stFlags.bConstantBlending != pCurCfg->stFlags.bConstantBlending) ||
	   (hWindow->hCompositor->hDisplay->stNewInfo.stDirty.stBits.bTiming))
	{
		stNewDirty.stBits.bCsc = BVDC_P_DIRTY;
	}

	/* set dirty bit if coefficient index changed in window */
	if(pWinNewInfo->stDirty.stBits.bCtIndex)
	{
		stNewDirty.stBits.bScaleCoeffs = BVDC_P_DIRTY;
	}

#if BVDC_P_GFX_INIT_WORKAROUND
	if(eOrientation != pCurCfg->eOutOrientation)
	{
		hGfxFeeder->ulInitVsyncCntr = BVDC_P_GFX_INIT_CNTR;
		BVDC_P_GfxFeeder_SetAllDirty (&stNewDirty);
	}
#endif

	pNewCfg->stDirty = stNewDirty;

	BDBG_LEAVE(BVDC_P_GfxFeeder_ValidateChanges);
	return BERR_SUCCESS;
}

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_ApplyChanges_isr
 *
 * To be called by BVDC_ApplyChanges, to copy "new user state" to "current
 * state", after validation of all VDC modules passed.
 *
 */
BERR_Code BVDC_P_GfxFeeder_ApplyChanges_isr
	( BVDC_P_GfxFeeder_Handle     hGfxFeeder )
{
	BVDC_P_GfxFeederCfgInfo  *pNewCfg, *pCurCfg;
	BVDC_P_SurfaceInfo  *pNewSur, *pCurSur;

	BDBG_ENTER(BVDC_P_GfxFeeder_ApplyChanges_isr);
	BDBG_OBJECT_ASSERT(hGfxFeeder, BVDC_GFX);

	if (NULL == hGfxFeeder->hWindow)
	{
		/* this gfd src is not used by gfx window yet */
		return BERR_SUCCESS;
	}

	pNewCfg = &(hGfxFeeder->stNewCfgInfo);
	pCurCfg = &(hGfxFeeder->stCurCfgInfo);
	pCurSur = &(hGfxFeeder->stGfxSurface.stCurSurInfo);
	pNewSur = hGfxFeeder->pNewSur; /* decided by ValidateChanges */

	/* Copy NewCfg to CurCfg if there is diff between cur and new Proc */
	if ( pNewCfg->stDirty.aulInts[0] )
	{
		/* Note: Current and new Dirty must be ORed in case that more
		 *       than one ApplyChange called before RUL is built
		 * Note: After NewCfg is copied to CurCfg, new Dirty should be set to
		 *       0 to indicate that there is no diff between cur and new cfg
		 */
		BVDC_P_OR_ALL_DIRTY(&pNewCfg->stDirty, &pCurCfg->stDirty);
		*pCurCfg = *pNewCfg;

		BVDC_P_CLEAN_ALL_DIRTY(&pNewCfg->stDirty);
	}
	else
	{
		/* might still need to copy Flags, since some of its bits changing does
		 * not set dirty bits */
		pCurCfg->stFlags = pNewCfg->stFlags;
	}

	/* Copy NewSur to curSur if bChangeSurface.
	 * Note: pNewSur could be _isr set Surface,
	 * Note: pNewSur could be NULL before a window is connected to the src */
	if ( NULL != pNewSur && pNewSur->bChangeSurface && pNewSur != pCurSur)
	{
		/* pCurSur->bChangeSurface would stay true so that the surface change
		 * is built into RUL later */
		*pCurSur = *pNewSur;

		/* to avoid future validation if surface no longer changes */
		pNewSur->bChangeSurface  = false;

		/* any previous set IsrSur should no longer be used */
		hGfxFeeder->stGfxSurface.stIsrSurInfo.bChangeSurface = false;
	}

	BDBG_LEAVE(BVDC_P_GfxFeeder_ApplyChanges_isr);
	return 	BERR_SUCCESS;
}

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_AbortChanges
 *
 * Cancel the user settings that either fail to validate or simply
 * because user wish to abort the changes in mid-way.
 */
void BVDC_P_GfxFeeder_AbortChanges
	( BVDC_P_GfxFeeder_Handle     hGfxFeeder )
{
	BVDC_P_GfxFeederCfgInfo  *pNewCfg;

	BDBG_ENTER(BVDC_P_GfxFeeder_AbortChanges);

	/* validate paramters */
	BDBG_OBJECT_ASSERT(hGfxFeeder, BVDC_GFX);

	/* */
	pNewCfg = &(hGfxFeeder->stNewCfgInfo);

	/* copy CurCfg back to NewCfg, including Flags */
	*pNewCfg = hGfxFeeder->stCurCfgInfo;
	BVDC_P_CLEAN_ALL_DIRTY(&(pNewCfg->stDirty));

	/* any user set surface should not be used again */
	hGfxFeeder->stGfxSurface.stNewSurInfo.bChangeSurface = false;

	BDBG_LEAVE(BVDC_P_GfxFeeder_AbortChanges);
	return;
}

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_BuildRulForUserChangeOnly_isr
 *
 * Builds RUL for user configure change
 *
 * designed to be called by BVDC_P_GfxFeeder_BuildRul_isr only, no paramter
 * validation performed here
 */
static BERR_Code BVDC_P_GfxFeeder_BuildRulForUserChangeOnly_isr
	( BVDC_P_GfxFeederCfgInfo       *pCurCfg,
	  uint32_t                     **ppulRulCur,
	  uint32_t                       ulRulOffset )
{
	BERR_Code  eResult = BERR_SUCCESS;
	uint32_t  *pulRulCur;
	uint32_t  ulOutWidth, ulOutHeight;
	uint32_t  ulClutAddr, ulClutSize;
	uint8_t  ucWinAlpha;
	BVDC_P_GfxDirtyBits  stDirty = pCurCfg->stDirty;

	/* init RUL buffer pointers */
	pulRulCur = *ppulRulCur;

	/* set RUL for output size */
	if ( stDirty.stBits.bOutRect )
	{
		ulOutWidth  = pCurCfg->ulOutWidth;
		ulOutHeight = (pCurCfg->stFlags.bInterlaced)?
			pCurCfg->ulOutHeight / 2 :  pCurCfg->ulOutHeight;
		*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_DISP_PIC_SIZE ) + ulRulOffset;
		*pulRulCur++ =
			BCHP_FIELD_DATA( GFD_0_DISP_PIC_SIZE, HSIZE, ulOutWidth ) |
			BCHP_FIELD_DATA( GFD_0_DISP_PIC_SIZE, VSIZE, ulOutHeight);
	}

	/* set RUL for loading gamma correction table */
	if ( stDirty.stBits.bGammaTable )
	{
		ulClutAddr = pCurCfg->ulGammaClutAddress;
		ulClutSize = pCurCfg->ulNumGammaClutEntries;

		/* set the addr and size for table loading */
		*pulRulCur++ = BRDC_OP_IMMS_TO_REGS( GFD_NUM_REGS_PALETTE );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_PALETTE_START ) + ulRulOffset;
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_PALETTE_START, ADDR, ulClutAddr );
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_PALETTE_SIZE,  SIZE, ulClutSize );

		/* triger the table loading */
		*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_LOAD_PALETTE ) + ulRulOffset;
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_LOAD_PALETTE, LOAD_PALETTE, 1);
	}

	/* set RUL for color key */
	if ( stDirty.stBits.bKey )
	{
		ucWinAlpha = (pCurCfg->stFlags.bConstantBlending) ?
			GFX_ALPHA_FULL: pCurCfg->ucWindowAlpha;

		*pulRulCur++ = BRDC_OP_IMMS_TO_REGS( GFD_NUM_REGS_KEY );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_KEY_MAX ) + ulRulOffset;
		*pulRulCur++ = pCurCfg->ulKeyMaxAMNO;
		*pulRulCur++ = pCurCfg->ulKeyMinAMNO;
		*pulRulCur++ = pCurCfg->ulKeyMaskAMNO;
		*pulRulCur++ =
			BCHP_FIELD_DATA( GFD_0_KEY_ALPHA, DEFAULT_ALPHA, ucWinAlpha ) |
			BCHP_FIELD_DATA( GFD_0_KEY_ALPHA, KEY_ALPHA,	 pCurCfg->ucKeyedAlpha );
	}

	/* set constant color for alpha-only pixels */
	if ( stDirty.stBits.bConstantColor )
	{
		*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_WIN_COLOR ) + ulRulOffset;
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_WIN_COLOR, WIN_COLOR, pCurCfg->ulConstantColor );
	}

	/* reset RUL buffer pointer */
	*ppulRulCur = pulRulCur;

	return BERR_TRACE(eResult);
}

/*------------------------------------------------------------------------
 *  {secret}
 *	BVDC_P_GfxFeeder_CalcSurfaceOffset_isr
 *
 *  Called during BuildRul, after validating the combination of surafce
 *  and graphics feeder configure
 */
static BERR_Code BVDC_P_GfxFeeder_CalcSurfaceOffset_isr
	( BVDC_P_GfxFeeder_Handle             hGfxFeeder )
{
	BVDC_P_SurfaceInfo   *pCurSur;
	BVDC_P_GfxFeederCfgInfo  *pCurCfg;
	uint32_t  ulCntLeft, ulFirInitPhase;
	uint32_t  ulBitsPerPixel, ulOffsetBitsInLine, ulOffsetByteInLine;

	BDBG_OBJECT_ASSERT(hGfxFeeder, BVDC_GFX);

	pCurCfg = &(hGfxFeeder->stCurCfgInfo);
	pCurSur = &(hGfxFeeder->stGfxSurface.stCurSurInfo);

	ulFirInitPhase = pCurCfg->ulCntLeft & GFD_MASK_FIR_INIT_PHASE_FRAC;
	ulCntLeft = pCurCfg->ulCntLeft >> GFD_NUM_BITS_FIR_INIT_PHASE_FRAC;
	ulBitsPerPixel = BPXL_BITS_PER_PIXEL(pCurSur->eInputPxlFmt);
	ulOffsetBitsInLine = ulBitsPerPixel * ulCntLeft;
	ulOffsetByteInLine = ulOffsetBitsInLine / GFD_NUM_BITS_PER_BYTE;
	if ( (0 < ulBitsPerPixel) && (ulBitsPerPixel < GFD_NUM_BITS_PER_BYTE) )
	{
		/* ulOffsetPixInByte is the extra number of pixels to skip in after GFD
		 * input for sub-byte format like P_1, P_2, P_4.
		 * note:  use 'if' to avoid the expensive '/', it is typically not needed */
		hGfxFeeder->ulOffsetPixInByte =
			(ulOffsetBitsInLine - ulOffsetByteInLine * GFD_NUM_BITS_PER_BYTE) / ulBitsPerPixel;
	}
	else
	{
		hGfxFeeder->ulOffsetPixInByte = 0;
	}

	if ( BPXL_IS_YCbCr422_FORMAT(pCurSur->eInputPxlFmt) && (ulCntLeft & 0x1) )
	{
		/* need even pixel allignment for Y0PrY1Pb format */
		ulOffsetByteInLine -= 2; /* two byte per YCrCb422 pixel */
		ulFirInitPhase += (1 << GFD_NUM_BITS_FIR_INIT_PHASE_FRAC);
	}

	hGfxFeeder->ulFirInitPhase = ulFirInitPhase;
	hGfxFeeder->stGfxSurface.ulMainByteOffset =
		pCurCfg->ulCntTopInt * pCurSur->ulPitch + ulOffsetByteInLine;

#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1)
	if (pCurSur->stAvcPic.hAlphaSurface)
	{
		ulOffsetByteInLine = ulCntLeft / GFD_NUM_BITS_PER_BYTE;
		hGfxFeeder->ulAlphaOffsetPixInByte =
			(ulCntLeft - ulOffsetByteInLine * GFD_NUM_BITS_PER_BYTE);
		hGfxFeeder->stGfxSurface.ulAlphaByteOffset =
			pCurCfg->ulCntTopInt * pCurSur->ulAlphaPitch + ulOffsetByteInLine;
	}
#endif

	return BERR_SUCCESS;
}

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_BuildRulForSurCtrl_isr
 *
 * Builds RUL for surface related state changes.
 *
 * designed to be called by BVDC_P_GfxFeeder_BuildRul_isr only, no paramter
 * validation performed here
 */
static BERR_Code BVDC_P_GfxFeeder_BuildRulForSurCtrl_isr
	( BVDC_P_GfxFeeder_Handle            hGfxFeeder,
	  BAVC_Polarity                      eFieldId,
	  uint32_t                         **ppulRulCur,
	  uint32_t                           ulRulOffset )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BVDC_P_GfxSurfaceContext  *pGfxSurface;
	BVDC_P_SurfaceInfo   *pCurSur;
	BVDC_P_GfxFeederCfgInfo  *pCurCfg;
	BVDC_P_GfxDirtyBits  stDirty;
	BVDC_P_GfxCfgFlags  stFlags;
	uint32_t  ulBottomOffset;
	bool  bSkipTopLine;
	uint32_t  *pulRulCur;
	uint32_t  ulMainSurPitch;
	uint32_t  ulOffsetPixInByte;
	bool  bChangeClipOrField;
	uint32_t  ulFirStepLow, ulFirStepInt;
	uint32_t  *pulCoeffs;
	uint32_t  ulAddrShadowReg0, ulAddrShadowReg1;
	uint32_t  ulRAddrShadowReg0, ulRAddrShadowReg1;
#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1)
	uint32_t  ulAlphaSurPitch, ulAlphaOffsetPixInByte, ulAddrReg;
#endif
#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
	uint32_t  ulEnDejag, ulEnDering, ulEnDemoMode;
	uint32_t  ulEnVscl, ulFilterOrder;
	uint32_t  ulVsclFirStep;
	uint32_t  *pulVsclCoeffs;
	uint32_t  ulCntHeight;
#endif
#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_4)
	uint32_t  ulAddrReg;
#endif

	BDBG_ENTER(BVDC_P_GfxFeeder_BuildRulForSurCtrl_isr);
	BDBG_OBJECT_ASSERT(hGfxFeeder, BVDC_GFX);

	/* init RUL buffer pointers */
	pulRulCur = *ppulRulCur;

	pGfxSurface = &hGfxFeeder->stGfxSurface;
	pCurCfg = &(hGfxFeeder->stCurCfgInfo);
	pCurSur = &(pGfxSurface->stCurSurInfo);
	stDirty = pCurCfg->stDirty;
	stFlags = pCurCfg->stFlags;
	bChangeClipOrField =
		stDirty.stBits.bSrcClip || stDirty.stBits.bScanType || stFlags.bInterlaced;

	/* (1) set RUL for scale coeffs
	 * note: stDirty.stBits.bClipOrOut will be set if clipRect, out size, or surface size change */
	if ( (stDirty.stBits.bScaleCoeffs | stDirty.stBits.bClipOrOut) && hGfxFeeder->hWindow )
	{
		const BVDC_CoefficientIndex *pCtIndex = &hGfxFeeder->hWindow->stCurInfo.stCtIndex;

		/* set horizontal scale coeffs */
		BVDC_P_GfxFeeder_DecideFilterCoeff_isr(
			pCurCfg->eScaleCoeffs, pCtIndex->ulSclHorzLuma, pCurCfg->ulCntWidth, pCurCfg->ulOutWidth, &pulCoeffs );
		*pulRulCur++ = BRDC_OP_IMMS_TO_REGS( GFD_NUM_REGS_HSCL_COEFF );
		*pulRulCur++ = BRDC_REGISTER(BCHP_GFD_0_HORIZ_FIR_COEFF_PHASE0_00_01) + ulRulOffset;
		BKNI_Memcpy( (void*) pulRulCur, (void*) pulCoeffs, 4 * GFD_NUM_REGS_HSCL_COEFF );
		pulRulCur += GFD_NUM_REGS_HSCL_COEFF;

		/* set vertical scale coeffs */
#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
		if(hGfxFeeder->bSupportVertScl)
		{
			BVDC_P_GfxFeeder_DecideVsclFirCoeff_isr(
				pCurCfg->eScaleCoeffs, pCtIndex->ulSclVertLuma, pCurCfg->ulCntHeight, pCurCfg->ulOutHeight, &pulVsclCoeffs );
			*pulRulCur++ = BRDC_OP_IMMS_TO_REGS( GFD_NUM_REGS_VSCL_COEFF );
			*pulRulCur++ = BRDC_REGISTER(BCHP_GFD_0_VERT_FIR_COEFF_PHASE0_00_01) + ulRulOffset;
			BKNI_Memcpy( (void*) pulRulCur, (void*) pulVsclCoeffs, 4 * GFD_NUM_REGS_VSCL_COEFF );
			pulRulCur += GFD_NUM_REGS_VSCL_COEFF;
		}
#endif
	}

	/* (2) set RUL for other scale configures related to scale factors,
	 * note: stDirty.stBits.bClipOrOut will be set if clipRect, out size, or surface size change
	 */
	if ( stDirty.stBits.bClipOrOut || stDirty.stBits.bDemoMode )
	{
		/* set horizontal scale factor */
		ulFirStepLow = pCurCfg->ulHsclSrcStep & GFD_MASK_FIR_STEP_LOW;
		ulFirStepInt = ((pCurCfg->ulHsclSrcStep & GFD_MASK_FIR_STEP_INT) >>
						GFD_SHIFT_FIR_STEP_INT);
		*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_HORIZ_FIR_SRC_STEP ) + ulRulOffset;
		*pulRulCur++ =
			BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_SRC_STEP,   STEP,     ulFirStepLow ) |
			BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_SRC_STEP,   STEP_INT, ulFirStepInt );

#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
		if(hGfxFeeder->bSupportVertScl)
		{
			/* turn on dering if horizontal scale factor is bigger than 2/3, alpha clip */
			ulEnDering =
				((2 * pCurCfg->ulHsclSrcStep) <= (3 * GFD_HSCL_FIR_STEP_1))? 1 : 0;
			ulEnDemoMode = (pCurCfg->stFlags.bDeringDemoMode)? 1 : 0;
			*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
			*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_DERINGING ) + ulRulOffset;
			*pulRulCur++ =
				BCHP_FIELD_DATA( GFD_0_DERINGING, HORIZ_ALPHA_DERINGING,  ulEnDering ) |
				BCHP_FIELD_DATA( GFD_0_DERINGING, HORIZ_CHROMA_DERINGING, ulEnDering ) |
				BCHP_FIELD_DATA( GFD_0_DERINGING, HORIZ_LUMA_DERINGING,   ulEnDering ) |
				BCHP_FIELD_DATA( GFD_0_DERINGING, DEMO_MODE,              ulEnDemoMode );

			/* set vertical scale factor and block average, and filter order
			 */

			/* pCurCfg->ulVsclSrcStep has been adjusted according to block average */
			ulVsclFirStep = pCurCfg->ulVsclSrcStep;
			*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
			*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_VERT_FIR_SRC_STEP ) + ulRulOffset;
			*pulRulCur++ = (stFlags.bInterlaced)?
				BCHP_FIELD_DATA( GFD_0_VERT_FIR_SRC_STEP, SIZE, 2 * ulVsclFirStep ) :
				BCHP_FIELD_DATA( GFD_0_VERT_FIR_SRC_STEP, SIZE, ulVsclFirStep );

			*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
			*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_SRC_VSIZE ) + ulRulOffset;

			ulCntHeight = (stFlags.bNeedVertScale || !pCurCfg->stFlags.bInterlaced) ?
				pCurCfg->ulCntHeight :  pCurCfg->ulCntHeight / 2;
			*pulRulCur++ =
				BCHP_FIELD_DATA( GFD_0_SRC_VSIZE, VSIZE, ulCntHeight );

			/* turn on dejag if vertical scale factor is bigger than 1*/
			ulEnDejag =
				((pCurCfg->ulVsclSrcStep <= GFD_VSCL_FIR_STEP_1) &&
				 (0 == pCurCfg->ulVsclBlkAvgSize))? 1 : 0;
			ulEnDemoMode = (pCurCfg->stFlags.bDejagDemoMode)? 1 : 0;
			*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
			*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_DEJAGGING ) + ulRulOffset;
			*pulRulCur++ =
				BCHP_FIELD_DATA( GFD_0_DEJAGGING, HORIZ,           0)          |
				BCHP_FIELD_DATA( GFD_0_DEJAGGING, GAIN,            0)          |
				BCHP_FIELD_DATA( GFD_0_DEJAGGING, CORE,            0)          |
				BCHP_FIELD_DATA( GFD_0_DEJAGGING, VERT_DEJAGGING,  ulEnDejag ) |
				BCHP_FIELD_DATA( GFD_0_DEJAGGING, DEMO_MODE,       ulEnDemoMode );

			/* set gfd scale top configure */
			ulFilterOrder = (pCurCfg->ulHsclSrcStep < GFD_HSCL_FIR_STEP_1)?
				BCHP_FIELD_ENUM( GFD_0_GSCL_CFG, FILTER_ORDER, VERT_FIRST ) :
				BCHP_FIELD_ENUM( GFD_0_GSCL_CFG, FILTER_ORDER, HORIZ_FIRST );
			ulEnVscl = (stFlags.bNeedVertScale)? 1: 0;
			*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
			*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_GSCL_CFG ) + ulRulOffset;
			*pulRulCur++ =
				BCHP_FIELD_DATA( GFD_0_GSCL_CFG, BAVG_BLK_SIZE, pCurCfg-> ulVsclBlkAvgSize ) |
				BCHP_FIELD_DATA( GFD_0_GSCL_CFG, HCLIP_ENABLE,  1 )                          |
				BCHP_FIELD_DATA( GFD_0_GSCL_CFG, VCLIP_ENABLE,  1 )                          |
				ulFilterOrder                                                                |
				BCHP_FIELD_DATA( GFD_0_GSCL_CFG, IOBUF_ENABLE,  1 )                          |
				BCHP_FIELD_DATA( GFD_0_GSCL_CFG, VSCL_ENABLE,   ulEnVscl )                   |
				BCHP_FIELD_DATA( GFD_0_GSCL_CFG, GSCL_ENABLE,   1 );
			/* TODO set demo_setting */
		}
#endif
	}

	/* (3) set RUL for main size, pitch, offset and scl initial phase
	 * note: stDirty.stBits.bClipOrOut will be set if clipRect, out size, or surface size
	 * change */
	if ( bChangeClipOrField || stDirty.stBits.bSurOffset )
	{
		if ( stDirty.stBits.bSurOffset )
		{
			BVDC_P_GfxFeeder_CalcSurfaceOffset_isr(hGfxFeeder);

#if (BVDC_P_SUPPORT_GFD_VER<BVDC_P_SUPPORT_GFD_VER_4)
			*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
			*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_HORIZ_FIR_INIT_PHASE ) + ulRulOffset;
			*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_INIT_PHASE, PHASE, hGfxFeeder->ulFirInitPhase );
#else
		    /* 3D support */
			*pulRulCur++ = BRDC_OP_IMMS_TO_REGS(2);
			*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_HORIZ_FIR_INIT_PHASE ) + ulRulOffset;
			*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_INIT_PHASE,   PHASE, hGfxFeeder->ulFirInitPhase );
			*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_HORIZ_FIR_INIT_PHASE_R, PHASE, hGfxFeeder->ulFirInitPhase );
#endif
		}

		/* if ClipRect or Surface changes, more likely some thing in the following
		 * will change, therefore no need to comppare */
		ulOffsetPixInByte = hGfxFeeder->ulOffsetPixInByte;
		ulMainSurPitch  = (stFlags.bNeedVertScale || !stFlags.bInterlaced)?
			pCurSur->ulPitch : 2 * pCurSur->ulPitch;
		*pulRulCur++ = BRDC_OP_IMMS_TO_REGS( GFD_NUM_REGS_SRC );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_SRC_OFFSET ) + ulRulOffset;
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_SRC_OFFSET, BLANK_PIXEL_COUNT, ulOffsetPixInByte );
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_SRC_HSIZE,  HSIZE, pCurCfg->ulCntWidth );
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_SRC_START,  ADDR,  0 ); /* set later */

#if (BVDC_P_SUPPORT_GFD_VER>=BVDC_P_SUPPORT_GFD_VER_4)
		/* 3D support */
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_SRC_START_R, ADDR, 0);
#endif
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_SRC_PITCH,  PITCH, ulMainSurPitch );

		/* vertical init phase will be diff for top and bot field, even if surface
		 * and clip-rect and output rect has no change */
#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_3)
		if(hGfxFeeder->bSupportVertScl)
		{
			/* set vertical scl init phase  */
			*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
			*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_VERT_FIR_INIT_PHASE ) + ulRulOffset;
			*pulRulCur++ = (BAVC_Polarity_eBotField == eFieldId)?
				BCHP_FIELD_DATA( GFD_0_VERT_FIR_INIT_PHASE, PHASE, pCurCfg->ulVsclInitPhaseBot ) :
				BCHP_FIELD_DATA( GFD_0_VERT_FIR_INIT_PHASE, PHASE, pCurCfg->ulVsclInitPhase );
		}
#endif
	}

	/* (4) set RUL for test feaure */
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_GFD)
	if ( stDirty.stBits.bSurface )
	{
		uint32_t   ulPixelPerGroup, ulBitsPerGroup;
		BSUR_TestFeature1_Settings  *pTestFeature1;

		pTestFeature1 = &pCurSur->stTestFeature1;
		ulPixelPerGroup = 4;
		ulBitsPerGroup = (stTestFeature1.ulBitsPerPixel *
			ulPixelPerGroup) >> BVDC_P_TestFeature1_FRACTIONAL_SHIFT;

		/*  GFD_0_DCD_PRED_CFG */
		*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_DCD_PRED_CFG + ulRulOffset );
		*pulRulCur++ = (
			BCHP_FIELD_DATA(GFD_0_DCD_PRED_CFG, ENABLE,          pTestFeature1->bEnable) |
			BCHP_FIELD_ENUM(GFD_0_DCD_PRED_CFG, CONVERT_RGB,     Disable      ) |
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_VER < 2)
			BCHP_FIELD_ENUM(GFD_0_DCD_PRED_CFG, PREDICTION_MODE, Normal_Mode  ) |
#else
			BCHP_FIELD_DATA(GFD_0_DCD_PRED_CFG, PREDICTION_MODE, pTestFeature1->ulPredictionMode) |
#endif
			BCHP_FIELD_ENUM(GFD_0_DCD_PRED_CFG, EDGE_PRED_ENA,   Disable      ) |
			BCHP_FIELD_ENUM(GFD_0_DCD_PRED_CFG, LEFT_PRED_ENA,   Disable      ) |
			BCHP_FIELD_ENUM(GFD_0_DCD_PRED_CFG, ABCD_PRED_ENA,   Disable      ) |
			BCHP_FIELD_ENUM(GFD_0_DCD_PRED_CFG, LS_PRED_ENA,     Enable       ));

		/* GFD_0_DCD_COMPR_CFG1 */
		*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_DCD_COMPR_CFG1 + ulRulOffset );
		*pulRulCur++ = (
			BCHP_FIELD_DATA(GFD_0_DCD_COMPR_CFG1, PIXELS_PER_GROUP,  ulPixelPerGroup) |
			BCHP_FIELD_DATA(GFD_0_DCD_COMPR_CFG1, TGT_OFFSET_HI,     0xfa   ) |
			BCHP_FIELD_DATA(GFD_0_DCD_COMPR_CFG1, TGT_OFFSET_LO,     12     ) |
			BCHP_FIELD_DATA(GFD_0_DCD_COMPR_CFG1, TGT_BPG, ulBitsPerGroup));

		/* GFD_0_DCD_VIDEO_CFG */
		*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_DCD_VIDEO_CFG + ulRulOffset );
		*pulRulCur++ = (
			BCHP_FIELD_ENUM(GFD_0_DCD_VIDEO_CFG, COMP10BIT,    Eight_bit) |
			BCHP_FIELD_ENUM(GFD_0_DCD_VIDEO_CFG, VIDEO_FORMAT, Fmt_4444 ));
	}
#endif

	/* ---------------------------------------------------------------------
	 * (5) send new surface to address shadow registers.
	 * note: we only want to call SetShadowRegs_isr once for this surface setting
	 * even if this RUL get lost, . So we test pCurSur->bChangeSurface, rather than
	 * stDirty.stBits.bSurface*/
	if (pCurSur->bChangeSurface || stDirty.stBits.bSurOffset)
	{
		BVDC_P_GfxSurface_SetShadowRegs_isr(&hGfxFeeder->stGfxSurface, pCurSur);
	}

	/* ---------------------------------------------------------------------
	 * (6) setup RUL to pick one set of surface address shadow registers.
	 *
	 * After this RUL is built, gfx surface might change with SetSurface, we
	 * want the change showing up as soon as possible. SetSurface will put the
	 * surface address to a shadow register, and here we pre-build the RUL to
	 * copy surface address value from shadow register to GFD.
	 *
	 * In order to activate left and right surface atomically, we use an index
	 * register, and ping-pong buffered left addr and right addr register
	 * pairs.  The index indicates the left/right surface addr in which pair
	 * should be used. The following is the algorithm:
	 *
	 * v_0  <-  index
	 * v_1  <-  ~index
	 *
	 * v_2 = v_0 & left_start_1
	 * v_3 = v_1 & left_start_0
	 * v_2 = v_2 | v_3
	 * v_3 <- ulBottomOffset
	 * BCHP_GFD_0_SRC_START_ADDR = v_2 + v3
	 *
	 * v_2 = v_0 & right_start_0
	 * v_3 = v_1 & right_start_1
	 * v_2 = v_2 | v_3
	 * v_3 <-  ulBottomOffset
	 * BCHP_GFD_0_SRC_START_R_ADDR = v_2 + v3
	 */
	bSkipTopLine = (eFieldId == BAVC_Polarity_eBotField) && (!stFlags.bNeedVertScale);
	ulBottomOffset = (bSkipTopLine)? pCurSur->ulPitch : 0;
	if (pGfxSurface->b3dSrc)
	{
		if (BVDC_P_ORIENTATION_IS_3D(pCurSur->stAvcPic.eInOrientation) &&
			BFMT_Orientation_e3D_Right == pCurCfg->eOutOrientation)
		{
			/* sawp right Surface and left surface due to output orientation */
			ulAddrShadowReg0  = BRDC_REGISTER(pGfxSurface->ulRSurAddrReg[0]);
			ulAddrShadowReg1  = BRDC_REGISTER(pGfxSurface->ulRSurAddrReg[1]);
			ulRAddrShadowReg0 = BRDC_REGISTER(pGfxSurface->ulSurAddrReg[0]);
			ulRAddrShadowReg1 = BRDC_REGISTER(pGfxSurface->ulSurAddrReg[1]);
		}
		else
		{
			ulAddrShadowReg0  = BRDC_REGISTER(pGfxSurface->ulSurAddrReg[0]);
			ulAddrShadowReg1  = BRDC_REGISTER(pGfxSurface->ulSurAddrReg[1]);
			ulRAddrShadowReg0 = BRDC_REGISTER(pGfxSurface->ulRSurAddrReg[0]);
			ulRAddrShadowReg1 = BRDC_REGISTER(pGfxSurface->ulRSurAddrReg[1]);
		}

		*pulRulCur++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
		*pulRulCur++ = BRDC_REGISTER(pGfxSurface->ulRegIdxReg);
		*pulRulCur++ = BRDC_OP_VAR_XOR_IMM_TO_VAR(BRDC_Variable_0, BRDC_Variable_1);
		*pulRulCur++ = ~0;

		*pulRulCur++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_2);
		*pulRulCur++ = ulAddrShadowReg1;
		*pulRulCur++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_3);
		*pulRulCur++ = ulAddrShadowReg0;

		*pulRulCur++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_0, BRDC_Variable_2, BRDC_Variable_2);
		*pulRulCur++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_1, BRDC_Variable_3, BRDC_Variable_3);
		*pulRulCur++ = BRDC_OP_VAR_OR_VAR_TO_VAR (BRDC_Variable_2, BRDC_Variable_3, BRDC_Variable_2);

		*pulRulCur++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_3);
		*pulRulCur++ = ulBottomOffset;
		*pulRulCur++ = BRDC_OP_VAR_SUM_VAR_TO_VAR(BRDC_Variable_2, BRDC_Variable_3, BRDC_Variable_2);

		*pulRulCur++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_SRC_START ) + ulRulOffset;

		/* right surface and alpha surface are never used at the same time
		 * and they share shadow registers */

#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_4)
		ulAddrReg = BRDC_REGISTER( BCHP_GFD_0_SRC_START_R);
#elif (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1)
		ulBottomOffset = (bSkipTopLine)? pCurSur->ulAlphaPitch : 0;
		ulAddrReg = BRDC_REGISTER( BCHP_GFD_0_ALPHA_START );
#endif

#if ((BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_4) || \
     (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1))

		if((BVDC_P_ORIENTATION_IS_3D(pCurSur->stAvcPic.eInOrientation) &&
			BVDC_P_ORIENTATION_IS_3D(pCurCfg->eOutOrientation)) ||
		   (pCurSur->stAvcPic.hAlphaSurface))
		{
			*pulRulCur++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_2);
			*pulRulCur++ = ulRAddrShadowReg1;
			*pulRulCur++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_3);
			*pulRulCur++ = ulRAddrShadowReg0;

			*pulRulCur++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_0, BRDC_Variable_2, BRDC_Variable_2);
			*pulRulCur++ = BRDC_OP_VAR_AND_VAR_TO_VAR(BRDC_Variable_1, BRDC_Variable_3, BRDC_Variable_3);
			*pulRulCur++ = BRDC_OP_VAR_OR_VAR_TO_VAR (BRDC_Variable_2, BRDC_Variable_3, BRDC_Variable_2);

			*pulRulCur++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_3);
			*pulRulCur++ = ulBottomOffset;
			*pulRulCur++ = BRDC_OP_VAR_SUM_VAR_TO_VAR(BRDC_Variable_2, BRDC_Variable_3, BRDC_Variable_2);

			*pulRulCur++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
			*pulRulCur++ = ulAddrReg + ulRulOffset;
		}
		else
		{
			*pulRulCur++ = BRDC_OP_IMM_TO_REG();
			*pulRulCur++ = ulAddrReg + ulRulOffset;
			*pulRulCur++ = 0;
		}
#endif
		
		/* (7) set RUL for alpha surface pitch, pixInByte offset ... */
#if (BVDC_P_SUPPORT_GFD_VER == BVDC_P_SUPPORT_GFD_VER_1)
		if (pCurSur->stAvcPic.hAlphaSurface &&
			(stDirty.stBits.bSurface || bChangeClipOrField))
		{
			ulAlphaSurPitch = (stFlags.bInterlaced)? 2 * pCurSur->ulAlphaPitch : pCurSur->ulAlphaPitch;
			ulAlphaOffsetPixInByte = hGfxFeeder->ulAlphaOffsetPixInByte;
			*pulRulCur++ = BRDC_OP_IMMS_TO_REGS( GFD_NUM_REGS_ALPHA );
			*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_ALPHA_PITCH ) + ulRulOffset;
			*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_ALPHA_PITCH,  PITCH, ulAlphaSurPitch );
			*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_ALPHA_OFFSET, BLANK_PIXEL_COUNT, ulAlphaOffsetPixInByte );
		}
#endif
	}

	/* !(pGfxSurface->b3dSrc) */
	else
	{
		*pulRulCur++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_0);
		*pulRulCur++ = BRDC_REGISTER(pGfxSurface->ulSurAddrReg[0]);

		*pulRulCur++ = BRDC_OP_IMM_TO_VAR(BRDC_Variable_1);
		*pulRulCur++ = ulBottomOffset;
		*pulRulCur++ = BRDC_OP_VAR_SUM_VAR_TO_VAR(BRDC_Variable_0, BRDC_Variable_1, BRDC_Variable_0);

		*pulRulCur++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_0);
		*pulRulCur++ = BRDC_REGISTER(BCHP_GFD_0_SRC_START) + ulRulOffset;
	}

	/* (8) set RUL for W with alpha expanding for such as WRGB_1555
	 * note: W not necessary from the old separate alpha surface
	 * TODO: it seems that W0 and W1 should also use shadow register */
	if (stDirty.stBits.bSurface)
	{
		*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_W_ALPHA ) + ulRulOffset;
		*pulRulCur++ =
			BCHP_FIELD_DATA( GFD_0_W_ALPHA, W1_ALPHA, pCurSur->stAvcPic.ucW1Alpha ) |
			BCHP_FIELD_DATA( GFD_0_W_ALPHA, W0_ALPHA, pCurSur->stAvcPic.ucW0Alpha );
	}
		
	/* (9) set RUL to increase VsyncCntr */
	*pulRulCur++ = BRDC_OP_REG_TO_VAR(BRDC_Variable_2);
	*pulRulCur++ = BRDC_REGISTER(pGfxSurface->ulVsyncCntrReg);
	*pulRulCur++ = BRDC_OP_VAR_SUM_IMM_TO_VAR(BRDC_Variable_2, BRDC_Variable_2);
	*pulRulCur++ = 1;
	*pulRulCur++ = BRDC_OP_VAR_TO_REG(BRDC_Variable_2);
	*pulRulCur++ = BRDC_REGISTER(pGfxSurface->ulVsyncCntrReg);

	/* reset RUL buffer pointer */
	*ppulRulCur = pulRulCur;

	BDBG_LEAVE(BVDC_P_GfxFeeder_BuildRulForSurCtrl_isr);
	return BERR_TRACE(eResult);
}

/* the following macro must match the HW / RDB spec */
#define GFD_PIXEL_FORMAT_TYPE_ALPHA     5
#define GFD_PIXEL_FORMAT_TYPE_YCRCB422  4
#define GFD_PIXEL_FORMAT_TYPE_PALETTE   3
#define GFD_PIXEL_FORMAT_TYPE_WRGB1565  2
#define GFD_PIXEL_FORMAT_TYPE_WRGB1555  1
#define GFD_PIXEL_FORMAT_TYPE_OTHER     0
/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_DecidePxlFmtType_isr
 *
 * output: the HW pixel format type of the specified pixel format
 */
static BERR_Code BVDC_P_GfxFeeder_DecidePxlFmtType_isr
	( BPXL_Format   eMainSurPxlFmt,
	  BPXL_Format   eAlphaSurPxlFmt,
	  uint32_t     *pulPxlFmtType )
{
	if ( (true  == BPXL_IS_YCbCr_FORMAT(eMainSurPxlFmt)) &&
		 (false == BPXL_HAS_ALPHA(eMainSurPxlFmt)) )
	{
		*pulPxlFmtType = GFD_PIXEL_FORMAT_TYPE_YCRCB422;
	}
	else if ( true == BPXL_IS_PALETTE_FORMAT(eMainSurPxlFmt) )
	{
		*pulPxlFmtType = GFD_PIXEL_FORMAT_TYPE_PALETTE;
	}
	else if ( ((BPXL_eR5_G6_B5 == eMainSurPxlFmt) ||
			   (BPXL_eB5_G6_R5 == eMainSurPxlFmt)) &&
			  (BPXL_eW1 == eAlphaSurPxlFmt) )
	{
		*pulPxlFmtType = GFD_PIXEL_FORMAT_TYPE_WRGB1565;
	}
	else if ( (BPXL_eW1_R5_G5_B5 == eMainSurPxlFmt) ||
			  (BPXL_eW1_B5_G5_R5 == eMainSurPxlFmt) ||
			  (BPXL_eR5_G5_B5_W1 == eMainSurPxlFmt) ||
			  (BPXL_eB5_G5_R5_W1 == eMainSurPxlFmt) )
	{
		*pulPxlFmtType = GFD_PIXEL_FORMAT_TYPE_WRGB1555;
	}
	else if ( true == BPXL_IS_ALPHA_ONLY_FORMAT(eMainSurPxlFmt) )
	{
		*pulPxlFmtType = GFD_PIXEL_FORMAT_TYPE_ALPHA;
	}
	else
	{
		*pulPxlFmtType = GFD_PIXEL_FORMAT_TYPE_OTHER;
	}

	return BERR_SUCCESS;
}

/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_BuildRulForColorCtrl_isr
 *
 * Builds RUL for those color related state changes that are not covered
 * by BVDC_P_GfxFeeder_BuildRulForUserChangeOnly_isr
 *
 * designed to be called by BVDC_P_GfxFeeder_BuildRul_isr only, no paramter
 * validation performed here
 */
static BERR_Code BVDC_P_GfxFeeder_BuildRulForColorCtrl_isr
	( BVDC_P_GfxFeederCfgInfo           *pCurCfg,
	  BVDC_P_SurfaceInfo                *pCurSur,
	  BVDC_P_CscCoeffs                  *pGfxCsc,
	  uint32_t                         **ppulRulCur,
	  uint32_t                           ulRulOffset )
{
	BERR_Code   eResult = BERR_SUCCESS;
	uint32_t  *pulRulCur;
	uint32_t   ulClutAddr, ulClutSize;
	uint32_t   ulSurFormatType;
	BPXL_Format  eMainPxlFmt, eAlphaPxlFmt;
	BVDC_P_GfxDirtyBits  stDirty = pCurCfg->stDirty;

	/* init RUL buffer pointers */
	pulRulCur = *ppulRulCur;

	/* set RUL for palatte color look up table loading */
	if (stDirty.stBits.bPaletteTable)
	{
		ulClutAddr = pCurSur->ulPaletteAddress;
		ulClutSize = pCurSur->ulPaletteNumEntries;

		/* set the addr and size for table loading */
		*pulRulCur++ = BRDC_OP_IMMS_TO_REGS( GFD_NUM_REGS_PALETTE );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_PALETTE_START ) + ulRulOffset;
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_PALETTE_START, ADDR, ulClutAddr );
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_PALETTE_SIZE,  SIZE, ulClutSize );

		/* triger the table loading */
		*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_LOAD_PALETTE ) + ulRulOffset;
		*pulRulCur++ = BCHP_FIELD_DATA( GFD_0_LOAD_PALETTE, LOAD_PALETTE, 1);
	}

	/* set RUL for pixel format: if alpha is disabled, then its fmt had been
	 * set to BPXL_eInvalid */
	if (stDirty.stBits.bPxlFmt)
	{
		eMainPxlFmt = pCurSur->eInputPxlFmt;
		eAlphaPxlFmt  = (pCurSur->stAvcPic.hAlphaSurface)? BPXL_eW1 : BPXL_eInvalid;
		BVDC_P_GfxFeeder_DecidePxlFmtType_isr( eMainPxlFmt, eAlphaPxlFmt, &ulSurFormatType );
		*pulRulCur++ = BRDC_OP_IMMS_TO_REGS( GFD_NUM_REGS_PIXEL_FORMAT );
		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_FORMAT_DEF_1 ) + ulRulOffset;
		*pulRulCur++ =
			BCHP_FIELD_DATA( GFD_0_FORMAT_DEF_1, FORMAT_TYPE,  ulSurFormatType ) |
			BCHP_FIELD_DATA( GFD_0_FORMAT_DEF_1, CH3_NUM_BITS, BPXL_COMPONENT_SIZE(eMainPxlFmt, 3) ) |
			BCHP_FIELD_DATA( GFD_0_FORMAT_DEF_1, CH2_NUM_BITS, BPXL_COMPONENT_SIZE(eMainPxlFmt, 2) ) |
			BCHP_FIELD_DATA( GFD_0_FORMAT_DEF_1, CH1_NUM_BITS, BPXL_COMPONENT_SIZE(eMainPxlFmt, 1) ) |
			BCHP_FIELD_DATA( GFD_0_FORMAT_DEF_1, CH0_NUM_BITS, BPXL_COMPONENT_SIZE(eMainPxlFmt, 0) );
		*pulRulCur++ =
			BCHP_FIELD_DATA( GFD_0_FORMAT_DEF_2, CH3_LSB_POS, BPXL_COMPONENT_POS(eMainPxlFmt, 3) ) |
			BCHP_FIELD_DATA( GFD_0_FORMAT_DEF_2, CH2_LSB_POS, BPXL_COMPONENT_POS(eMainPxlFmt, 2) ) |
			BCHP_FIELD_DATA( GFD_0_FORMAT_DEF_2, CH1_LSB_POS, BPXL_COMPONENT_POS(eMainPxlFmt, 1) ) |
			BCHP_FIELD_DATA( GFD_0_FORMAT_DEF_2, CH0_LSB_POS, BPXL_COMPONENT_POS(eMainPxlFmt, 0) );
	}

	/* set RUL for color matrix */
	if (stDirty.stBits.bCsc)
	{
		/* write color elements from the table to rul */
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER > 0)
		*pulRulCur++ = BRDC_OP_IMMS_TO_REGS(
			BVDC_P_REGS_ENTRIES(GFD_0_CSC_COEFF_C00, GFD_0_CSC_COEFF_C24));

		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_CSC_COEFF_C00 ) + ulRulOffset;

		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C00, CSC_COEFF_C0, pGfxCsc->usY0);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C01, CSC_COEFF_C1, pGfxCsc->usY1);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C02, CSC_COEFF_C2, pGfxCsc->usY2);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C03, CSC_COEFF_C3, pGfxCsc->usYAlpha);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C04, CSC_COEFF_C4, pGfxCsc->usYOffset);

		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C10, CSC_COEFF_C0, pGfxCsc->usCb0);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C11, CSC_COEFF_C1, pGfxCsc->usCb1);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C12, CSC_COEFF_C2, pGfxCsc->usCb2);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C13, CSC_COEFF_C3, pGfxCsc->usCbAlpha);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C14, CSC_COEFF_C4, pGfxCsc->usCbOffset);

		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C20, CSC_COEFF_C0, pGfxCsc->usCr0);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C21, CSC_COEFF_C1, pGfxCsc->usCr1);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C22, CSC_COEFF_C2, pGfxCsc->usCr2);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C23, CSC_COEFF_C3, pGfxCsc->usCrAlpha);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CSC_COEFF_C24, CSC_COEFF_C4, pGfxCsc->usCrOffset);
#else
		*pulRulCur++ = BRDC_OP_IMMS_TO_REGS(
			BVDC_P_REGS_ENTRIES(GFD_0_CM_C00_C01, GFD_0_CM_C24));

		*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_CM_C00_C01 ) + ulRulOffset;

		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CM_C00_C01, CM_C00, pGfxCsc->usY0) |
		               BCHP_FIELD_DATA(GFD_0_CM_C00_C01, CM_C01, pGfxCsc->usY1);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CM_C02_C03, CM_C02, pGfxCsc->usY2) |
		               BCHP_FIELD_DATA(GFD_0_CM_C02_C03, CM_C03, pGfxCsc->usYAlpha);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CM_C04,     CM_C04, pGfxCsc->usYOffset);

		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CM_C10_C11, CM_C10, pGfxCsc->usCb0) |
		               BCHP_FIELD_DATA(GFD_0_CM_C10_C11, CM_C11, pGfxCsc->usCb1);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CM_C12_C13, CM_C12, pGfxCsc->usCb2) |
		               BCHP_FIELD_DATA(GFD_0_CM_C12_C13, CM_C13, pGfxCsc->usCbAlpha);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CM_C14,     CM_C14, pGfxCsc->usCbOffset);

		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CM_C20_C21, CM_C20, pGfxCsc->usCr0) |
		               BCHP_FIELD_DATA(GFD_0_CM_C20_C21, CM_C21, pGfxCsc->usCr1);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CM_C22_C23, CM_C22, pGfxCsc->usCr2) |
		               BCHP_FIELD_DATA(GFD_0_CM_C22_C23, CM_C23, pGfxCsc->usCrAlpha);
		*pulRulCur++ = BCHP_FIELD_DATA(GFD_0_CM_C24,     CM_C24, pGfxCsc->usCrOffset);
#endif
	}

	/* reset RUL buffer pointer */
	*ppulRulCur = pulRulCur;

	return BERR_TRACE(eResult);
}

#define GFD_ENABLE_GFD     1
/*------------------------------------------------------------------------
 * {private}
 * BVDC_P_GfxFeeder_BuildRulForEnableCtrl_isr
 *
 * designed to be called by BVDC_P_GfxFeeder_BuildRul_isr only, no paramter
 * validation performed here
 */
static BERR_Code BVDC_P_GfxFeeder_BuildRulForEnableCtrl_isr
	( BVDC_P_GfxFeederCfgInfo       *pCurCfg,
	  BVDC_P_State                   eVnetState,
	  BFMT_Orientation               eInOrientation,
	  uint32_t                     **ppulRulCur,
	  uint32_t                       ulRulOffset )
{
	BERR_Code  eResult = BERR_SUCCESS;
	uint32_t  *pulRulCur;
	uint32_t  ulEnClrMtrx, ulEnGamma, ulDoneAlphaPreMul, ulEnKey, ulEnHscl;
	uint32_t  ulPointSample, ulContinue, ulEnGfd;
	BVDC_P_GfxCfgFlags  stFlags;
#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_4)
	uint32_t ulInOrientation, ulOutOrientation;
#else
	BSTD_UNUSED(eInOrientation);
#endif

	/* init RUL buffer pointers */
	pulRulCur = *ppulRulCur;

	/* set RUL for GFD_0_CTRL,
	 * must be done after scale and color matrix are decided,
	 * always set without compraring, because it is short and ulEnScale
	 * depends on surface that could be set in isr
	 */
	/* note: for alpha pre-multiply, our concept is the opposite of HW! */
	stFlags = pCurCfg->stFlags;
	ulEnClrMtrx       = 1;
	ulEnHscl          = stFlags.bNeedHorizScale;
	ulEnGamma         = stFlags.bEnableGammaCorrection;
	ulDoneAlphaPreMul = (stFlags.bEnGfdHwAlphaPreMultiply)? 0: 1; /* NOT 1: 0*/
	ulEnKey           = (stFlags.bEnableKey) && (false == stFlags.bConstantBlending);
	ulPointSample     = (BVDC_FilterCoeffs_ePointSample == pCurCfg->eScaleCoeffs)? 1: 0;

#if (BVDC_P_SUPPORT_GFD_VER >= BVDC_P_SUPPORT_GFD_VER_4)
	ulOutOrientation =
		BVDC_P_ORIENTATION_IS_3D(pCurCfg->eOutOrientation) ?
		pCurCfg->eOutOrientation : BFMT_Orientation_e2D;
	ulInOrientation  =
		(BVDC_P_ORIENTATION_IS_3D(eInOrientation) &&
		 BVDC_P_ORIENTATION_IS_3D(ulOutOrientation)) ?
		BCHP_FIELD_ENUM( GFD_0_CTRL, MEM_VIDEO,        MODE_3D_DUAL_POINTER) :
		BCHP_FIELD_ENUM( GFD_0_CTRL, MEM_VIDEO,        MODE_2D);

	*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
	*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_CTRL + ulRulOffset );
	*pulRulCur++ =
		BCHP_FIELD_DATA( GFD_0_CTRL, CSC_ENABLE,       ulEnClrMtrx )       |
		BCHP_FIELD_DATA( GFD_0_CTRL, GC_ENABLE,        ulEnGamma )         |
		BCHP_FIELD_DATA( GFD_0_CTRL, HFIR_ENABLE,      ulEnHscl )          |
		BCHP_FIELD_DATA( GFD_0_CTRL, CLUT_SCALE_MODE,  ulPointSample )     |
		BCHP_FIELD_DATA( GFD_0_CTRL, ALPHA_PRE_MULT,   ulDoneAlphaPreMul ) |
		BCHP_FIELD_DATA( GFD_0_CTRL, COLOR_KEY_ENABLE, ulEnKey )           |
		ulInOrientation                                                    |
		BCHP_FIELD_DATA( GFD_0_CTRL, BVB_VIDEO,        ulOutOrientation);
#else
	*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
	*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_CTRL + ulRulOffset );
	*pulRulCur++ =
		BCHP_FIELD_DATA( GFD_0_CTRL, CSC_ENABLE,	   ulEnClrMtrx )	   |
		BCHP_FIELD_DATA( GFD_0_CTRL, GC_ENABLE, 	   ulEnGamma )		   |
		BCHP_FIELD_DATA( GFD_0_CTRL, HFIR_ENABLE,	   ulEnHscl )		   |
		BCHP_FIELD_DATA( GFD_0_CTRL, CLUT_SCALE_MODE,  ulPointSample )	   |
		BCHP_FIELD_DATA( GFD_0_CTRL, ALPHA_PRE_MULT,   ulDoneAlphaPreMul ) |
		BCHP_FIELD_DATA( GFD_0_CTRL, COLOR_KEY_ENABLE, ulEnKey );
#endif

	/*vertical scaling is enabled in GSCL_CFG */

	/* Set RUL for gfx feeder enable:
	 * Note: Enabling should be put at the last of gfx feeder config RUL,
	 * We use both eVnetState to control gfd enabling and gfx surface enabling
	 * in compositor */
	ulEnGfd = (BVDC_P_State_eActive == eVnetState)? GFD_ENABLE_GFD : 0;
	ulContinue = stFlags.bContinueOnFieldEnd;
	*pulRulCur++ = BRDC_OP_IMM_TO_REG( );
	*pulRulCur++ = BRDC_REGISTER( BCHP_GFD_0_ENABLE + ulRulOffset );
	*pulRulCur++ =
		BCHP_FIELD_DATA( GFD_0_ENABLE, ENABLE,      ulEnGfd ) |
		BCHP_FIELD_DATA( GFD_0_ENABLE, ENABLE_CTRL, ulContinue );

	/* reset RUL buffer pointer */
	*ppulRulCur = pulRulCur;

	return BERR_TRACE(eResult);
}

/*------------------------------------------------------------------------
 *  {secret}
 *	BVDC_P_GfxFeeder_HandleIsrSurface_isr
 *
 *  No paramter error check is performed. Designed to be called by
 *  BVDC_P_GfxFeeder_BuildRul_isr only
 *
 *  It pulls a new isr surface if callback func is installed; then activates
 *  any new set surace (by Source_SetSurface_isr or this callback func). It
 *  at first validates the combination of this surface and current confugure,
 *  and set up to build RUL for this combination.
 */
static void BVDC_P_GfxFeeder_HandleIsrSurface_isr
	( BVDC_P_GfxFeeder_Handle      hGfxFeeder,
  	  BVDC_P_Source_Info *         pCurSrcInfo,
	  BAVC_Polarity                eFieldId )
{
	BVDC_P_GfxFeederCfgInfo  *pCurCfg;
	void  *pvGfxPic;
	const BVDC_P_ClipRect  *pCurClip;
	const BVDC_P_Rect  *pCurDst, *pCurSclOut;
	BVDC_P_SurfaceInfo  *pIsrSur = &(hGfxFeeder->stGfxSurface.stIsrSurInfo);
	BERR_Code  eResult;

	if( NULL != pCurSrcInfo->pfPicCallbackFunc )
	{
		pCurSrcInfo->pfPicCallbackFunc(
			pCurSrcInfo->pvParm1, pCurSrcInfo->iParm2,
			eFieldId, BAVC_SourceState_eActive, &pvGfxPic);
		if ( NULL != pvGfxPic )
		{
			BVDC_P_GfxSurface_SetSurface_isr(&hGfxFeeder->stGfxSurface,
				pIsrSur, (BAVC_Gfx_Picture *)pvGfxPic);
		}
	}

	/* after previous vsync, pIsrSur->bChangeSurface might be set by
	 * Source_SetSurface_isr, by the above callback function, or not re-set
	 * at all. If it was re-set, and there was no change in format, size, or
	 * pitch, then it has already been sent to HW in GfxSurface_SetSurface_isr,
	 * and pIsrSur->bChangeSurface has been marked as false */
	if (pIsrSur->bChangeSurface && hGfxFeeder->hWindow)
	{
		BVDC_P_Window_GetCurrentRectangles(
			hGfxFeeder->hWindow, &pCurClip, &pCurSclOut, &pCurDst);

		/* validate surface size and rectangles, adjust rectangles and do some
		 * scale related computation. The intermediate values are stored in
		 * CurCfg, and will affect RUL build immediately. */
		pCurCfg = &(hGfxFeeder->stCurCfgInfo);
		eResult = BVDC_P_GfxFeeder_ValidateSurAndRects_isr(
			hGfxFeeder, pIsrSur, pCurCfg, pCurClip, pCurSclOut, pCurDst);
		if (BERR_SUCCESS == eResult)
		{
			/* Copy IsrSur to curSur
			 * note: pCurSur->bChangeSurface would stay true so that the surface
			 * change is built into RUL later */
			hGfxFeeder->stGfxSurface.stCurSurInfo = *pIsrSur;

			/* to avoid future validation if isr surface no longer changes */
			pIsrSur->bChangeSurface  = false;
		}
		/* otherwise we stay with original current surface and ignore this
		 * IsrSur until either Cfg or surface changes
		 * note: ValidateSurAndRects will NOT change CurCfg until all error
		 * check are completed */
	}
}

/*************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_BuildRul_isr
 *
 * Append GfxFeeder specific RULs into hList.
 *
 */
void BVDC_P_GfxFeeder_BuildRul_isr
	( BVDC_P_GfxFeeder_Handle          hGfxFeeder,
	  BVDC_P_Source_Info *             pCurSrcInfo,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId,
	  BVDC_P_State                     eVnetState )
{
	BVDC_P_SurfaceInfo  *pCurSur;
	BVDC_P_GfxFeederCfgInfo  *pCurCfg;
	BVDC_P_GfxDirtyBits  stCurDirty;
	BVDC_P_CscCoeffs *pGfxCsc;
	BVDC_P_CscCoeffs const *pRGBToYCbCr, *pYCbCrToRGB;
	uint32_t  *pulRulCur;
	uint32_t  ulRulOffset;

	BDBG_ENTER(BVDC_P_GfxFeeder_BuildRul_isr);

	/* Note: we will not be called if this src is not connected to a window */
	/* validate paramters */
	BDBG_OBJECT_ASSERT(hGfxFeeder, BVDC_GFX);
	BDBG_ASSERT( NULL != pCurSrcInfo );

	/* init current state ptrs */
	pCurSur = &(hGfxFeeder->stGfxSurface.stCurSurInfo);
	pCurCfg = &(hGfxFeeder->stCurCfgInfo);

	/* If surface has been set by *SetSurface_isr, or src pic call back func is
	 * installed, we activate it now */
	BVDC_P_GfxFeeder_HandleIsrSurface_isr( hGfxFeeder, pCurSrcInfo, eFieldId );

	/* init RUL buffer pointers */
	pulRulCur = pList->pulCurrent;
	ulRulOffset = hGfxFeeder->ulRegOffset;

	/* this copy should be after HandleIsrSurface_isr since it might change Dirty */
	stCurDirty = pCurCfg->stDirty;
	if(!pList->bLastExecuted)
	{
		/* rebuild RUL for the changes done in last vsync, because last vsync's
		 * RUL is lost */
		BVDC_P_OR_ALL_DIRTY(&stCurDirty, &hGfxFeeder->stPrevDirty);
	}

	/* at this point CurSur could be the same as last vsync, be changed by SetSurface
	 * /SetSurface_isr, or by the callback func */
	if(pCurSur->bChangeSurface)
	{
		/* pass ChangeSurface bit to stDirty, This will ensure the change is
		 * set to HW. */
		stCurDirty.stBits.bSurface = BVDC_P_DIRTY;
		stCurDirty.stBits.bPaletteTable =
			(pCurSur->bChangePaletteTable)? BVDC_P_DIRTY : BVDC_P_CLEAN;
		pCurSur->bChangePaletteTable = false;
	}

#if (BVDC_P_GFX_INIT_CNTR > 1)
	/* after bInitial, we need to SetAllDirty one more time? */
	if(hGfxFeeder->ulInitVsyncCntr && BVDC_P_GFX_INIT_CNTR > hGfxFeeder->ulInitVsyncCntr)
	{
		BVDC_P_GfxFeeder_SetAllDirty(&stCurDirty);
		hGfxFeeder->ulInitVsyncCntr --;
	}
#endif

	/* BuildRulFor* will use pCurCfg->stDirty, so copy back after modification */
	pCurCfg->stDirty = stCurDirty;

	/* SW3548-2976 workaround: reset GFD if DCX is siwtched ON->OFF */
	if(BVDC_P_GFX_INIT_CNTR == hGfxFeeder->ulInitVsyncCntr)
	{
		BVDC_P_BUILD_RESET(pulRulCur,
			hGfxFeeder->ulResetRegAddr, hGfxFeeder->ulResetMask);
		hGfxFeeder->ulInitVsyncCntr --;
	}

	/* resolve color conversion state */
	if (hGfxFeeder->hWindow)
	{
		pGfxCsc = &hGfxFeeder->stGfxCsc;
		BVDC_P_GfxFeeder_DecideColorMatrix_isr(
			pCurSur->eActivePxlFmt, hGfxFeeder, pCurCfg->stFlags.bConstantBlending,
			pGfxCsc, &pRGBToYCbCr, &pYCbCrToRGB);

		/* color adjustment */
		if ( stCurDirty.stBits.bCsc )
		{
			BVDC_P_Csc_ApplyContrast_isr(hGfxFeeder->hWindow->stCurInfo.sContrast, &hGfxFeeder->stGfxCsc);
			BVDC_P_Csc_ApplyBrightness_isr(hGfxFeeder->hWindow->stCurInfo.sBrightness, &hGfxFeeder->stGfxCsc);
			BVDC_P_Csc_ApplySaturationAndHue_isr(
				hGfxFeeder->hWindow->stCurInfo.sSaturation,
				hGfxFeeder->hWindow->stCurInfo.sHue, &hGfxFeeder->stGfxCsc);
			BVDC_P_Csc_ApplyAttenuationRGB_isr(
				hGfxFeeder->hWindow->stCurInfo.lAttenuationR,
				hGfxFeeder->hWindow->stCurInfo.lAttenuationG,
				hGfxFeeder->hWindow->stCurInfo.lAttenuationB,
				hGfxFeeder->hWindow->stCurInfo.lOffsetR,
				hGfxFeeder->hWindow->stCurInfo.lOffsetG,
				hGfxFeeder->hWindow->stCurInfo.lOffsetB,
				pGfxCsc,
				pYCbCrToRGB, /* YCbCr->RGB */
				pRGBToYCbCr, /* RGB->YCbCr */
				hGfxFeeder->hWindow->stCurInfo.bUserCsc,
				hGfxFeeder->hWindow->stCurInfo.bUserCscC);
		}
	}

	/* set RULs, gfx should be enabled at the last of config */
	BVDC_P_GfxFeeder_BuildRulForUserChangeOnly_isr(
		pCurCfg, &pulRulCur, ulRulOffset );

	BVDC_P_GfxFeeder_BuildRulForSurCtrl_isr(
		hGfxFeeder, eFieldId, &pulRulCur, ulRulOffset );

	BVDC_P_GfxFeeder_BuildRulForColorCtrl_isr(
		pCurCfg, pCurSur, &hGfxFeeder->stGfxCsc, &pulRulCur, ulRulOffset );

	BVDC_P_GfxFeeder_BuildRulForEnableCtrl_isr(
		pCurCfg, eVnetState, pCurSur->stAvcPic.eInOrientation, &pulRulCur, ulRulOffset );

	/* copy dirty bits to PrevDirty in case this RUL never goes to HW */
	hGfxFeeder->stPrevDirty = stCurDirty;

	BVDC_P_CLEAN_ALL_DIRTY(&(pCurCfg->stDirty));
	pCurSur->bChangeSurface = false; /* must reset after BuildRulForSurCtrl_isr */

	pList->pulCurrent = pulRulCur;
	BDBG_LEAVE(BVDC_P_GfxFeeder_BuildRul_isr);
	return;
}

/***************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_ValidateBlend
 *
 * Called by BVDC_Window_SetBlendFactor to validate the graphics window
 * blending factor setting
 */
BERR_Code BVDC_P_GfxFeeder_ValidateBlend
	( BVDC_BlendFactor             eSrcBlendFactor,
	  BVDC_BlendFactor             eDstBlendFactor,
	  uint8_t                      ucConstantAlpha )
{
	BERR_Code eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_GfxFeeder_ValidateBlend);
	if ( false ==
		 (((BVDC_BlendFactor_eSrcAlpha         == eSrcBlendFactor) &&
		   (BVDC_BlendFactor_eOneMinusSrcAlpha == eDstBlendFactor))       ||
		  ((BVDC_BlendFactor_eConstantAlpha    == eSrcBlendFactor) &&
		   (BVDC_BlendFactor_eOneMinusSrcAlpha == eDstBlendFactor) &&
		   (BVDC_ALPHA_MAX                     == ucConstantAlpha))       ||
		  ((BVDC_BlendFactor_eConstantAlpha         == eSrcBlendFactor) &&
		   (BVDC_BlendFactor_eOneMinusConstantAlpha == eDstBlendFactor))  ||
		  ((BVDC_BlendFactor_eOneMinusConstantAlpha == eSrcBlendFactor) &&
		   (BVDC_BlendFactor_eConstantAlpha         == eDstBlendFactor))  ||
		  ((BVDC_BlendFactor_eOne  == eSrcBlendFactor) &&
		   (BVDC_BlendFactor_eZero == eDstBlendFactor))                   ||
		  ((BVDC_BlendFactor_eZero == eSrcBlendFactor) &&
		   (BVDC_BlendFactor_eOne  == eDstBlendFactor))) )
	{
		/* we support 3 cases for blend factor, refer to
		 * BVDC_P_GfxFeeder_ValidateChanges for detail */
		eResult = BERR_TRACE(BVDC_ERR_ILLEGAL_GFX_WIN_BLEND);
	}

	BDBG_LEAVE(BVDC_P_GfxFeeder_ValidateBlend);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 * BVDC_P_GfxFeeder_AdjustBlend_isr
 *
 * Called by BVDC_P_Window_SetBlender to adjust the blending factor
 * of a graphics window for HW register setting
 *
 * Note: peSrcBlendFactor, peDstBlendFactor, and pucConstantAlpha are both
 * input and output of this function, they must be filled with current
 * values before calling this function
 */
BERR_Code BVDC_P_GfxFeeder_AdjustBlend_isr
	( BVDC_BlendFactor            *peSrcBlendFactor,
	  BVDC_BlendFactor            *peDstBlendFactor,
	  uint8_t                     *pucConstantAlpha )
{
	BERR_Code eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_GfxFeeder_AdjustBlend_isr);

	if ( (NULL != peSrcBlendFactor) && (NULL != peDstBlendFactor) &&
		 (NULL != pucConstantAlpha) )
	{
		/* refer to BVDC_P_GfxFeeder_ValidateChanges, there is only one
		 * case, when users means to use src alpha for blending factors,
		 * we need to adjust blending setting. In this case, we should
		 * enable the alpha pre-multiply in GFD HW and accordingly
		 * adjust SrcBlendDactor to BVDC_BlendFactor_eConstantAlpha.
		 * This is because the alpha output from color key stage is always
		 * pre-multiplied to pixel color by HW */
		if (BVDC_BlendFactor_eSrcAlpha == *peSrcBlendFactor )
		{
			*peSrcBlendFactor = BVDC_BlendFactor_eConstantAlpha;
			*pucConstantAlpha = GFX_ALPHA_FULL;
		}
	}
	else
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	BDBG_LEAVE(BVDC_P_GfxFeeder_AdjustBlend_isr);
	return BERR_TRACE(eResult);
}

/* End of File */
