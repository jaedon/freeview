/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc.c $
 * $brcm_Revision: Hydra_Software_Devel/312 $
 * $brcm_Date: 11/12/12 2:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc.c $
 * 
 * Hydra_Software_Devel/312   11/12/12 2:01p pntruong
 * SW7552-375: Correctly destroy and re-create timer.
 *
 * Hydra_Software_Devel/311   11/9/12 2:59p yuxiaz
 * SW7552-375: Need to destroy/re-create TMR when VDC go into
 * standby/resume.
 *
 * Hydra_Software_Devel/310   9/10/12 6:31p tdo
 * SW7231-837: Remove band gap calibration option
 *
 * Hydra_Software_Devel/309   9/10/12 6:26p tdo
 * SW7231-837: Remove band gap calibration option
 *
 * Hydra_Software_Devel/308   8/16/12 2:22p yuxiaz
 * SW7425-3619: Replace uncache memory access with cache
 * memory access.
 *
 * Hydra_Software_Devel/307   8/15/12 6:20p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/306   8/13/12 4:01p tdo
 * SW7425-3734: Fix coverity issues
 *
 * Hydra_Software_Devel/305   8/7/12 11:58a tdo
 * SW7552-297: VEC reset when resume
 *
 * Hydra_Software_Devel/SW7552-299/1   8/7/12 1:12p xhuang
 * SW7552-297: reset VEC when resume from standby mode
 *
 * Hydra_Software_Devel/304   7/27/12 1:50p tdo
 * SW7231-837:Default using HW calibration (gain override off) for all
 * chips except 7231.  Gain override effecting DAC detection
 *
 * Hydra_Software_Devel/303   7/24/12 3:02p tdo
 * SW7360-29 : Temporarily turn off DAC auto-detection & calibration until
 * it's fully working for 7360 and 7231
 *
 * Hydra_Software_Devel/302   7/13/12 11:23a tdo
 * SW7563-9: Add support for VDC
 *
 * Hydra_Software_Devel/301   7/6/12 12:37p tdo
 * SW7231-837: Rename to add option to control band gap calibration
 *
 * Hydra_Software_Devel/300   7/5/12 12:02p tdo
 * SW7231-837:  Add option to turn on/off Dac Gain Override
 *
 * Hydra_Software_Devel/299   6/14/12 3:41p syang
 * SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
 * prepare to add SetSurface support to MFD
 *
 * Hydra_Software_Devel/298   5/31/12 3:18p syang
 * SW7425-2093: alloc scratch reg for a playback's timeStamp if the vfd is
 * marked "false" for source creation during BVDC_Open
 *
 * Hydra_Software_Devel/297   5/30/12 4:53p syang
 * SW7425-2093: extra ApplyChange with 1st SetSurface no longer needed;
 * vfd/cap time stamp related scratch registers now share with vfd's
 * surface manager's; added vfd resource check; more clean up
 *
 * Hydra_Software_Devel/296   4/27/12 4:02p tdo
 * SW7584-21: Add PI/vdc support for 7584
 *
 * Hydra_Software_Devel/295   4/26/12 11:34a tdo
 * SW7425-2571: Add VDC API to query BVN module error
 *
 * Hydra_Software_Devel/294   3/30/12 5:29p pntruong
 * SW7435-80: Reduced BVDC_Open-time allocations from BMEM based on
 * maximum usage.
 *
 * Hydra_Software_Devel/293   2/8/12 10:05a pntruong
 * SW7360-3: Initial support.
 *
 * Hydra_Software_Devel/292   1/13/12 2:51p tdo
 * SW7358-159 , SW7418-55 : Default DAC auto detection ON for
 * 7231/7344/7346Bx
 *
 * Hydra_Software_Devel/291   1/12/12 4:33p yuxiaz
 * SW7552-181: Adjust buffer size based on alignment.
 *
 * Hydra_Software_Devel/290   11/23/11 4:24p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/289   11/23/11 11:30a tdo
 * SW7435-9: add support for 7435A0 in VDC
 *
 * Hydra_Software_Devel/288   11/18/11 2:27p pntruong
 * SW7425-1727: Corrected the default bandgap default values.
 *
 * Hydra_Software_Devel/287   11/8/11 9:57p tdo
 * SW7358-159: No ouput on composite port for B0.  Enable DAC auto
 * detection for chipsets with the older DAC detection logic
 *
 * Hydra_Software_Devel/286   11/4/11 3:23p pntruong
 * SW7231-387: Enabled hddvi for b0 build.
 *
 * Hydra_Software_Devel/285   11/1/11 2:38p pntruong
 * SW7231-387: Enabled hddvi for b0 build.
 *
 * Hydra_Software_Devel/284   10/14/11 2:23p tdo
 * SW7425-1416, SW7358-159: Add feature to control automatic DAC
 * detection.  Default is currently OFF until it's fully functional.
 *
 * Hydra_Software_Devel/283   10/6/11 4:12p pntruong
 * SW7429-16: Initial support.
 *
 * Hydra_Software_Devel/282   9/22/11 3:32p pntruong
 * SW3548-3090: Pruned ununsed code.
 *
 * Hydra_Software_Devel/281   9/6/11 3:34p tdo
 * SW7425-979:  Change init value for Dac gain adj for 7425B0/7346B0
 *
 * Hydra_Software_Devel/280   8/25/11 3:55p pntruong
 * SW7425-1191: Rollback.  The acquire/release is more efficience to be
 * handle in nexus, where the done event is already known.
 *
 * Hydra_Software_Devel/279   8/25/11 9:23a vanessah
 * SW7425-1191:sg pwr management
 *
 * Hydra_Software_Devel/278   8/4/11 7:20p tdo
 * SW7425-979: Add support for 7425B0 DAC name change
 *
 * Hydra_Software_Devel/277   7/11/11 2:16p tdo
 * SW7420-1971: Video Pause seen when VEC alignment is going on.  Add flag
 * to keep BVN connected while doing alignment.
 *
 * Hydra_Software_Devel/276   6/8/11 2:55p pntruong
 * SW7346-276: Corrected populating the bandgap field.
 *
 * Hydra_Software_Devel/275   6/7/11 11:44a pntruong
 * SW7400-3034, SW7400-2808: Better guarded checks, and refactored
 * duplicate codes.
 *
 * Hydra_Software_Devel/274   2/28/11 10:16a vanessah
 * SW7422-280: change the default setting
 *
 * Hydra_Software_Devel/273   2/25/11 4:15p yuxiaz
 * SW7408-210: Fixed segfault in BVDC_Standby.
 *
 * Hydra_Software_Devel/272   2/17/11 6:31p jtna
 * SW7420-1456: update BCHP_PWR pm code
 *
 * Hydra_Software_Devel/271   2/16/11 7:12p jtna
 * SW7420-1456: fix typo
 *
 * Hydra_Software_Devel/270   2/16/11 5:29p pntruong
 * SW7420-1456: Control the top level for vdc.
 *
 * Hydra_Software_Devel/269   2/9/11 3:58p yuxiaz
 * SW7400-2882: Ported multi-buffing event logging scheme to VDC. Move
 * related APIs to bvdc_dbg.h
 *
 * Hydra_Software_Devel/268   2/9/11 3:35p pntruong
 * SW7420-1456: Initial standby power management that used chp's pm
 * functionalities.
 *
 * Hydra_Software_Devel/267   12/14/10 5:32p pntruong
 * SW7231-2: Removed BCHP_CHIP spot fixes.  Programmed bandgap adjustment
 * number from API.  Programmed power management for bandgap power down.
 * Need to add for individual dac.
 *
 * Hydra_Software_Devel/266   11/17/10 11:02p hongtaoz
 * SW7422-13: 7422 has no 656in source;
 *
 * Hydra_Software_Devel/265   11/16/10 3:05p hongtaoz
 * SW7425-33: added gfd3 support and vscl detection;
 *
 * Hydra_Software_Devel/264   11/15/10 5:59p pntruong
 * SW7231-2: Initial support to make 7231 compile.
 *
 * Hydra_Software_Devel/263   11/12/10 3:55p pntruong
 * SW7425-31: Takes bvn yuv into account for hddvi input.  Fixed bad debug
 * register read on non-mfd source.  Updated scratch registers
 * availabliity.
 *
 * Hydra_Software_Devel/262   11/11/10 7:27p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/261   11/8/10 4:58p yuxiaz
 * SW7552-7: Add initial support for 7552.
 *
 * Hydra_Software_Devel/260   10/20/10 4:10p yuxiaz
 * SW7420-1190: Put back generic VDC drian buffer for mosaic mode. Only
 * allocate drain buffer in window if it does not use main VDC heap.
 *
 * Hydra_Software_Devel/259   10/19/10 4:06p yuxiaz
 * SW7420-1190: Make mosaic scratch buffer to be per window base.
 *
 * Hydra_Software_Devel/258   10/19/10 1:44p pntruong
 * SW7425-31: Initial support for hddvi.
 *
 * Hydra_Software_Devel/257   10/18/10 10:05a yuxiaz
 * SW7422-39: Added BAVC_SourceId_eMpeg2 support in VDC.
 *
 * Hydra_Software_Devel/256   10/11/10 11:33a jessem
 * SW7420-173: Added support for using VFD as  a  source.
 *
 * Hydra_Software_Devel/255   9/29/10 11:40a tdo
 * SW7425-33: VDC: Add the 4th Compositor Support for 7422/7425
 *
 * Hydra_Software_Devel/254   9/13/10 4:33p yuxiaz
 * SW7358-4: Added initial VDC support for 7358.
 *
 * Hydra_Software_Devel/253   9/8/10 3:09p hongtaoz
 * SW7425-13: disabled 7425 HDDVI source tempoarily;
 *
 * Hydra_Software_Devel/252   8/30/10 4:14p tdo
 * SW7425-11, SW7425-13: Add 7425 support for VDC
 *
 * Hydra_Software_Devel/251   8/26/10 5:22p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/250   8/6/10 11:41a rpan
 * SW7400-2882: Integrated the multi-buffering event logging scheme.
 *
 * Hydra_Software_Devel/249   7/7/10 1:11p pntruong
 * SW35230-575: Corrected the alignment pameter for mem alloc.
 *
 * Hydra_Software_Devel/248   6/23/10 4:59p rpan
 * SW7400-2808: Stop enabling BVN while aligning VECs.
 *
 * Hydra_Software_Devel/247   5/7/10 7:07p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings.
 *
 * Hydra_Software_Devel/246   4/19/10 10:10p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/245   4/7/10 11:22a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/244   4/5/10 3:57p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/243   3/31/10 6:41p albertl
 * SW7420-674: Changed bandgap defaults to twenty-six for all chips.
 *
 * Hydra_Software_Devel/242   1/6/10 3:59p tdo
 * SW7420-535: Change TMR code to use shared timer.  Remove direct
 * register access.
 *
 * Hydra_Software_Devel/241   1/6/10 3:50p tdo
 * SW7420-535: Change TMR code to use shared timer
 *
 * Hydra_Software_Devel/240   11/19/09 10:15a pntruong
 * SW7408-13: Initial check in to get 7408 build.
 *
 * Hydra_Software_Devel/239   11/17/09 2:43p rpan
 * SW7468-20: 7468 work.
 *
 * Hydra_Software_Devel/238   10/23/09 5:16p pntruong
 * SW7340-54, SW7342-48, SW7340-57, SW7340-56, SW7125-36: Temporary don't
 * enable clock gating.
 *
 * Hydra_Software_Devel/237   9/24/09 9:20p tdo
 * SW7125-27: Fix FORWARDING_NULL coverity
 *
 * Hydra_Software_Devel/236   9/24/09 4:01p tdo
 * SW7125-27 : Disable vecSwap for new chipsets with orthogonal VEC to
 * temporarily fix the issue with hdsd simul on 7125 and 7342
 *
 * Hydra_Software_Devel/235   8/25/09 7:25p albertl
 * SW7125-10: Initial 7125 support.
 *
 * Hydra_Software_Devel/234   8/21/09 2:37p tdo
 * PR57734: Add capability for display to handle DACs re-assignment
 *
 * Hydra_Software_Devel/233   8/12/09 11:56a pntruong
 * PR55225: Corrected the 656 input.
 *
 * Hydra_Software_Devel/232   8/5/09 9:48a pntruong
 * PR55812: Fixed bandgap initialization.
 *
 * Hydra_Software_Devel/231   6/25/09 12:10p rpan
 * PR56137, PR56138, PR56139, PR56166, PR56167, PR56168: Support for
 * various orthogonal VEC configurations.
 *
 * Hydra_Software_Devel/230   6/19/09 5:46p darnstein
 * PR55225: add support for 7342 chipset.
 *
 * Hydra_Software_Devel/229   6/18/09 5:52p syang
 * PR 55812: add 7550 support
 *
 * Hydra_Software_Devel/228   6/11/09 4:02p darnstein
 * PR55219: fill in some details for 7340 chipset.
 *
 * Hydra_Software_Devel/227   6/5/09 3:08p pntruong
 * PR54615: [M+T][LCD][VIDEO] Pink gargabe blinks on DTV.  Ensured that
 * the window shutdown process go thru upon destroy.
 *
 * Hydra_Software_Devel/226   4/17/09 11:56a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/225   3/16/09 10:42p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/5   3/13/09 5:22p tdo
 * PR45785, PR45789: merge from main branch on 3/13/09
 *
 * Hydra_Software_Devel/224   3/13/09 3:13p albertl
 * PR51648: Added debug messages showing default bandgap settings.
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/18/09 11:55a syang
 * PR 45785, PR 45789: merge from main branch on 2/18/09
 *
 * Hydra_Software_Devel/223   2/17/09 2:33p rpan
 * PR52001: Added 7420 DAC connection state for power management.
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/11/09 11:30a tdo
 * PR 45785, PR 45789: merge from main branch on 2/11/09
 *
 * Hydra_Software_Devel/222   2/10/09 4:47p rpan
 * PR52001: Added 7420 DAC power management.
 *
 * Hydra_Software_Devel/7420_mcvp/2   1/29/09 4:29p syang
 * PR 45785, PR 45789: merge from main branch on 1/29/09
 *
 * Hydra_Software_Devel/221   1/27/09 8:57p tdo
 * PR51627: add VDC 7336 PI support
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:28p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/220   12/3/08 7:53p pntruong
 * PR45817: Added hddvi support.
 *
 * Hydra_Software_Devel/219   11/24/08 5:02p rpan
 * PR45804: Move VEC reset to display module.
 *
 * Hydra_Software_Devel/218   11/17/08 5:01p darnstein
 * PR45819: fix error in previous check-in (include file name).
 *
 * Hydra_Software_Devel/217   11/17/08 4:10p darnstein
 * PR45819: Update scratch register trickery to support ITU-R 656 output,
 * in addition to analog output.
 *
 * Hydra_Software_Devel/216   11/7/08 6:40p albertl
 * PR48740: Fixed #ifdef accidentally surrounding return statement.
 *
 * Hydra_Software_Devel/215   11/7/08 5:24p albertl
 * PR48740: Fixed compiler warnings when debug is turned off.
 *
 * Hydra_Software_Devel/214   10/29/08 3:59p darnstein
 * PR45819: Correct usage of scratch registers for VBI.
 *
 * Hydra_Software_Devel/213   10/9/08 5:27p syang
 * PR 46891: add _isr to the name of func used in _isr context
 *
 * Hydra_Software_Devel/212   9/29/08 1:32p yuxiaz
 * PR47370: Use BVDC_P_BUF_MSG for buffer related messages.
 *
 * Hydra_Software_Devel/211   9/29/08 12:56p pntruong
 * PR47072: Moving 3563 vdc dedicated branch.
 *
 * Hydra_Software_Devel/210   9/26/08 1:32p pntruong
 * PR46515: Adapted to new power management.
 *
 * Hydra_Software_Devel/209   9/22/08 9:39a syang
 * PR 41898:  again more error message for the  win apply time out case
 *
 * Hydra_Software_Devel/208   9/19/08 2:52p syang
 * PR 41898:  add more error message for the  win apply time out case
 *
 * Hydra_Software_Devel/207   9/16/08 10:48p pntruong
 * PR46118: Removed pointless code.
 *
 * Hydra_Software_Devel/206   9/15/08 9:16p pntruong
 * PR46514: Upon destroyed and disabled slot disable adc if not used.
 *
 * Hydra_Software_Devel/204   9/5/08 4:43p tdo
 * PR46484: Bringup appframework for7420
 *
 * Hydra_Software_Devel/203   9/4/08 8:29p pntruong
 * PR46502: Moving 7403 vdc dedicated branch.
 *
 * Hydra_Software_Devel/202   9/4/08 12:33p tdo
 * PR43508, PR43509: Fix initial IREF_ADJ value for TDAC and QDAC for 7400
 *
 * Hydra_Software_Devel/201   8/29/08 2:25p darnstein
 * PR46118: Adapt to pointless name changes in VBI_ENC registers.
 *
 * Hydra_Software_Devel/200   7/23/08 7:26p tdo
 * PR43508, PR43509: Fix compiling errors
 *
 * Hydra_Software_Devel/199   7/23/08 7:00p tdo
 * PR43508, PR43509: Mapping individual bandgap adjustment for each DAC
 *
 * Hydra_Software_Devel/198   6/16/08 2:45p darnstein
 * PR43509: Add more generic video DAC adjustment defaults: by DAC
 * version.
 *
 * Hydra_Software_Devel/197   6/16/08 2:29p darnstein
 * PR43509: Provide Video DAC bandgap adjustment option for 7325 and 7335
 * chips.
 *
 * Hydra_Software_Devel/196   6/16/08 2:16p darnstein
 * PR43509: Provide adjustments to DAC bandgap for 3548 and 3556.
 *
 * Hydra_Software_Devel/195   6/16/08 1:46p darnstein
 * PR43509: Add bandgap adjust default for 7405 chipset.
 *
 * Hydra_Software_Devel/194   6/16/08 1:28p darnstein
 * PR43509: Provide separate adjustments to TDAC and QDAC bandgap
 * settings.
 *
 * Hydra_Software_Devel/193   6/16/08 1:10p darnstein
 * PR43509: Provide separate adjustments to TDAC and QDAC bandgap
 * settings.
 *
 * Hydra_Software_Devel/192   6/13/08 8:39p darnstein
 * PR43509: Update last checkin for 3563.
 *
 * Hydra_Software_Devel/191   6/13/08 8:27p darnstein
 * PR43509: I got confused with a register name.
 *
 * Hydra_Software_Devel/190   6/13/08 8:13p darnstein
 * PR43509: Switch to scheme: specify default bandgap adjustment by
 * chipset.
 *
 * Hydra_Software_Devel/189   6/13/08 7:50p darnstein
 * PR43509: fix a numeric error in last check-in.
 *
 * Hydra_Software_Devel/188   6/13/08 6:05p darnstein
 * PR43509: Video DAC bandgap adjustment can be made using new member of
 * struct BVDC_Settings.
 *
 * Hydra_Software_Devel/187   6/11/08 10:27a tdo
 * PR43119: Use atomic register access
 *
 * Hydra_Software_Devel/186   6/7/08 8:25p albertl
 * PR39336, PR39163:  Fixed bugs in second compositor support.  Added
 * dirty bits mechanism to compositor for colorclip.
 *
 * Hydra_Software_Devel/185   5/2/08 4:00p rpan
 * PR39421: Set 3548/3556 'cmpb' to true.
 *
 * Hydra_Software_Devel/184   3/25/08 3:17p syang
 * PR 40431: add complete assert for critical section protection among src
 * _isr, dsp _isr, and ApplyChanges
 *
 * Hydra_Software_Devel/183   3/7/08 9:13p pntruong
 * PR37836, PR39921: Added more helpful info for out of vdc heap to ease
 * debugging.
 *
 * Hydra_Software_Devel/182   3/5/08 1:41p tdo
 * PR39417: VDC PI support 3556
 *
 * Hydra_Software_Devel/181   2/21/08 4:59p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/180   2/13/08 3:10p pntruong
 * PR39421: Removed unused code to improve readability.
 *
 * Hydra_Software_Devel/179   2/5/08 2:42p yuxiaz
 * PR34712: Added H.264 support through MPEG feeder on 3548.
 *
 * Hydra_Software_Devel/178   11/19/07 3:49p tdo
 * PR36898: Add VDC PI support for 7335
 *
 * Hydra_Software_Devel/177   10/30/07 7:08p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/176   10/23/07 11:14a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/3   10/22/07 5:15p yuxiaz
 * PR29569: More FGT work.
 *
 * Hydra_Software_Devel/PR29569/2   10/19/07 11:30a yuxiaz
 * PR29569: Merge from mainline.
 *
 * Hydra_Software_Devel/175   10/12/07 10:05a pntruong
 * PR36058: Fixed compositor/display coupling in custom display setup.
 *
 * Hydra_Software_Devel/174   10/11/07 6:06p syang
 * PR 35036: coverity fix
 *
 * Hydra_Software_Devel/173   10/8/07 2:35p pntruong
 * PR34855: Initial VDC bringup.
 *
 * Hydra_Software_Devel/172   9/21/07 4:35p yuxiaz
 * PR34523: Add 7325 support for VDC.
 *
 * Hydra_Software_Devel/171   9/4/07 5:23p jessem
 * PR 34590: Changed s_stDefaultSettings' pixel format to use
 * BVDC_P_CAP_PIXEL_FORMAT_8BIT422 instead.
 *
 * Hydra_Software_Devel/170   8/7/07 4:21p yuxiaz
 * PR27644: BVDC_P_CheckDefSettings only checks when the buffer count is
 * not zero.
 *
 * Hydra_Software_Devel/169   7/23/07 10:13a pntruong
 * PR31869: [VDEC] snow noise blink between normal snow noise and black
 * screen.
 *
 * Hydra_Software_Devel/168   4/24/07 1:45p yuxiaz
 * PR27644: Added 7405 support.
 *
 * Hydra_Software_Devel/167   4/2/07 6:10p darnstein
 * PR29426: Activate 656 input for 7403-A0 chip. I separated the cases for
 * 7401 and 7403 chips.
 *
 * Hydra_Software_Devel/166   2/23/07 6:26p tdo
 * PR 27970: Share LPB and FCH if source is the same to eliminate the out
 * of resource error when number of LPB and FCH are limited
 *
 * Hydra_Software_Devel/165   2/16/07 9:37a pntruong
 * PR15284, PR27951: Graphics shimmering on HD path when video is scaled
 * down (PIG).  HW fixed.  Removed software work-around to avoid bvb
 * error interrupts from window surface.  Make bvdc dbg more portable.
 *
 * Hydra_Software_Devel/164   1/24/07 9:04p albertl
 * PR22237:  Updated BMEM calls to use new BMEM_Heap functions.
 *
 * Hydra_Software_Devel/163   1/22/07 11:12a pntruong
 * PR22579: Bringup of HD_DVI input (dual core).  Redo hddvi code.
 *
 * Hydra_Software_Devel/162   1/9/07 4:45p tdo
 * PR 26924: Add support for 7403 VCXO so that it won't be reset by VEC
 * core reset
 *
 * Hydra_Software_Devel/161   1/8/07 10:59p pntruong
 * PR22577: Initial vdc bringup.  Added flag to indicate if mad reset is
 * supported.
 *
 * Hydra_Software_Devel/160   12/18/06 11:17p pntruong
 * PR22577: Merged back to mainline.
 *
 ***************************************************************************/
#include "bstd.h"                /* standard types */
#include "bdbg.h"                /* Dbglib */
#include "bkni.h"                /* malloc */
#include "btmr.h"                /* timer */

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

/* Note: Tricky here!  bavc.h needs bchp_gfd_x.h defininitions.
 * The check here is to see if chips has more than one gfx feeder. */
#include "bchp_gfd_0.h"
#include "bchp_gfd_1.h"

#include "bvdc.h"                /* Video display */
#include "bvdc_priv.h"           /* VDC internal data structures */
#include "bvdc_common_priv.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_display_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_bufferheap_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_fgt_priv.h"
#include "bvdc_window_priv.h"  /* only for err msg as time out */
#include "bvdc_display_priv.h"

#include "bchp_fmisc.h"
#include "bchp_mmisc.h"
#include "bchp_bmisc.h"
#include "bchp_timer.h"

#if BVDC_P_SUPPORT_DMISC
#include "bchp_dmisc.h"
#endif


/* bDisableComponentVDEC mode registers */
#if (BVDC_P_SUPPORT_VDEC)
#include "bchp_ifd_top.h"
#include "bvdc_vdec_priv.h"  /* power up/down, and reset */
#endif

BDBG_MODULE(BVDC);
BDBG_OBJECT_ID(BVDC_VDC);

/* This table used to indicate which DACs belong to each group. 0 for unused DAC */
static const uint32_t s_aulDacGrouping[BVDC_MAX_DACS] =
{
#if (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_0)
	1, 1, 1, 1, 0, 0, 0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_1)
    1, 1, 1, 2, 2, 2, 0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_2)
	/* TODO: Need to confirm if TDAC = DAC[0-2] and QDAC = DAC[3-6] or
	   TDAC = DAC[4-6] and QDAC = DAC[0-3] */
	1, 1, 1, 2, 2, 2, 2
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_3)
	1, 1, 1, 2, 2, 2, 0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_4)
	1, 1, 1, 2, 2, 2, 0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_5)
	1, 1, 1, 0, 0, 0, 0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_6)
	1, 1, 1, 2, 2, 2, 2
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_7)
	1, 1, 1, 2, 2, 2, 0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_8)
	1, 1, 1, 1, 0, 0, 0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_9)
	1, 1, 1, 1, 0, 0, 0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_10)
	1, 1, 1, 1, 0, 0, 0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_11)
	1, 1, 1, 1, 0, 0, 0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_12)
	1, 1, 1, 1, 0, 0, 0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_13)
	1, 1, 1, 1, 0, 0, 0
#else
#error "Unknown chip!  Not yet supported in VDC."
#endif
};

/* Default settings. */
static const BVDC_Settings s_stDefaultSettings =
{
	BFMT_VideoFmt_e1080i,
	BAVC_FrameRateCode_e59_94,             /* Most HDMI monitors support 60Hz */
	BAVC_MatrixCoefficients_eItu_R_BT_709, /* default HD color matrix */
	BAVC_MatrixCoefficients_eSmpte_170M,   /* default SD color matrix */
	true,                                  /* VEC swap, cmp_0 -> vec_1 */
	false,                                 /* do not support FGT */

	/* Memory controller setttins. */
	{
		/* Double HD Buffer settings */
		BVDC_P_MAX_2HD_BUFFER_COUNT,           /* default 2HD buffer count */
		BVDC_P_CAP_PIXEL_FORMAT_8BIT422,       /* default capture Pixel format */
		BFMT_VideoFmt_e1080p_30Hz,             /* 2HD buffer is 1080p */
		0,                                     /* no additional lines for 2HD buffer */
		BVDC_P_DEFAULT_2HD_PIP_BUFFER_COUNT,   /* default 1/4 2HD buffer count */

		/* HD buffer settings */
		BVDC_P_MAX_HD_BUFFER_COUNT,            /* default HD buffer count */
		BVDC_P_CAP_PIXEL_FORMAT_8BIT422,                 /* default capture Pixel format */
		BFMT_VideoFmt_e1080i,                  /* HD buffer is 1080i */
		0,                                     /* no additional lines for HD buffer */
		BVDC_P_DEFAULT_HD_PIP_BUFFER_COUNT,    /* default 1/4 HD buffer count */

		/* SD buffer settings */
		BVDC_P_MAX_SD_BUFFER_COUNT,            /* default SD buffer count */
		BVDC_P_CAP_PIXEL_FORMAT_8BIT422,       /* default capture Pixel format */
		BFMT_VideoFmt_ePAL_G,                  /* default SD buffer is PAL */
		0,                                     /* no additional lines for SD buffer */
		BVDC_P_DEFAULT_SD_PIP_BUFFER_COUNT     /* default 1/4 SD buffer count */
	},

	/* New Video DAC bandgap adjust */
	{
#if (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_0)
		BCHP_MISC_QDAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_QDAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_QDAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_QDAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		0,
		0,
		0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_1)
		BCHP_MISC_TDAC0_CTRL_REG_BG_PTATADJ_NORM,
		BCHP_MISC_TDAC0_CTRL_REG_BG_PTATADJ_NORM,
		BCHP_MISC_TDAC0_CTRL_REG_BG_PTATADJ_NORM,
		BCHP_MISC_TDAC1_CTRL_REG_BG_PTATADJ_NORM,
		BCHP_MISC_TDAC1_CTRL_REG_BG_PTATADJ_NORM,
		BCHP_MISC_TDAC1_CTRL_REG_BG_PTATADJ_NORM,
		0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_2)
	/* TODO: Need to confirm if TDAC = DAC[0-2] and QDAC = DAC[3-6] or
	   TDAC = DAC[4-6] and QDAC = DAC[0-3] */
		BCHP_MISC_TDAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_QDAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_QDAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_QDAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_QDAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_3)
		BCHP_MISC_TDAC0_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC0_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC0_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC1_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC1_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC1_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_4)
		BCHP_MISC_TDAC0_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC0_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC0_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC1_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC1_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_TDAC1_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_5)
		BCHP_MISC_DAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_REG_IREF_ADJ_TWENTY_SIX,
		0,
		0,
		0,
		0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_6)
		BCHP_MISC_DAC_BG_CTRL_0_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_0_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_0_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_1_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_1_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_1_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_1_IREF_ADJ_TWENTY_SIX
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_7)
		BCHP_MISC_DAC_BG_CTRL_0_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_0_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_0_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_1_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_1_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_1_IREF_ADJ_TWENTY_SIX,
		0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_8)
		BCHP_MISC_DAC_BG_CTRL_0_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_0_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_0_IREF_ADJ_TWENTY_SIX,
		BCHP_MISC_DAC_BG_CTRL_0_IREF_ADJ_TWENTY_SIX,
		0,
		0,
		0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_9)
		244,
		244,
		244,
		244,
		0,
		0,
		0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_10)
		244,
		244,
		244,
		244,
		0,
		0,
		0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_11)
		244,
		244,
		244,
		244,
		0,
		0,
		0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_12)
		244,
		244,
		244,
		244,
		0,
		0,
		0
#elif (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_13)
		244,
		244,
		244,
		244,
		0,
		0,
		0
#else
#error "Unknown chip!  Not yet supported in VDC."
#endif
	},
	BVDC_Mode_eAuto,
	false,
	false,
	false
};

/* Here is a little consistency check */
#if (BVDC_P_SUPPORT_TDAC_VER == BVDC_P_SUPPORT_TDAC_VER_1)
	#if (BCHP_MISC_TDAC0_CTRL_REG_BG_PTATADJ_NORM != \
		 BCHP_MISC_TDAC0_CTRL_REG_BG_CTATADJ_NORM)
		#error bandgap constants not equal
	#endif
#endif

/* Available features
 * INDEX: by compositor id, window id source id */
static const BVDC_P_Features s_VdcFeatures =
{
#if (BCHP_CHIP==7400)
	false,
	/* cmp0   cmp1   cmp2   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  true,  false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  false, false, false, false, false, false, true,  true,  true,  true,  true,  false, false, false, false, false, false, false, true,  true,  true,  true,  false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  false, false, false, false, false, false, false, false, false, false, true,  true,  true,  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false },
#elif (BCHP_CHIP==7420)
	false,
	/* cmp0   cmp1   cmp2   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  true,  false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  false, false, false, false, false, false, true,  true,  true,  true,  true,  false, false, false, false, true,  false, false, true,  true,  true,  true,  false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false },
#elif (BCHP_CHIP==7422) || (BCHP_CHIP==7425)
	false,
	/* cmp0   cmp1   cmp2   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  true,  true,  false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  true,  false, false, false, false, false, false, false, true,  true,  true,  true,  false, false, false, true,  false, false, true,  true,  true,  true,  true,  false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  true,  false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },
#elif (BCHP_CHIP==7435)
	false,
	/* cmp0   cmp1   cmp2   cmp3   cmp4   cmp5   cmp6  */
	{  true,  true,  true,  true,  true,  true,  false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  true,  true,  false, false, false, false, false, false, true,  true,  true,  true,  true,  true,  false, true,  false, false, true,  true,  true,  true,  true,  true,  false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  true,  true,  false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },
#elif (BCHP_CHIP==7405)
	false,
	/* cmp0   cmp1   cmpb   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  false, false, false, false, false, false, true,  false, true,  true,  false, false, false, false, false, false, false, false, true,  true,  true,  true,  false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  false, false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false },
#elif (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || (BCHP_CHIP==7336)
	false,
	/* cmp0   cmp1   cmpb   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  false, false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false },
#elif (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || (BCHP_CHIP==7550)
	false,
	/* cmp0   cmp1   cmpb   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false },
#elif (BCHP_CHIP==7358) || (BCHP_CHIP==7552) || (BCHP_CHIP==7360) || (BCHP_CHIP==7563)
	false,
	/* cmp0   cmp1   cmpb   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  false, false, false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },
#elif (BCHP_CHIP==7344) || (BCHP_CHIP==7346)
	false,
	/* cmp0   cmp1   cmpb   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  false, false, false, false, false, false, false, false, true, true,   false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  false, false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },
#elif (BCHP_CHIP==7231) && (BCHP_VER == BCHP_VER_A0)
	false,
	/* cmp0   cmp1   cmpb   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  false, false, false, false, false, false, false,  false, true, true,  false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  false, false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },
#elif (BCHP_CHIP==7231) || (BCHP_CHIP==7429)
	false,
	/* cmp0   cmp1   cmpb   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  false, false, false, false, false, false, false,  false, true, true,  false, false, false, false, false, true,  false, false, true,  true,  false, false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  false, false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },
#elif (BCHP_CHIP==7584)
	false,
	/* cmp0   cmp1   cmpb   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  false, false, false, false, false, false, false,  false, true, true,  false, false, false, false, false, false, false, false, true,  true,  true,  true,  false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  false, false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },
#elif (BCHP_CHIP==7125) || (BCHP_CHIP==7468) || (BCHP_CHIP==7408)
	false,
	/* cmp0   cmp1   cmpb   cmp3   cmp4   cmp5   cmp6 */
	{  true,  true,  false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false },
#elif (BCHP_CHIP==7445)
	false,
	/* cmp0   cmp1   cmp2   cmp3   cmp4   cmp5   cmp6  */
	{  true,  true,  true,  true,  true,  true,  true  },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  true,  true,  true , true,  false, false, false, false, true,  true,  true,  true,  true,  true,  true,  true,  false, false, true,  true,  true,  true,  true,  true,  true,  true  },

	/* mpg0   mpg1   mpg2   mpg3   mpg4   mpg5   vdec0  vdec1  656_0  656_1  gfx0   gfx1   gfx2   gfx3   gfx4   gfx5   gfx6   dvi0   dvi1   ds 0   vfd0   vfd1   vfd2   vfd3   vfd4   vfd5   vfd6   vfd7  */
	{  true,  true,  true,  true,  true,  true,  false, false, false, false, true,  false, false, false, false, false, false, false, false, false, true,  true,  false, false, false, false, false, false },
#else
#error "Unknown chip!  Not yet supported in VDC."
#endif
};


/***************************************************************************
 * This function does a soft-reset of each VDC modules.
 */
static void BVDC_P_PrintHeapInfo
	( const BVDC_Heap_Settings        *pHeap )
{
	const BFMT_VideoInfo *pFmtInfo;

	pFmtInfo = BFMT_GetVideoFormatInfoPtr(pHeap->eBufferFormat_2HD);
	if(pFmtInfo == NULL)
		return;
	BVDC_P_BUF_MSG(("--------2HD---------"));
	BVDC_P_BUF_MSG(("ulBufferCnt     = %d", pHeap->ulBufferCnt_2HD));
	BVDC_P_BUF_MSG(("ulBufferCnt_Pip = %d", pHeap->ulBufferCnt_2HD_Pip));
	BVDC_P_BUF_MSG(("eBufferFormat   = %s", pFmtInfo->pchFormatStr));
	BVDC_P_BUF_MSG(("ePixelFormat    = %s", BPXL_ConvertFmtToStr(pHeap->ePixelFormat_2HD)));

	pFmtInfo = BFMT_GetVideoFormatInfoPtr(pHeap->eBufferFormat_HD);
	if(pFmtInfo == NULL)
		return;
	BVDC_P_BUF_MSG(("---------HD---------"));
	BVDC_P_BUF_MSG(("ulBufferCnt     = %d", pHeap->ulBufferCnt_HD));
	BVDC_P_BUF_MSG(("ulBufferCnt_Pip = %d", pHeap->ulBufferCnt_HD_Pip));
	BVDC_P_BUF_MSG(("eBufferFormat   = %s", pFmtInfo->pchFormatStr));
	BVDC_P_BUF_MSG(("ePixelFormat    = %s", BPXL_ConvertFmtToStr(pHeap->ePixelFormat_HD)));

	pFmtInfo = BFMT_GetVideoFormatInfoPtr(pHeap->eBufferFormat_SD);
	if(pFmtInfo == NULL)
		return;
	BVDC_P_BUF_MSG(("---------SD---------"));
	BVDC_P_BUF_MSG(("ulBufferCnt     = %d", pHeap->ulBufferCnt_SD));
	BVDC_P_BUF_MSG(("ulBufferCnt_Pip = %d", pHeap->ulBufferCnt_SD_Pip));
	BVDC_P_BUF_MSG(("eBufferFormat   = %s", pFmtInfo->pchFormatStr));
	BVDC_P_BUF_MSG(("ePixelFormat    = %s", BPXL_ConvertFmtToStr(pHeap->ePixelFormat_SD)));

	return;
}


/***************************************************************************
 *
 */
static void BVDC_P_ResetBvn
	( BVDC_P_Context                  *pVdc )
{
	bool bFreeRun;
	uint32_t ulReg;

	/* Note we can not use the SUN_TOP_CTRL_SW_RESET, because it would
	 * also reset the RDC, and MEMC memory controller. */
#if (BVDC_P_SUPPORT_CLOCK_GATING)
	bFreeRun = false;
#else
	bFreeRun = true;
#endif

	/* Reset BVN front-end, middle-end, and back-end modules.
	 * To reset write a 1 to a bit, and then write a 0.*/
	/*---------------------------*/
	/* FRONT-END & MIDDLE BLOCKS */
	/*---------------------------*/
	ulReg = 0xffffffff;
	ulReg &= ~(
#if BVDC_P_SUPPORT_NEW_SW_INIT
	BCHP_MASK(FMISC_SW_INIT, RDC ));
	BREG_Write32(pVdc->hRegister, BCHP_FMISC_SW_INIT, ulReg);
	BREG_Write32(pVdc->hRegister, BCHP_FMISC_SW_INIT, 0);
#else
#if BVDC_P_SUPPORT_FMISC_PFRI
		BCHP_MASK(FMISC_SOFT_RESET, MEMC_PFRI )  |
#endif
#if BVDC_P_SUPPORT_FMISC_MEMC
		BCHP_MASK(FMISC_SOFT_RESET, MEMC_IOBUF ) |
		BCHP_MASK(FMISC_SOFT_RESET, MEMC_RBUS )  |
		BCHP_MASK(FMISC_SOFT_RESET, MEMC_CORE)   |
#endif
		BCHP_MASK(FMISC_SOFT_RESET, RDC ) );
	BREG_Write32(pVdc->hRegister, BCHP_FMISC_SOFT_RESET, ulReg);
	BREG_Write32(pVdc->hRegister, BCHP_FMISC_SOFT_RESET, 0);
#endif

#ifdef BCHP_FMISC_BVNF_CLOCK_CTRL
	BREG_Write32(pVdc->hRegister, BCHP_FMISC_BVNF_CLOCK_CTRL,
		BCHP_FIELD_DATA(FMISC_BVNF_CLOCK_CTRL, CLK_FREE_RUN_MODE, bFreeRun));
#endif

	/*---------------*/
	/* MAD BLOCKS    */
	/*---------------*/
#if BVDC_P_SUPPORT_DMISC
	ulReg = 0xffffffff;
	BREG_Write32(pVdc->hRegister, BCHP_DMISC_SOFT_RESET, ulReg);
	BREG_Write32(pVdc->hRegister, BCHP_DMISC_SOFT_RESET, 0);
#endif

#ifdef BCHP_DMISC_BVND_MAD_0_CLOCK_CTRL
	BREG_Write32(pVdc->hRegister, BCHP_DMISC_BVND_MAD_0_CLOCK_CTRL,
		BCHP_FIELD_DATA(DMISC_BVND_MAD_0_CLOCK_CTRL, CLK_FREE_RUN_MODE, bFreeRun));
#endif

	/*---------------*/
	/* MIDDLE BLOCKS */
	/*---------------*/
	ulReg = 0xffffffff;
#if BVDC_P_SUPPORT_NEW_SW_INIT
	BREG_Write32(pVdc->hRegister, BCHP_MMISC_SW_INIT, ulReg);
	BREG_Write32(pVdc->hRegister, BCHP_MMISC_SW_INIT, 0);
#else
	BREG_Write32(pVdc->hRegister, BCHP_MMISC_SOFT_RESET, ulReg);
	BREG_Write32(pVdc->hRegister, BCHP_MMISC_SOFT_RESET, 0);
#endif

#ifdef BCHP_MMISC_BVNM_CLOCK_CTRL
	BREG_Write32(pVdc->hRegister, BCHP_MMISC_BVNM_CLOCK_CTRL,
		BCHP_FIELD_DATA(MMISC_BVNM_CLOCK_CTRL, CLK_FREE_RUN_MODE, bFreeRun));
#endif

	/*------------------*/
	/* BACK-END BLOCKS */
	/*------------------*/
	ulReg = 0xffffffff;
#if BVDC_P_SUPPORT_NEW_SW_INIT
	BREG_Write32(pVdc->hRegister, BCHP_BMISC_SW_INIT, ulReg);
	BREG_Write32(pVdc->hRegister, BCHP_BMISC_SW_INIT, 0);
#else
	BREG_Write32(pVdc->hRegister, BCHP_BMISC_SOFT_RESET, ulReg);
	BREG_Write32(pVdc->hRegister, BCHP_BMISC_SOFT_RESET, 0);
#endif

#ifdef BCHP_BMISC_BVNB_CLOCK_CTRL
	BREG_Write32(pVdc->hRegister, BCHP_BMISC_BVNB_CLOCK_CTRL,
		BCHP_FIELD_DATA(BMISC_BVNB_CLOCK_CTRL, CLK_FREE_RUN_MODE, bFreeRun));
#endif

	return;
}


/***************************************************************************
 * This function does a soft-reset of each VDC modules.
 */
static void BVDC_P_SoftwareReset
	( BVDC_P_Context                  *pVdc )
{
	/* Reset all BVN modules in BVN_Front, BVN_Middle, BVN_Back */
	BVDC_P_ResetBvn(pVdc);

	/* Reset all Vec modules in Sundry, and initialize vec Misc regs. */
	BVDC_P_ResetVec(pVdc);

#if (BVDC_P_SUPPORT_VDEC)
	BVDC_P_Vdec_Reset(pVdc->hRegister);
#endif

	return;
}

/***************************************************************************
 * Check VDC default settings.
 *
 * 1) SD buffer format is SD
 * 2) HD buffer format is HD
 * 3) 2HD buffer format is  HD
 * 4) SD buffer < HD buffer
 * 5) HD buffer < 2HD buffer
 * 6) VBI lines
 *
 */
static BERR_Code BVDC_P_CheckDefSettings
	( const BVDC_Heap_Settings         *pHeapSettings )
{
	BERR_Code  eStatus = BERR_SUCCESS;
	uint32_t   ulWidth, ulHeight;
	uint32_t   ulSDBufSize, ulHDBufSize, ul2HDBufSize;
	const BVDC_P_FormatInfo  *pVdcFmt;

	/* 1) SD buffer format is SD */
	pVdcFmt = BVDC_P_GetFormatInfoPtr_isr(
		BFMT_GetVideoFormatInfoPtr(pHeapSettings->eBufferFormat_SD));
	if( (!pVdcFmt->bSd) &&
	    (pHeapSettings->ulBufferCnt_SD || pHeapSettings->ulBufferCnt_SD_Pip))
	{
		BDBG_ERR(("SD buffer format is not SD"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* 2) HD buffer format is HD */
	pVdcFmt = BVDC_P_GetFormatInfoPtr_isr(
		BFMT_GetVideoFormatInfoPtr(pHeapSettings->eBufferFormat_HD));
	if( (!pVdcFmt->bHd) &&
	    (pHeapSettings->ulBufferCnt_HD || pHeapSettings->ulBufferCnt_HD_Pip))
	{
		BDBG_ERR(("HD buffer format is not HD"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* 3) 2Hd buffer format is HD */
	pVdcFmt = BVDC_P_GetFormatInfoPtr_isr(
		BFMT_GetVideoFormatInfoPtr(pHeapSettings->eBufferFormat_2HD));
	if( (!pVdcFmt->bHd) &&
	    (pHeapSettings->ulBufferCnt_2HD || pHeapSettings->ulBufferCnt_2HD_Pip))
	{
		BDBG_ERR(("2HD buffer format is not HD"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Get SD, HD, 2HD buffer size */
	ulSDBufSize = BVDC_P_BufferHeap_GetHeapSize(
		BFMT_GetVideoFormatInfoPtr(pHeapSettings->eBufferFormat_SD),
		pHeapSettings->ePixelFormat_SD, 0, false, NULL, &ulWidth, &ulHeight);
	ulHDBufSize = BVDC_P_BufferHeap_GetHeapSize(
		BFMT_GetVideoFormatInfoPtr(pHeapSettings->eBufferFormat_HD),
		pHeapSettings->ePixelFormat_HD, 0, false, NULL, &ulWidth, &ulHeight);
	ul2HDBufSize = BVDC_P_BufferHeap_GetHeapSize(
		BFMT_GetVideoFormatInfoPtr(pHeapSettings->eBufferFormat_2HD),
		pHeapSettings->ePixelFormat_2HD, 0, false, NULL, &ulWidth, &ulHeight);

	/* 4) SD buffer < HD buffer */
	if( (!(ulSDBufSize < ulHDBufSize)) &&
	    (pHeapSettings->ulBufferCnt_SD || pHeapSettings->ulBufferCnt_SD_Pip) &&
	    (pHeapSettings->ulBufferCnt_HD || pHeapSettings->ulBufferCnt_HD_Pip))
	{
		BDBG_ERR(("HD buffer (0x%d) is not bigger than SD buffer (0x%x)",
			ulHDBufSize, ulSDBufSize));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* 5) HD buffer < 2HD buffer */
	if( (!(ulHDBufSize < ul2HDBufSize)) &&
	    (pHeapSettings->ulBufferCnt_HD || pHeapSettings->ulBufferCnt_HD_Pip) &&
	    (pHeapSettings->ulBufferCnt_2HD || pHeapSettings->ulBufferCnt_2HD_Pip))
	{
		BDBG_ERR(("2HD buffer is not bigger than HD buffer"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* 6) VBI lines */
	if((pHeapSettings->ulAdditionalLines_HD > BVDC_ADDITIONAL_VBI_LINES_MAX_HD)  ||
	   (pHeapSettings->ulAdditionalLines_2HD > BVDC_ADDITIONAL_VBI_LINES_MAX_HD) ||
	   (pHeapSettings->ulAdditionalLines_SD >
	   (VIDEO_FORMAT_IS_NTSC(pHeapSettings->eBufferFormat_SD) ?
	    BVDC_ADDITIONAL_VBI_LINES_MAX_NTSC : BVDC_ADDITIONAL_VBI_LINES_MAX_PAL)))
	{
		BDBG_ERR(("Default setting requests more vbi lines pass-through lines than max allowed"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	return eStatus;
}

/***************************************************************************
 * Check VDC DAC bandgap default settings.
 */
static BERR_Code BVDC_P_CheckBandgapDefSettings
	( const BVDC_Settings             *pDefSettings )
{
	BERR_Code  eStatus = BERR_SUCCESS;
	uint32_t   id;

	for(id = 0; id < BVDC_MAX_DACS; id++)
	{
		BDBG_MSG(("DAC %d BG setting = %d", id + 1, pDefSettings->aulDacBandGapAdjust[id]));
	}

	for(id = 1; id < BVDC_MAX_DACS; id++)
	{
		if(s_aulDacGrouping[id] != 0 &&
		   s_aulDacGrouping[id-1] == s_aulDacGrouping[id] &&
		   pDefSettings->aulDacBandGapAdjust[id-1] != pDefSettings->aulDacBandGapAdjust[id])
		{
			BDBG_ERR(("BG setting for DAC %d = %d should be same as DAC %d = %d",
				id, pDefSettings->aulDacBandGapAdjust[id],
				id - 1, pDefSettings->aulDacBandGapAdjust[id-1]));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}

	return eStatus;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_GetDefaultSettings
	( BVDC_Settings                   *pDefSettings )
{
	BDBG_ENTER(BVDC_GetDefaultSettings);

	if(pDefSettings)
	{
		*pDefSettings = s_stDefaultSettings;

#if BVDC_P_ORTHOGONAL_VEC
		/* Temporarily disable  bVecSwap for new chipsets with orthogonal VEC */
		/* so that DISP_0 (main display) can be created with CMP_0 because    */
		/* currently the HD_SRC is assumed to be with the VEC that connects   */
		/* to CMP_0.  This can be removed after the assumption is removed     */
		pDefSettings->bVecSwap = false;
#endif
	}

	BDBG_LEAVE(BVDC_GetDefaultSettings);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_GetMaxCompositorCount
	( const BVDC_Handle                hVdc,
	  uint32_t                        *pulCompositorCount )
{
	BDBG_ENTER(BVDC_GetMaxCompositorCount);
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	/* set return value */
	if(pulCompositorCount)
	{
		*pulCompositorCount = BVDC_P_MAX_COMPOSITOR_COUNT;
	}

	BDBG_LEAVE(BVDC_GetMaxCompositorCount);
	return BERR_SUCCESS;
}


/***************************************************************************
 * BVDC_Open()
 *
 */
BERR_Code BVDC_Open
	( BVDC_Handle                     *phVdc,
	  BCHP_Handle                      hChip,
	  BREG_Handle                      hRegister,
	  BMEM_Heap_Handle                 hMemory,
	  BINT_Handle                      hInterrupt,
	  BRDC_Handle                      hRdc,
	  BTMR_Handle                      hTmr,
	  const BVDC_Settings             *pDefSettings )
{
	BVDC_P_Context *pVdc = NULL;
	BERR_Code eStatus;
	BTMR_Settings sTmrSettings;
	uint32_t i;
	uint32_t ulCmpCount = 0;

	BDBG_ENTER(BVDC_Open);
	BDBG_ASSERT(phVdc);
	BDBG_ASSERT(hChip);
	BDBG_ASSERT(hRegister);
	BDBG_ASSERT(hMemory);
	BDBG_ASSERT(hInterrupt);
	BDBG_ASSERT(hRdc);

	/* The handle will be NULL if create fails. */
	*phVdc = NULL;

	/* check VDC settings */
	if(pDefSettings)
	{
		BVDC_P_PrintHeapInfo(&pDefSettings->stHeapSettings);
		eStatus = BERR_TRACE(BVDC_P_CheckDefSettings(&pDefSettings->stHeapSettings));
		if( eStatus != BERR_SUCCESS )
		{
			goto BVDC_Open_Done;
		}

		eStatus = BERR_TRACE(BVDC_P_CheckBandgapDefSettings(pDefSettings));
		if( eStatus != BERR_SUCCESS )
		{
			goto BVDC_Open_Done;
		}
	}

	/* (1) Alloc the main VDC context. */
	pVdc = (BVDC_P_Context*)(BKNI_Malloc(sizeof(BVDC_P_Context)));
	if(NULL == pVdc)
	{
		eStatus = BERR_OUT_OF_SYSTEM_MEMORY;
		goto BVDC_Open_Done;
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pVdc, 0x0, sizeof(BVDC_P_Context));
	BDBG_OBJECT_SET(pVdc, BVDC_VDC);

	/* Store the hChip, hRegister, hMemory, and hRdc for later use. */
	pVdc->hChip      = hChip;
	pVdc->hRegister  = hRegister;
	pVdc->hMemory    = hMemory;
	pVdc->hInterrupt = hInterrupt;
	pVdc->hRdc       = hRdc;
	pVdc->hTmr       = hTmr;

	/* (1.1) Power managment */
#ifdef BCHP_PWR_RESOURCE_VDC
	BCHP_PWR_AcquireResource(pVdc->hChip, BCHP_PWR_RESOURCE_VDC);
#endif

	/* (2) Initalize and start timer */
	if(!pVdc->hTimer)
	{
		BTMR_GetDefaultTimerSettings(&sTmrSettings);
		sTmrSettings.type = BTMR_Type_eSharedFreeRun;
		sTmrSettings.cb_isr = NULL;
		sTmrSettings.pParm1 = NULL;
		sTmrSettings.parm2 = 0;
		sTmrSettings.exclusive = true;

		eStatus = BTMR_CreateTimer(hTmr, &pVdc->hTimer, &sTmrSettings);
		if (eStatus != BERR_SUCCESS)
		{
			goto BVDC_Open_Done;
		}
	}

	/* Take in feature, this should be the centralize place to discover about
	 * chip information and features. */
	pVdc->pFeatures = &s_VdcFeatures;

	/* Take in default settings. */
	pVdc->stSettings = (pDefSettings) ? *pDefSettings : s_stDefaultSettings;

	/* Do we need to swap the CMP/VEC. */
	pVdc->bSwapVec = (
		(pVdc->stSettings.bVecSwap) &&
		(pVdc->pFeatures->abAvailCmp[BVDC_CompositorId_eCompositor1]));

	/* (3) Allocate Buffer Heap (VDC Internal) */
	BVDC_P_BufferHeap_Create(pVdc, &pVdc->hBufferHeap, pVdc->hMemory,
		&pVdc->stSettings.stHeapSettings);

	/* (4-1) create FGT noise pattern table */
	if(pVdc->stSettings.bEnableFgt)
	{
		uint8_t  *pucFgtPatTblCachedAddr;

		pVdc->pucFgtPatTblAddr = BMEM_Heap_AllocAligned(hMemory,
			BVDC_P_FGT_NOISE_PIXEL_PATTERN_TABLE_SIZE,
			BVDC_P_HEAP_MEMORY_ALIGNMENT, 0);
		if(!pVdc->pucFgtPatTblAddr)
		{
			BDBG_ERR(("Not enough device memory"));
			BDBG_ASSERT(0);
			return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
		}

		/* Convert to cached address */
		eStatus = BMEM_ConvertAddressToCached(hMemory, pVdc->pucFgtPatTblAddr,
			(void**)((void *)&pucFgtPatTblCachedAddr));
		if (BERR_SUCCESS != eStatus)
		{
			BDBG_ERR(( "Failed to convert address 0x%x to cached", pVdc->pucFgtPatTblAddr));
			BMEM_Free(hMemory, pVdc->pucFgtPatTblAddr);
			return BERR_TRACE(eStatus);
		}

		/* load table */
		BVDC_P_Fgt_LoadPatTable(pucFgtPatTblCachedAddr);

		BMEM_FlushCache(hMemory, pucFgtPatTblCachedAddr,
			BVDC_P_FGT_NOISE_PIXEL_PATTERN_TABLE_SIZE);

	}

	/* (4-2) Create resource */
	BVDC_P_Resource_Create(&pVdc->hResource, pVdc);

	/* (5) Alloc context for sources.
	 * should be done before BVDC_P_Window_Create because its hPlayback->ulTimestampRegAddr
	 * shares the scratch register allocated for VFD gfx source */
	for(i  = 0; i < BVDC_P_MAX_SOURCE_COUNT; i++)
	{
		if(pVdc->pFeatures->abAvailSrc[i])
		{
			/* SW7435-80: Skip these sources to save RUL memory */
			if((BVDC_P_SRC_IS_HDDVI(i) && pVdc->stSettings.bDisableHddviInput) ||
			   (BVDC_P_SRC_IS_ITU656(i) && pVdc->stSettings.bDisable656Input))
			{
				BDBG_MSG(("User disabled SRC[%d] to save memory!", i));
				continue;
			}
			BVDC_P_Source_Create(pVdc, &pVdc->ahSource[i], (BAVC_SourceId)i, pVdc->hResource,
				pVdc->pFeatures->ab3dSrc[i]);
		}
	}

	/* (6) Alloc context for compositor/display. */
	for(i  = 0; i < BVDC_P_MAX_COMPOSITOR_COUNT; i++)
	{
		if(pVdc->pFeatures->abAvailCmp[i])
		{
			BVDC_P_Compositor_Create(pVdc, &pVdc->ahCompositor[i], (BVDC_CompositorId)i);
			pVdc->ahCompositor[i]->ulCmpCount = ulCmpCount; /* count number of the compositors */
			BVDC_P_Display_Create(pVdc, &pVdc->ahDisplay[i], (BVDC_DisplayId)i);
			pVdc->ahDisplay[i]->ulDisplayCnt  = ulCmpCount++;/* count number of displays */
		}
	}

	/* (7) Put Hardware into a known state. */
	BVDC_P_SoftwareReset((BVDC_Handle)pVdc);

	/* (8) allocate drain buffer (2 pixels deep) for mosaic mode support */
#if BVDC_P_SUPPORT_MOSAIC_MODE
	pVdc->pvVdcNullBufAddr = BMEM_Heap_AllocAligned(pVdc->hMemory,
		16, /* 2 pixels wide, in case 10-bit 4:2:2 capture rounding; */
		4,  /* 16 bytes aligned for capture engine */
		0   /* no boundary */);
	BERR_TRACE(BMEM_Heap_ConvertAddressToOffset(pVdc->hMemory, pVdc->pvVdcNullBufAddr,
		&pVdc->ulVdcNullBufOffset));
#endif

	/* (9) Initialize all DACs to unused */
#if BVDC_P_ORTHOGONAL_VEC
	for (i = 0; i < BVDC_P_MAX_DACS; i++ )
	{
		pVdc->aDacOutput[i] = BVDC_DacOutput_eUnused;
	}
	pVdc->aulDacGrouping = s_aulDacGrouping;
	/* Default Auto = Off */
	pVdc->bDacDetectionEnable = (pVdc->stSettings.eDacDetection == BVDC_Mode_eOn) ? true : false;
#endif /* BVDC_P_ORTHOGONAL_VEC */

	/* All done. now return the new fresh context to user. */
	*phVdc = (BVDC_Handle)pVdc;

BVDC_Open_Done:
	BDBG_LEAVE(BVDC_Open);

	if ((BERR_SUCCESS != eStatus) && (NULL != pVdc))
	{
#ifdef BCHP_PWR_RESOURCE_VDC
		BCHP_PWR_ReleaseResource(pVdc->hChip, BCHP_PWR_RESOURCE_VDC);
#endif
		BDBG_OBJECT_DESTROY(pVdc, BVDC_VDC);
		BKNI_Free((void*)pVdc);
	}

	return BERR_TRACE(eStatus);
}


/***************************************************************************
 *
 */
BERR_Code BVDC_Close
	( BVDC_Handle                      hVdc )
{
	uint32_t i;
	BERR_Code eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_Close);

	/* Return if trying to free a NULL handle. */
	if(!hVdc)
	{
		goto done;
	}

	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	/* [8] free drain buffer */
#if BVDC_P_SUPPORT_MOSAIC_MODE
	BMEM_Heap_Free(hVdc->hMemory, hVdc->pvVdcNullBufAddr);
#endif

	/* [7] Make sure we disable capture before we exit so that it would
	 * not write to memory that potential contain heap bookeeping
	 * next time we create. */
	BVDC_P_ResetBvn(hVdc);

#if (BVDC_P_SUPPORT_VDEC)
	BVDC_P_Vdec_Reset(hVdc->hRegister);
#endif

	/* [6] Free compositor handles . */
	for(i  = 0; i < BVDC_P_MAX_COMPOSITOR_COUNT; i++)
	{
		if(hVdc->pFeatures->abAvailCmp[i])
		{
			BVDC_P_Display_Destroy(hVdc->ahDisplay[i]);
			BVDC_P_Compositor_Destroy(hVdc->ahCompositor[i]);
		}
	}

	/* [5] Free sources handles. */
	for(i  = 0; i < BVDC_P_MAX_SOURCE_COUNT; i++)
	{
		if(hVdc->pFeatures->abAvailSrc[i])
		{
			/* SW7435-80: Skip these sources to save RUL memory */
			if((BVDC_P_SRC_IS_HDDVI(i) && hVdc->stSettings.bDisableHddviInput) ||
			   (BVDC_P_SRC_IS_ITU656(i) && hVdc->stSettings.bDisable656Input))
			{
				BDBG_MSG(("User disabled SRC[%d] to save memory!", i));
				continue;
			}
			BVDC_P_Source_Destroy(hVdc->ahSource[i]);
		}
	}

	/* [4-2] destroy resource */
	BVDC_P_Resource_Destroy(hVdc->hResource);

	/* [4-1] Free FGT memory */
	if(hVdc->stSettings.bEnableFgt)
	{
		BMEM_Free(hVdc->hMemory, hVdc->pucFgtPatTblAddr);
	}

	/* [3] Release Buffer Heap */
	BVDC_P_BufferHeap_Destroy(hVdc->hBufferHeap);

	/* [2] Destroy Timer */
	if(hVdc->hTimer)
	{
		eStatus = BTMR_DestroyTimer(hVdc->hTimer);
		if (eStatus != BERR_SUCCESS)
		{
			return BERR_TRACE(eStatus);
		}
		hVdc->hTimer = NULL;
	}

	/* [1.1] Power managment */
#ifdef BCHP_PWR_RESOURCE_VDC
	BCHP_PWR_ReleaseResource(hVdc->hChip, BCHP_PWR_RESOURCE_VDC);
#endif

	/* [1] Release main context.   User will be responsible for destroying
	 * compositors, sources, windows, and displays prior. */
	BDBG_OBJECT_DESTROY(hVdc, BVDC_VDC);
	BKNI_Free((void*)hVdc);

done:
	BDBG_LEAVE(BVDC_Close);
	return eStatus;
}

/***************************************************************************
 *
 */
void BVDC_GetDefaultStandbySettings
	( BVDC_StandbySettings            *pStandbypSettings )
{
	BSTD_UNUSED(pStandbypSettings);
	return;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Standby
	( BVDC_Handle                      hVdc,
	  const BVDC_StandbySettings      *pStandbypSettings )
{
	uint32_t i, j;
	bool bActive = false;
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	BSTD_UNUSED(pStandbypSettings);

	for(i = 0; i < BVDC_P_MAX_SOURCE_COUNT && !bActive; i++)
	{
		bActive |= BVDC_P_STATE_IS_ACTIVE(hVdc->ahSource[i]);
		BDBG_MSG(("hVdc->ahSource[%d]=%d", i, BVDC_P_STATE_IS_ACTIVE(hVdc->ahSource[i])));
	}

	for(i = 0; i < BVDC_P_MAX_DISPLAY_COUNT && !bActive; i++)
	{
		bActive |= BVDC_P_STATE_IS_ACTIVE(hVdc->ahDisplay[i]);
		BDBG_MSG(("hVdc->ahDisplay[%d]=%d", i, BVDC_P_STATE_IS_ACTIVE(hVdc->ahDisplay[i])));
	}

	for(i = 0; i < BVDC_P_MAX_COMPOSITOR_COUNT && !bActive; i++)
	{
		bActive |= BVDC_P_STATE_IS_ACTIVE(hVdc->ahCompositor[i]);
		BDBG_MSG(("hVdc->ahCompositor[%d]=%d", i, BVDC_P_STATE_IS_ACTIVE(hVdc->ahCompositor[i])));

		if(hVdc->pFeatures->abAvailCmp[i])
		{
			for(j = 0; j < BVDC_P_MAX_WINDOW_COUNT && !bActive; j++)
			{
				bActive |= BVDC_P_STATE_IS_ACTIVE(hVdc->ahCompositor[i]->ahWindow[j]);
				BDBG_MSG(("hVdc->ahCompositor[%d]->ahWindow[%d]=%d",
					i, j, BVDC_P_STATE_IS_ACTIVE(hVdc->ahCompositor[i]->ahWindow[j])));
			}
		}
	}

	BDBG_MSG(("Power Standby %s ready!", !bActive ? "is" : "is not"));

#ifdef BCHP_PWR_RESOURCE_VDC
	if(bActive) {
		BDBG_ERR(("Cannot enter standby due to VDC in use"));
		return BERR_UNKNOWN;
	}
	/* if we get to this point, then nothing is in use and we can power down */
	if(!hVdc->bStandby)
	{
		BERR_Code eStatus = BERR_SUCCESS;

		/* Destroy Timer */
		if(hVdc->hTimer)
		{
			eStatus = BTMR_DestroyTimer(hVdc->hTimer);
			if (eStatus != BERR_SUCCESS)
			{
				return BERR_TRACE(eStatus);
			}
			hVdc->hTimer = NULL;
		}

		BCHP_PWR_ReleaseResource(hVdc->hChip, BCHP_PWR_RESOURCE_VDC);
		hVdc->bStandby = true;
		BDBG_MSG(("Entering standby mode!"));
	}
#endif

	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BVDC_Resume
	( BVDC_Handle                      hVdc )
{
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

#ifdef BCHP_PWR_RESOURCE_VDC
	if(!hVdc->bStandby)
	{
		BDBG_ERR(("Not in standby"));
		return BERR_UNKNOWN;
	}
	else
	{
		BCHP_PWR_AcquireResource(hVdc->hChip, BCHP_PWR_RESOURCE_VDC);

		if(!hVdc->hTimer)
		{
			BERR_Code eStatus = BERR_SUCCESS;
			BTMR_Settings sTmrSettings;

			/* Initalize and start timer */
			BTMR_GetDefaultTimerSettings(&sTmrSettings);
			sTmrSettings.type = BTMR_Type_eSharedFreeRun;
			sTmrSettings.cb_isr = NULL;
			sTmrSettings.pParm1 = NULL;
			sTmrSettings.parm2 = 0;
			sTmrSettings.exclusive = true;

			eStatus = BTMR_CreateTimer(hVdc->hTmr, &hVdc->hTimer, &sTmrSettings);
			if (eStatus != BERR_SUCCESS)
			{
				return BERR_TRACE(eStatus);
			}
		}

		BVDC_P_SoftwareReset(hVdc);
		hVdc->bStandby = false;

		BDBG_MSG(("Leaving standby mode!"));
	}
#endif

	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
#if (BDBG_DEBUG_BUILD)
static void BVDC_P_CheckDisplayAlignAdjustedStatus
	( BVDC_Handle                      hVdc )
{
	uint32_t k;

	BDBG_ENTER(BVDC_P_CheckDisplayAlignAdjustStatus);
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	for (k = 0; k < BVDC_P_MAX_COMPOSITOR_COUNT; k++)
	{
		/* Bypass the inactive ones. */
		if(!hVdc->ahDisplay[k] || !hVdc->ahDisplay[k]->hCompositor)
		{
			continue;
		}

		if (hVdc->ahDisplay[k]->bAlignAdjusting)
		{
			BDBG_ERR(("Display %d is in the process of VEC alignment", k));
		}
	}

	BDBG_ENTER(BVDC_P_CheckDisplayAlignAdjustStatus);
	return;
}
#endif

/***************************************************************************
 *
 */
static BERR_Code BVDC_P_CheckApplyChangesStatus
	( BVDC_Handle                      hVdc )
{
	uint32_t i, j;
	BERR_Code eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_CheckApplyChangesStatus);
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	/* Active various debug message at runtime */
#if (BDBG_DEBUG_BUILD)
	{
		uint32_t ulReg = BREG_Read32(hVdc->hRegister, BCHP_BMISC_SCRATCH_0);
		hVdc->bForcePrint = (ulReg & (1 << 0)); /* BMISC_SCRATCH_0[00:00] */
		                                        /* BMISC_SCRATCH_0[31:01] - avail */
	}
#endif

	for(i = 0; i < BVDC_P_MAX_COMPOSITOR_COUNT; i++)
	{
		/* Bypass the inactive ones. */
		if(!hVdc->ahDisplay[i] || !hVdc->ahDisplay[i]->hCompositor)
		{
			continue;
		}

		BDBG_OBJECT_ASSERT(hVdc->ahDisplay[i], BVDC_DSP);
		BDBG_OBJECT_ASSERT(hVdc->ahDisplay[i]->hCompositor, BVDC_CMP);

		/* Wait for compositor/display to be applied/destroyed. */
		if(BVDC_P_STATE_IS_ACTIVE(hVdc->ahDisplay[i]) &&
		   BVDC_P_STATE_IS_ACTIVE(hVdc->ahDisplay[i]->hCompositor) &&
		   hVdc->ahDisplay[i]->bSetEventPending)
		{
			BDBG_MSG(("Waiting for Display%d to be applied", hVdc->ahDisplay[i]->eId));
			eStatus = BKNI_WaitForEvent(hVdc->ahDisplay[i]->hAppliedDoneEvent,
				BVDC_P_MAX_VEC_APPLY_WAIT_TIMEOUT);
			if(BERR_TIMEOUT == eStatus)
			{
				BDBG_ERR(("Display%d apply times out", hVdc->ahDisplay[i]->eId));
				return BERR_TRACE(eStatus);
			}
		}

		/* Wait for window to applied/destroy */
		for(j = 0; j < BVDC_P_MAX_WINDOW_COUNT; j++)
		{
			if((BVDC_P_STATE_IS_SHUTDOWNPENDING(hVdc->ahDisplay[i]->hCompositor->ahWindow[j]) ||
			    BVDC_P_STATE_IS_SHUTDOWNRUL(hVdc->ahDisplay[i]->hCompositor->ahWindow[j]) ||
			    BVDC_P_STATE_IS_SHUTDOWN(hVdc->ahDisplay[i]->hCompositor->ahWindow[j]) ||
			    BVDC_P_STATE_IS_INACTIVE(hVdc->ahDisplay[i]->hCompositor->ahWindow[j])) &&
			   (hVdc->ahDisplay[i]->hCompositor->ahWindow[j]->bSetDestroyEventPending))
			{
				BVDC_Window_Handle hWindow = hVdc->ahDisplay[i]->hCompositor->ahWindow[j];
				BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
				BDBG_MSG(("Waiting for window%d to be destroyed", hWindow->eId));
				eStatus = BKNI_WaitForEvent(hWindow->hDestroyDoneEvent,
					BVDC_P_MAX_DESTROY_WAIT_TIMEOUT);
				if(BERR_TIMEOUT == eStatus)
				{
#if (BDBG_DEBUG_BUILD)
					const BVDC_P_Window_DirtyBits  *pDirty = &(hWindow->stCurInfo.stDirty);
					const BVDC_P_VnetMode  *pVntMd = &(hWindow->stVnetMode);
					BVDC_P_CheckDisplayAlignAdjustedStatus(hVdc);
					BDBG_ERR(("Window%d destroy times out", hWindow->eId));
					BDBG_ERR(("VnetMode 0x%08lx, readerState %d, writerState %d",
						*(uint32_t *) pVntMd, hWindow->stCurInfo.eReaderState, hWindow->stCurInfo.eWriterState));
					BDBG_ERR(("   bShutDown %d, bRecfgVnet %d, bSrcPending %d, dirty 0x%08lx",
						pDirty->stBits.bShutdown, pDirty->stBits.bReConfigVnet, pDirty->stBits.bSrcPending, pDirty->aulInts[0]));
#endif
					return BERR_TRACE(eStatus);
				}
			}
			else if(BVDC_P_STATE_IS_ACTIVE(hVdc->ahDisplay[i]->hCompositor->ahWindow[j]) &&
			        hVdc->ahDisplay[i]->hCompositor->ahWindow[j]->bSetAppliedEventPending)
			{
				BVDC_Window_Handle hWindow = hVdc->ahDisplay[i]->hCompositor->ahWindow[j];
				BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
				BDBG_MSG(("Waiting for window%d to be applied", hWindow->eId));
				eStatus = BKNI_WaitForEvent(hWindow->hAppliedDoneEvent,
					BVDC_P_MAX_APPLY_WAIT_TIMEOUT);
				if(BERR_TIMEOUT == eStatus)
				{
#if (BDBG_DEBUG_BUILD)
					const BVDC_P_Window_DirtyBits *pDirty = &(hWindow->stCurInfo.stDirty);
					const BVDC_P_VnetMode  *pVntMd = &(hWindow->stVnetMode);
					BVDC_P_CheckDisplayAlignAdjustedStatus(hVdc);
					BDBG_ERR(("Window%d apply times out", hWindow->eId));
					BDBG_ERR(("VnetMode 0x%08lx, readerState %d, writerState %d",
						*(uint32_t *) pVntMd, hWindow->stCurInfo.eReaderState, hWindow->stCurInfo.eWriterState));
					BDBG_ERR(("   bShutDown %d, bRecfgVnet %d, bSrcPending %d, dirty 0x%08lx",
						pDirty->stBits.bShutdown, pDirty->stBits.bReConfigVnet, pDirty->stBits.bSrcPending, pDirty->aulInts[0]));
#endif
					return BERR_TRACE(eStatus);
				}
			}
		}
	}

	for(i = 0; i < BVDC_P_MAX_SOURCE_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(hVdc->ahSource[i]) &&
		   hVdc->ahSource[i]->bUserAppliedChanges)
		{
			BDBG_OBJECT_ASSERT(hVdc->ahSource[i], BVDC_SRC);
			BDBG_MSG(("Waiting for Source%d to be applied", hVdc->ahSource[i]->eId));
			eStatus = BKNI_WaitForEvent(hVdc->ahSource[i]->hAppliedDoneEvent,
				BVDC_P_MAX_APPLY_WAIT_TIMEOUT);
			if(BERR_TIMEOUT == eStatus)
			{
#if (BDBG_DEBUG_BUILD)
				BVDC_P_CheckDisplayAlignAdjustedStatus(hVdc);
				BDBG_ERR(("Source%d apply times out", hVdc->ahSource[i]->eId));
#endif
				return BERR_TRACE(eStatus);
			}
		}
	}

	BDBG_LEAVE(BVDC_P_CheckApplyChangesStatus);
	return eStatus;
}


/***************************************************************************
 * BVDC_AbortChanges
 *
 */
BERR_Code BVDC_AbortChanges
	( BVDC_Handle                      hVdc )
{
	uint32_t i;

	BDBG_ENTER(BVDC_AbortChanges);
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	for(i = 0; i < BVDC_P_MAX_SOURCE_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(hVdc->ahSource[i]) ||
		   BVDC_P_STATE_IS_CREATE(hVdc->ahSource[i]) ||
		   BVDC_P_STATE_IS_DESTROY(hVdc->ahSource[i]))
		{
			BVDC_P_Source_AbortChanges(hVdc->ahSource[i]);
		}
	}

	for(i = 0; i < BVDC_P_MAX_COMPOSITOR_COUNT; i++)
	{
		if((BVDC_P_STATE_IS_ACTIVE(hVdc->ahCompositor[i]) &&
		    BVDC_P_STATE_IS_ACTIVE(hVdc->ahDisplay[i])) ||
		   (BVDC_P_STATE_IS_CREATE(hVdc->ahCompositor[i]) &&
		    BVDC_P_STATE_IS_CREATE(hVdc->ahDisplay[i])) ||
		   (BVDC_P_STATE_IS_DESTROY(hVdc->ahCompositor[i]) &&
		    BVDC_P_STATE_IS_DESTROY(hVdc->ahDisplay[i])))
		{
			BVDC_P_Display_AbortChanges(hVdc->ahDisplay[i]);
			BVDC_P_Compositor_AbortChanges(hVdc->ahCompositor[i]);
		}
	}

	BDBG_LEAVE(BVDC_AbortChanges);
	return BERR_SUCCESS;
}


/***************************************************************************
 * BVDC_ApplyChanges
 *
 * Validate/Apply User's new changes.  This function will not result in
 * RUL building, but rather taken the new changes.  The ISR will be
 * responsible for building the RUL.
 */
BERR_Code BVDC_ApplyChanges
	( BVDC_Handle                      hVdc )
{
	uint32_t i;
	BERR_Code eStatus = BERR_SUCCESS;

	BDBG_ENTER(BVDC_ApplyChanges);
	BDBG_OBJECT_ASSERT(hVdc, BVDC_VDC);

	BDBG_MSG(("-------------ApplyChanges(%d)------------", hVdc->ulApplyCnt++));

	/* +------------------+
	 * | VALIDATE CHANGES |
	 * +------------------+
	 * User's new settings reject if we have bad new setting with
	 * approriate error status.  No new settings will be used.
	 * Frontend   Things that are going to cause failures should be caught
	 * here.   Other failures may able to detect early in the set function
	 * where error checking is not depended on other settings.   Our build
	 * RULs and ApplyChanges should not failed anymore. */
	eStatus = BERR_TRACE(BVDC_P_Source_ValidateChanges(hVdc->ahSource));
	if(BERR_SUCCESS != eStatus)
	{
		return BERR_TRACE(eStatus);
	}

	eStatus = BERR_TRACE(BVDC_P_Display_ValidateChanges(hVdc->ahDisplay));
	if(BERR_SUCCESS != eStatus)
	{
		return BERR_TRACE(eStatus);
	}

	eStatus = BERR_TRACE(BVDC_P_Compositor_ValidateChanges(hVdc->ahCompositor));
	if(BERR_SUCCESS != eStatus)
	{
		return BERR_TRACE(eStatus);
	}

	/* +-------------- +
	 * | APPLY CHANGES |
	 * +---------------+
	 * Apply user's new settings it will be include in the next RUL building*/
	BKNI_EnterCriticalSection();
	BDBG_ASSERT(0 == hVdc->ulInsideCs);
	hVdc->ulInsideCs++;

	for(i = 0; i < BVDC_P_MAX_SOURCE_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(hVdc->ahSource[i]) ||
		   BVDC_P_STATE_IS_CREATE(hVdc->ahSource[i]) ||
		   BVDC_P_STATE_IS_DESTROY(hVdc->ahSource[i]))
		{
			BVDC_P_Source_ApplyChanges_isr(hVdc->ahSource[i]);
		}
	}

	BVDC_P_Vec_Update_OutMuxes_isr(hVdc);

	/* Note, since display has id now, which might be different from compositor
	   id, we need to loop them separately in case cmp/win logic depends on its
	   display's context changes; */
	for(i = 0; i < BVDC_P_MAX_COMPOSITOR_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(hVdc->ahDisplay[i]) ||
		   BVDC_P_STATE_IS_CREATE(hVdc->ahDisplay[i]) ||
		   BVDC_P_STATE_IS_DESTROY(hVdc->ahDisplay[i]))
		{
			BVDC_P_Display_ApplyChanges_isr(hVdc->ahDisplay[i]);
		}
	}
	for(i = 0; i < BVDC_P_MAX_COMPOSITOR_COUNT; i++)
	{
		if(BVDC_P_STATE_IS_ACTIVE(hVdc->ahCompositor[i]) ||
		   BVDC_P_STATE_IS_CREATE(hVdc->ahCompositor[i]) ||
		   BVDC_P_STATE_IS_DESTROY(hVdc->ahCompositor[i]))
		{
			BVDC_P_Compositor_ApplyChanges_isr(hVdc->ahCompositor[i]);
		}
	}

	hVdc->ulInsideCs--;
	BKNI_LeaveCriticalSection();

	/* +------------------------------------- +
	 * | WAIT FOR CHANGES APPLIED (next vsync)|
	 * +--------------------------------------+
	 * After the changes are put in RUL in _isr it will set event to notify
	 * that changes will be hardware on next trigger. */
	eStatus = BERR_TRACE(BVDC_P_CheckApplyChangesStatus(hVdc));
	if(BERR_SUCCESS != eStatus)
	{
		return BERR_TRACE(eStatus);
	}

	BDBG_LEAVE(BVDC_ApplyChanges);
	return BERR_SUCCESS;
}

/* End of File */
