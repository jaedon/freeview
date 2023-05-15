/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgrc_private.c $
 * $brcm_Revision: Hydra_Software_Devel/195 $
 * $brcm_Date: 8/24/12 1:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/grc/7435/bgrc_private.c $
 * 
 * Hydra_Software_Devel/195   8/24/12 1:45p nissen
 * SW7445-22: Added support for the dual M2MCs on the 7445.
 * 
 * Hydra_Software_Devel/194   8/21/12 12:19p nissen
 * SW7425-3625: Switched to using cached memory access.
 * 
 * Hydra_Software_Devel/192   2/21/12 11:45a nissen
 * SW7425-1413: Fixing problem with last output stripe size for M2MCs with
 * split fifo.
 * 
 * Hydra_Software_Devel/191   2/16/12 3:20p nissen
 * SW7405-5427: Adding flags for output mirroring when blitting
 * 
 * Hydra_Software_Devel/190   1/6/12 1:51p nissen
 * SW7435-13 : Fixed support for 7435 A0.
 * 
 * Hydra_Software_Devel/189   12/15/11 6:22p nissen
 * SW7435-13 : Added support for 7435 A0.
 * 
 * Hydra_Software_Devel/188   12/14/11 4:55p nissen
 * SW7435-13 : Added support for 7435 A0.
 * 
 * Hydra_Software_Devel/187   10/25/11 1:17p nissen
 * SW7420-499: Removed FixedLeft fix.
 * 
 * Hydra_Software_Devel/186   9/9/11 7:27p nissen
 * SWBLURAY-27238: Added support for 7640 B0.
 * 
 * Hydra_Software_Devel/185   4/20/11 2:58p nissen
 * SW7405-5274: Adjusted initial phases for 420 scaling.
 * 
 * Hydra_Software_Devel/184   3/3/11 11:18a nissen
 * SW7420-499: Fixed problem with initial phase on last strip when
 * scaling.
 * 
 * Hydra_Software_Devel/183   11/18/10 5:57p nissen
 * SW7425-19: Fixed destination format registers for null dest on the
 * 7422.
 * 
 * Hydra_Software_Devel/182   11/18/10 5:44p nissen
 * SW7425-19: Fixed source format registers for null source on the 7422.
 * 
 * Hydra_Software_Devel/181   8/31/10 5:01p nissen
 * SW7405-4831: Reduced stripe width when software striping.
 * 
 * Hydra_Software_Devel/180   8/31/10 4:48p nissen
 * SW7420-499: Fixed problem with source stripe width being too small for
 * last stripe.
 * 
 * Hydra_Software_Devel/178   8/3/10 2:34p nissen
 * SW7420-925: Removed BERR_TRACE when returning out of memory error.
 * 
 * Hydra_Software_Devel/177   6/9/10 5:23p nissen
 * SW7405-4385: Fixed calculation of source strip width when using fixed
 * scaling.
 * 
 * Hydra_Software_Devel/176   5/21/10 5:43p nissen
 * SW7405-4384: Added critical section to prevent interrupt handler from
 * getting interrupted to add a blit operation to the queue.
 * 
 * Hydra_Software_Devel/175   5/12/10 1:03p nissen
 * SW7420-499: Fixed problem with output stripe width being to large.
 * 
 * Hydra_Software_Devel/174   1/1/10 3:48p nissen
 * SW7420-499: Fixed problem with calculating output stripe width when
 * using fixed scaling.
 * 
 * Hydra_Software_Devel/173   12/10/09 2:14p nissen
 * SW7420-499: Increased precision of initial phase calculation.
 * 
 * Hydra_Software_Devel/172   10/28/09 2:01p nissen
 * SW7405-2856: Fixed build warning.
 * 
 * Hydra_Software_Devel/171   8/20/09 12:47p nissen
 * PR 52470: Changed some variables and functions to const and/or static.
 * 
 * Hydra_Software_Devel/170   8/18/09 5:40p nissen
 * PR 52470: Optimized setting registers.
 * 
 * Hydra_Software_Devel/169   8/10/09 6:20p nissen
 * PR 57469: Fixed problem with WaitForOperationReady function.
 * 
 * Hydra_Software_Devel/168   6/9/09 4:16p nissen
 * PR 52504: Changed global and static variables to constants.
 * 
 * Hydra_Software_Devel/167   6/9/09 11:43a nissen
 * PR 51077: Fixed getting previous operation in isr.
 * 
 * Hydra_Software_Devel/166   6/7/09 5:14p nissen
 * PR 51077: Fixed while loop condition when processing operations in isr.
 * 
 * Hydra_Software_Devel/165   6/3/09 2:11p nissen
 * PR 51077: Changed location of periodic event reset again.
 * 
 * Hydra_Software_Devel/164   6/3/09 1:15p nissen
 * PR 51077: Changed location of periodic event reset.
 * 
 * Hydra_Software_Devel/163   6/2/09 9:43p nissen
 * PR 51077: Changed code to reset event when running out of memory.
 * 
 * Hydra_Software_Devel/162   6/1/09 8:38p nissen
 * PR 51077: Removed code that processed operations during cleanup.
 * 
 * Hydra_Software_Devel/161   6/1/09 7:02p nissen
 * PR 51077: Added wait for when filter blit fails. Added code to better
 * process completed operations.
 * 
 * Hydra_Software_Devel/160   6/1/09 1:25p nissen
 * PR 51077: Fixed warning.
 * 
 * Hydra_Software_Devel/159   6/1/09 12:31p nissen
 * PR 51077: Added fix for when FilterBlit runs out of memory.
 * 
 * Hydra_Software_Devel/158   4/17/09 4:46p nissen
 * PR 51077: Added callbacks to wait functions.
 * 
 * Hydra_Software_Devel/157   4/9/09 2:11p nissen
 * PR 53698: Removed support for loading a destination palette because the
 * M2MC does not support it.
 * 
 * Hydra_Software_Devel/156   4/7/09 2:51p nissen
 * PR 52976: Added support for 7635/7630 and future chips using RDB
 * defines.
 * 
 * Hydra_Software_Devel/155   3/20/09 4:36p nissen
 * PR 51077: Added functions to wait on operations, and better handling
 * for running out of memory.
 * 
 * Hydra_Software_Devel/154   1/31/09 1:12a jrubio
 * PR51629: add 7336 support
 * 
 * Hydra_Software_Devel/153   12/3/08 7:09p nissen
 * PR 47763: Added support for 7420.
 * 
 * Hydra_Software_Devel/152   10/29/08 5:18p nissen
 * PR 46310: Added register dump for DCE registers 3548 B0.
 * 
 * Hydra_Software_Devel/151   7/30/08 8:10p nissen
 * PR 33687: Added support for the 7601.
 * 
 * Hydra_Software_Devel/150   6/25/08 5:32p nissen
 * PR 44159: Enabled sw striping while scaling/filtering for 3548 and 3556
 * for 420 fix.
 * 
 * Hydra_Software_Devel/149   6/23/08 4:22p nissen
 * PR 43693: Enabled hw striping while scaling/filtering for 3548  and
 * 3556.
 * 
 * Hydra_Software_Devel/148   6/23/08 12:50p nissen
 * PR 43693: Removed scaler coefficient register overwrite fix for 3548
 * and 3556.
 * 
 * Hydra_Software_Devel/147   6/10/08 1:31p nissen
 * PR 43459: Fixed "fixed point" problem when scaling a source that is
 * larger than 2048.
 * 
 * Hydra_Software_Devel/146   6/4/08 7:58p nissen
 * PR 43321: Updated for 3548 and 3556.
 * 
 * Hydra_Software_Devel/145   5/5/08 12:12p nissen
 * PR 42410: Fixed problem with checking if all operations are complete in
 * isr function.
 * 
 * Hydra_Software_Devel/144   4/4/08 12:05p nissen
 * PR 39460: Added support for the 3556.
 * 
 * Hydra_Software_Devel/143   2/8/08 2:17p nissen
 * PR 38945: Added support for the 3548.
 * 
 * Hydra_Software_Devel/142   2/8/08 10:45a nissen
 * PR 39221: Added pulldown register overwrite fix for 3563 C0.
 * 
 * Hydra_Software_Devel/141   2/7/08 12:38p nissen
 * PR 24618: Added sharper anti-flutter filter coefficient table.
 * 
 * Hydra_Software_Devel/140   1/7/08 2:35p nissen
 * PR 35747: Turned off HW striping when source is YCbCr 422 for 7405.
 * 
 * Hydra_Software_Devel/139   12/19/07 8:10p nissen
 * PR 35855: Fixed problem with processing interrupts.
 * 
 * Hydra_Software_Devel/138   12/17/07 9:58a nissen
 * PR 38269: Added support for inverting sources during a blit.
 * 
 * Hydra_Software_Devel/137   12/17/07 9:18a nissen
 * PR 35855: Fixed problem with checking if device is busy.
 * 
 * Hydra_Software_Devel/136   12/10/07 3:07p nissen
 * PR 29724: Fixed problem with reseting macroblock state.
 * 
 * Hydra_Software_Devel/135   12/4/07 5:38p nissen
 * PR 28248: Removed register overwrite fix for 3563 C0 and above.
 * 
 * Hydra_Software_Devel/134   12/4/07 5:28p nissen
 * PR 36878: Added support for the 7335.
 * 
 * Hydra_Software_Devel/133   11/16/07 3:10p nissen
 * PR 36759: Added support for setting the scale factor independently of
 * the provided rectangle sizes.
 * 
 * Hydra_Software_Devel/132   10/18/07 11:26p nissen
 * PR 35920: Fixed support for preallocating system memory.
 * 
 * Hydra_Software_Devel/131   10/12/07 7:52a nissen
 * PR 35920: Added support for preallocating system memory for operation
 * structures when opening module.
 * 
 * Hydra_Software_Devel/130   10/11/07 6:03p nissen
 * PR 33390: Using software striping when scaling down past 7x in vertical
 * direction.
 * 
 * Hydra_Software_Devel/129   9/19/07 11:34a nissen
 * PR 34538: Added support for building for the 7325.
 * 
 * Hydra_Software_Devel/128   9/18/07 1:21p nissen
 * PR 34852: Added support for building for the 3573.
 * 
 * Hydra_Software_Devel/127   7/26/07 1:40p nissen
 * PR 33320: Fixed problem with output stripe size with YCbCr formats.
 * 
 * Hydra_Software_Devel/126   5/2/07 11:46a nissen
 * PR 29724: Added support for 7405 A0 including new MacroBlock strip
 * width setting.
 * 
 * Hydra_Software_Devel/125   4/23/07 11:01a nissen
 * PR 29903: Switched to software striping when using YCbCr422 source for
 * 7400 and 7401.
 * 
 * Hydra_Software_Devel/124   4/3/07 9:03p nissen
 * PR 28248: Removed writing of source color matrix registers in 3563 M2MC
 * register overwrite fix because it was not required.
 * 
 * Hydra_Software_Devel/123   3/30/07 8:56a nissen
 * PR 29200: Fixed interrupt handler to check if module handle is null
 * before using it.
 * 
 * Hydra_Software_Devel/122   3/29/07 11:24a nissen
 * PR 28248: Fixed problem with 3563 M2MC overwritting source color matrix
 * and pulldown registers when writing scaler coefficent registers.
 * 
 * Hydra_Software_Devel/121   3/28/07 9:50a nissen
 * PR 29122: Fixed problem with using an XRGB output surface on 7440.
 * 
 * Hydra_Software_Devel/120   3/8/07 2:32p nissen
 * PR 27748: Fixed problem with output stripe size when scaling YCbCr 420
 * to 422.
 * 
 * Hydra_Software_Devel/119   1/29/07 10:48a nissen
 * PR 19665: Fixed problem with hardware striping while scaling.
 * 
 * Hydra_Software_Devel/118   1/22/07 1:15p nissen
 * PR 23200: Fixed problem with setting initial phase when scaling.
 * 
 * Hydra_Software_Devel/117   12/14/06 12:29p nissen
 * PR 24648: Disabled striping when using YCbCr420 format with 7400 A0.
 * 
 * Hydra_Software_Devel/116   12/14/06 10:05a nissen
 * PR 24648: Disabled striping when using YCbCr420 format with 7401 A0 and
 * B0.
 * 
 * Hydra_Software_Devel/115   12/1/06 10:59a nissen
 * PR 25991: Added support for 7400 B0.
 * 
 * Hydra_Software_Devel/114   11/15/06 11:48a nissen
 * PR 25668: Added support for 7403.
 * 
 * Hydra_Software_Devel/113   11/8/06 11:04a nissen
 * PR 25683: Switched register data memory to uncached.
 * 
 * Hydra_Software_Devel/112   10/24/06 7:37p nissen
 * PR 24648: Fixed problem with VC-1 macroblock
 * 
 * Hydra_Software_Devel/111   10/20/06 6:08p nissen
 * PR 24647: Fixed problem when allocating device memory.
 * 
 * Hydra_Software_Devel/110   10/19/06 11:00a nissen
 * PR 24647: Added support for preallocting all device memory when module
 * is opened.
 * 
 * Hydra_Software_Devel/109   10/6/06 11:59a nissen
 * PR 24740: Removed code that forced point sample filtering.
 * 
 * Hydra_Software_Devel/108   9/29/06 10:11a nissen
 * PR 24618: Added support for second blurryer anti-flutter filter
 * coefficient table.
 * 
 * Hydra_Software_Devel/107   9/8/06 4:01p nissen
 * PR 23517: Added support for 7440.
 * 
 * Hydra_Software_Devel/106   8/28/06 4:32p nissen
 * PR 23941: Re-added function to toggle destination palette bypass.
 * 
 * Hydra_Software_Devel/105   8/17/06 11:33a nissen
 * PR 23179: Added support for building and running on the 3563.
 * 
 * Hydra_Software_Devel/104   8/3/06 9:58a nissen
 * PR 22999: Fixed problem with setting surface registers on 7401 B0.
 * 
 * Hydra_Software_Devel/103   7/25/06 4:47p nissen
 * PR 22852: Fixed problem setting dst surface registers on 7401 B0.
 * 
 * Hydra_Software_Devel/102   7/24/06 8:53a nissen
 * PR 21171: Fixed build problem for 7401 A0.
 * 
 * Hydra_Software_Devel/101   7/12/06 9:33p nissen
 * PR 22396: Added code to clear secondary source surface related
 * registers when not using a YCbCr 420 source surface.
 * 
 * Hydra_Software_Devel/100   6/22/06 10:44a nissen
 * PR 21940: Added support for 7118.
 * 
 * Hydra_Software_Devel/99   5/8/06 7:07a nissen
 * PR 19575: Fixed use of feedthrough bit for M2MCs on 7438.
 * 
 * Hydra_Software_Devel/98   5/5/06 3:38p nissen
 * PR 20236: Removed striping when scaling with YCbCr420 source.
 * 
 * Hydra_Software_Devel/97   5/3/06 12:35p nissen
 * PR 21171: Added support for building and running 7401 B0.
 * 
 * Hydra_Software_Devel/96   5/2/06 12:22p nissen
 * PR 19553: Fixed problem with fill on 7401.
 * 
 * Hydra_Software_Devel/95   4/20/06 12:19p nissen
 * PR 14329: Added code to use "clut scale mode" when scaling/filtering to
 * a palette format.
 * 
 * Hydra_Software_Devel/94   4/18/06 1:55p nissen
 * PR 21007: Fixed warnings.
 * 
 * Hydra_Software_Devel/93   4/18/06 1:26p nissen
 * PR 18369: Added support for hardware striping when scaling on the 7400
 * and 7401.
 * 
 * Hydra_Software_Devel/92   4/10/06 4:24p nissen
 * PR 19575: Fixed use of feedthrough bit for second M2MC on 7438.
 * 
 * Hydra_Software_Devel/91   4/7/06 7:39p nissen
 * PR 19575: Fixed use of feedthrough bit in blit header register for
 * 7438.
 * 
 * Hydra_Software_Devel/90   3/22/06 1:10p nissen
 * PR 19551: Fixed use of format type register field.
 * 
 * Hydra_Software_Devel/89   3/22/06 10:48a nissen
 * PR 18903: Fixed warnings when debug is off.
 * 
 * Hydra_Software_Devel/88   3/15/06 1:25p nissen
 * PR 15840: Fixed problem with YCbCr420 format without filtering enabled.
 * 
 * Hydra_Software_Devel/87   3/15/06 12:08p nissen
 * PR 15840: Added support for using YCbCr420 surfaces as a source.
 * 
 * Hydra_Software_Devel/86   3/8/06 2:46p nissen
 * PR 19575: Added support for building and running second m2mc on 7438.
 * 
 * Hydra_Software_Devel/85   2/21/06 4:36p nissen
 * PR 19553: Fixed problem with setting filter coefficient registers for
 * 7400.
 * 
 * Hydra_Software_Devel/84   2/21/06 12:13p nissen
 * PR 19084: Added support for second m2mc device.
 * 
 * Hydra_Software_Devel/83   2/18/06 8:15a nissen
 * PR 19553: Enabled source channels for fill blit on 7400, otherwise it
 * seems to hang.
 * 
 * Hydra_Software_Devel/82   2/9/06 5:30p nissen
 * PR 19553: Adjusted register groups.
 * 
 * Hydra_Software_Devel/81   2/1/06 5:40p nissen
 * PR 19084: Added support for building for the 7400 A0.
 * 
 * Hydra_Software_Devel/80   2/1/06 1:54p nissen
 * PR 18903: Switch BDGB_MSG to DBG_ERR.
 * 
 * Hydra_Software_Devel/79   1/18/06 11:47a nissen
 * PR 19116: Added support for inactivating operations when no interrupts
 * are expected.
 * 
 * Hydra_Software_Devel/78   1/12/06 5:37p nissen
 * PR 18761: Added support for BCHP_VER macro.
 * 
 * Hydra_Software_Devel/77   1/12/06 3:12p nissen
 * PR 18903: Fixed vxworks warnings and changed BKNI_Printf calls to
 * BDBG_MSG.
 * 
 * Hydra_Software_Devel/76   12/9/05 1:47p nissen
 * PR 18007: Fixed warning.
 * 
 * Hydra_Software_Devel/75   12/8/05 1:41p nissen
 * PR 18007: Fixed initial phases when filtering without scaling.
 * 
 * Hydra_Software_Devel/74   12/7/05 8:32p nissen
 * PR 18007: Fixed potential problem with packet offset in interrupt
 * handler.
 * 
 * Hydra_Software_Devel/73   12/7/05 10:53a nissen
 * PR 18369: Turned off hardware striping for 7401.
 * 
 * Hydra_Software_Devel/72   12/5/05 4:54p nissen
 * PR 18007: Added fix for setting initial phases when scaling.
 * 
 * Hydra_Software_Devel/71   12/3/05 6:03p nissen
 * PR 18007: Fixed problem with reallocating memory that the device was
 * not done with yet.
 * 
 * Hydra_Software_Devel/70   12/1/05 6:10p nissen
 * PR 18007: Added code to return error when down scaling more than 15x
 * for M2MC Cx and above.
 * 
 * Hydra_Software_Devel/69   12/1/05 11:39a nissen
 * PR 18369: Added fix for scaling YCbCr422 surfaces and switched on
 * hardware striping.
 * 
 * Hydra_Software_Devel/68   10/5/05 9:37p nissen
 * PR 17273: Added support for cached memory.
 * 
 * Hydra_Software_Devel/67   10/5/05 1:22p nissen
 * PR 17267: Fixed problem with writing registers.
 * 
 * Hydra_Software_Devel/66   10/5/05 10:39a nissen
 * PR 17267: Having trouble with hardware striping on C0, so switching to
 * software striping for now.
 * 
 * Hydra_Software_Devel/65   10/5/05 10:29a nissen
 * PR 17267: Removed fix that was causing other problems.
 * 
 * Hydra_Software_Devel/64   9/26/05 5:22p nissen
 * PR 17306: Fixed problem with setting destination size registers.
 * 
 * Hydra_Software_Devel/63   9/23/05 2:33p nissen
 * PR 17267: Added code to not turn on dst feeder when scaling with
 * striping.
 * 
 * Hydra_Software_Devel/62   9/13/05 4:36p nissen
 * PR 16499: Added code to free memory past a default or specified
 * threshold.
 * 
 * Hydra_Software_Devel/61   8/4/05 5:34p nissen
 * PR 16489: Fixed problem with processing interrupts for operations that
 * have not completed.
 * 
 * Hydra_Software_Devel/60   8/2/05 4:07p nissen
 * PR 16489: Fixed problem with processing interrupts for operations that
 * have not completed.
 * 
 * Hydra_Software_Devel/59   7/5/05 5:34p nissen
 * PR 16056: Added support for building for the 7401 A0.
 * 
 * Hydra_Software_Devel/58   5/18/05 4:34p agin
 * PR14720: B2, C1, C2 compilation support.
 * 
 * Hydra_Software_Devel/57   5/12/05 4:40p nissen
 * PR 15264: Modified support for user defined phase adjustment when
 * filtering.
 * 
 * Hydra_Software_Devel/56   5/12/05 2:51p nissen
 * PR 15264: Added support for user defined phase adjustment when
 * filtering.
 * 
 * Hydra_Software_Devel/55   5/12/05 11:15a nissen
 * PR 15226: Fixed build error for 7038 Cx.
 * 
 * Hydra_Software_Devel/54   5/12/05 10:22a nissen
 * PR 15226: Modified code to store surface information when a surface is
 * set instead of looking it up later.
 * 
 * Hydra_Software_Devel/53   5/6/05 1:36p nissen
 * PR 13488: Added support for M2MC C1.
 * 
 * Hydra_Software_Devel/52   3/21/05 11:24p nissen
 * PR 14528: Added code for the C0 to use software striping when there is
 * a destination surface.
 * 
 * Hydra_Software_Devel/51   3/3/05 6:53p nissen
 * PR 14281: Fixed support for no scale filtering when scaling in only one
 * direction.
 * 
 * Hydra_Software_Devel/50   2/24/05 1:33p nissen
 * PR 14207: Added bstd.h include.
 * 
 * Hydra_Software_Devel/49   2/17/05 1:18p nissen
 * PR 14086: Added support for new color matrix register precision on the
 * 3560.
 * 
 * Hydra_Software_Devel/48   1/20/05 9:45a nissen
 * PR 13841: Fixed problem with calculating striping values when scaling
 * on the C0.
 * 
 * Hydra_Software_Devel/47   12/17/04 1:37p nissen
 * PR 13325: Added code to process all operations that missed an interrupt
 * when an interrupt for the current operation is received.
 * 
 * Hydra_Software_Devel/46   12/13/04 2:18p nissen
 * PR 13488: Added support for M2MC C0.
 * 
 * Hydra_Software_Devel/45   10/13/04 10:31a nissen
 * PR 12989: Removed code to do striping in hardware.
 * 
 * Hydra_Software_Devel/44   10/6/04 10:18a nissen
 * PR 12728: Removed pedantic errors.
 * 
 * Hydra_Software_Devel/43   9/23/04 2:24p nissen
 * PR 12729: Added support for anti-flutter filtering. Fixed turning off
 * horizontal filtering only when not scaling.
 * 
 * Hydra_Software_Devel/42   7/29/04 9:58a nissen
 * PR 12095: Added sharper and blurry entries to the filter function
 * table.
 * 
 * Hydra_Software_Devel/41   6/29/04 9:13p nissen
 * PR 11745: Fixed problem with vertical scale.
 * 
 * Hydra_Software_Devel/40   6/29/04 8:52p nissen
 * PR 11744: Fixed problem with setting palette offset register.
 * 
 * Hydra_Software_Devel/39   6/28/04 1:02p nissen
 * PR 11699: Eliminated compiler warnings.
 * 
 * Hydra_Software_Devel/38   6/22/04 9:12p nissen
 * PR 11638: Added fix for expanding alpha using replication for ARGBB1555
 * formats.
 * 
 * Hydra_Software_Devel/37   6/11/04 10:27p nissen
 * PR 11486: Fixed problemss with scaling.
 * 
 * Hydra_Software_Devel/36   6/4/04 4:10p nissen
 * PR 11361: Added code to adjust src padding when scaling YCbCr422
 * surfaces so that no src stripes start on a odd pixel boundry.
 * 
 * Hydra_Software_Devel/35   6/3/04 8:34p nissen
 * PR 11349: Added functionality for tracking interrupts.
 * 
 * Hydra_Software_Devel/34   5/24/04 5:08p jasonh
 * PR 11189: Merge down from B0 to main-line
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/3   4/30/04 10:57a nissen
 * PR 9635: Fixed problem with not setting packet memory block to busy.
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   4/28/04 2:53p nissen
 * PR 9635: Fixed problem with initiating register packet.
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/27/04 7:20p nissen
 * PR 9635: Added support for M2MC B0.
 * 
 * Hydra_Software_Devel/33   4/22/04 4:48p nissen
 * PR 10676: Fixed problem with shifting negative numbers when dealing
 * with initial phases while scaling.
 * 
 * Hydra_Software_Devel/32   4/21/04 11:49a nissen
 * PR 10692: Fixed initial phase when down scaling more than 8 times by
 * dividing initial phase by averager count.
 * 
 * Hydra_Software_Devel/31   4/19/04 9:06p nissen
 * PR 10676: Fixed initial phases when scaling.
 * 
 * Hydra_Software_Devel/30   4/16/04 4:55p nissen
 * PR 10427: Fixed problem with last stripe in a scale over reading the
 * source when filtering.
 * 
 * Hydra_Software_Devel/29   4/16/04 10:59a nissen
 * PR 10622: Fixed scaling offset problem by setting initial phases to
 * zero.
 * 
 * Hydra_Software_Devel/28   4/9/04 2:17p nissen
 * PR 10380: Modified field setting macros to do mask and set operation
 * for more than one field at a time.
 * 
 * Hydra_Software_Devel/27   3/12/04 2:29p nissen
 * PR 10024: Added support in isr for forcing events.
 * PR 10072: Added support for no-scale filtering.
 * 
 * Hydra_Software_Devel/26   3/9/04 10:40a nissen
 * PR 8725: Fixed warnings in printf statements.
 * 
 * Hydra_Software_Devel/25   3/9/04 10:27a nissen
 * PR 9856: Fixed problem with not disabling striping when not using it.
 * 
 * Hydra_Software_Devel/24   3/5/04 2:37p nissen
 * PR 9856: Fixed problem with rectangle width's and height's being set to
 * zero.
 * 
 * Hydra_Software_Devel/23   2/17/04 2:42p nissen
 * PR 9719: Added support for sharper filtering when upscaling.
 * 
 * Hydra_Software_Devel/22   2/11/04 5:30p nissen
 * PR 9721: Added support for dest constant color when there is no dest
 * surface.
 * 
 * Hydra_Software_Devel/21   2/10/04 11:23a nissen
 * PR 9700: Added support for blurry filtering.
 * 
 * Hydra_Software_Devel/20   1/30/04 1:46p nissen
 * PR 9537: Fixed problem with loading wrong register group when copying a
 * state.
 * 
 * Hydra_Software_Devel/19   1/21/04 6:49p nissen
 * PR 9431: Fixed problem with destination when doing a source fill.
 * 
 * Hydra_Software_Devel/18   1/21/04 1:49p nissen
 * PR 9195: Removed math.h include.
 * 
 * Hydra_Software_Devel/17   12/21/03 8:52p nissen
 * PR 9033: Fixed setting source width and height registers when doing a
 * fill.
 * 
 * Hydra_Software_Devel/16   12/18/03 2:09p nissen
 * PR 9016: Added support for splitting scaled/filtered blits into
 * vertical strips
 * 
 * Hydra_Software_Devel/15   11/21/03 6:42p nissen
 * Fixed problem with callback function pointer.
 * 
 * Hydra_Software_Devel/14   11/20/03 7:03p nissen
 * Added operation queuing mechanism and support for interrupts.
 * 
 * Hydra_Software_Devel/13   10/20/03 1:54p nissen
 * Eliminated warnings.
 * 
 * Hydra_Software_Devel/12   10/10/03 5:29p jasonh
 * Removed C++ style comments.
 * 
 * Hydra_Software_Devel/11   9/23/03 5:51p pntruong
 * Removed the 7038_ from header filename generated from RDB.
 * 
 * Hydra_Software_Devel/10   9/19/03 3:57p nissen
 * Removed use of float (PR 8043).
 * 
 * Hydra_Software_Devel/9   8/26/03 4:11p nissen
 * Added support for initial phase when scaling.
 * 
 * Hydra_Software_Devel/8   8/21/03 1:27p nissen
 * Set palette to non-palette blit to use one pixel feedthrough. Added
 * code to disable color key in copy register functions.
 * 
 * Hydra_Software_Devel/7   8/20/03 4:43p nissen
 * Added functions to copy state groups.
 * 
 * Hydra_Software_Devel/6   8/19/03 10:12a nissen
 * Fixed filter coefficient tables.  Added code to check if registers
 * changed.
 * 
 * Hydra_Software_Devel/5   8/12/03 11:35a nissen
 * Added more filter coefficient tables. Changed use of field and register
 * setting macros.
 * 
 * Hydra_Software_Devel/4   8/6/03 2:34p nissen
 * Added support for scaling and filtering. Added support for only setting
 * register sets that have changed.
 * 
 * Hydra_Software_Devel/3   6/25/03 12:03p nissen
 * Update function that sets registers.
 * 
 * Hydra_Software_Devel/2   6/5/03 5:39p jasonh
 * Changed register include name.
 * 
 * Hydra_Software_Devel/1   5/28/03 4:16p nissen
 * Graphics compositor.
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bstd_defs.h"
#include "berr.h"
#include "bdbg.h"
#include "bkni.h"
#include "breg_mem.h"
#include "bchp.h"

#if (BCHP_CHIP==7435) || (BCHP_CHIP==7445)
#include "bchp_m2mc.h"
#include "bchp_m2mc1.h"
#else
#include "bchp_m2mc.h"
#endif

#if ((BCHP_CHIP==7400) && (BCHP_VER == BCHP_VER_A0)) || \
	((BCHP_CHIP==7438) && (BCHP_VER >= BCHP_VER_A0)) || \
	((BCHP_CHIP==7440) && (BCHP_VER >= BCHP_VER_A0)) || \
	((BCHP_CHIP==7640) && (BCHP_VER >= BCHP_VER_B0))
#include "bchp_m2mc_1.h"
#endif
#include "bchp_common.h"
#include "bgrc.h"
#include "bgrc_private.h"

BDBG_MODULE(BGRC);

/***************************************************************************/
static uint32_t *BGRC_P_List_AllocPacket( BGRC_Handle hGrc, uint32_t ulPacketHeader, uint32_t *pulPacketOffset, uint32_t *pulPacketSize, BGRC_P_Block **ppBlock );
static BERR_Code BGRC_P_List_PreparePacket( BGRC_Handle hGrc, bool bEnableInterrupt );
static void BGRC_P_List_WritePacket( BGRC_Handle hGrc, uint32_t *pulPacket, uint32_t ulPacketHeader );
static void BGRC_P_List_RenderPacket( BGRC_Handle hGrc, uint32_t ulPacketOffset );
static BGRC_P_Block *BGRC_P_List_CreateBlock( BGRC_Handle hGrc );
static BGRC_P_Block *BGRC_P_List_DestroyBlock( BGRC_Handle hGrc, BGRC_P_Block *pBlock );
static BGRC_P_Operation *BGRC_P_Operation_Alloc( BGRC_Handle hGrc, BGRC_Callback pCallback, void *pData, BGRC_P_Block *pBlock, uint32_t ulPacketOffset, bool bSetEvent );
static void BGRC_P_Operation_AddToList( BGRC_Handle hGrc, BGRC_P_Operation *pOp );
static void BGRC_P_Operation_RemoveFromList( BGRC_Handle hGrc, BGRC_P_Operation *pOp );

/***************************************************************************/
/* FIR FILTER COEFFICENT TABLES                                            */
/***************************************************************************/
extern const uint32_t BGRC_P_FIRCOEFF_POINTSAMPLE[];
extern const uint32_t BGRC_P_FIRCOEFF_BILINEAR[];
extern const uint32_t BGRC_P_FIRCOEFF_BLURRY_3to1[];
extern const uint32_t BGRC_P_FIRCOEFF_ANISOTROPIC_1to1[];
extern const uint32_t BGRC_P_FIRCOEFF_ANISOTROPIC_2to1[];
extern const uint32_t BGRC_P_FIRCOEFF_ANISOTROPIC_3to1[];
extern const uint32_t BGRC_P_FIRCOEFF_ANISOTROPIC_4to1[];
extern const uint32_t BGRC_P_FIRCOEFF_ANISOTROPIC_5to1[];
extern const uint32_t BGRC_P_FIRCOEFF_ANISOTROPIC_6to1[];
extern const uint32_t BGRC_P_FIRCOEFF_ANISOTROPIC_7to1[];
extern const uint32_t BGRC_P_FIRCOEFF_ANISOTROPIC_8to1[];
extern const uint32_t BGRC_P_FIRCOEFF_SHARP_8to9[];
extern const uint32_t BGRC_P_FIRCOEFF_SHARP_8to8[];
extern const uint32_t BGRC_P_FIRCOEFF_SHARP_8to7[];
extern const uint32_t BGRC_P_FIRCOEFF_SHARP_8to6[];
extern const uint32_t BGRC_P_FIRCOEFF_SHARP_8to5[];
extern const uint32_t BGRC_P_FIRCOEFF_SHARP_8to4[];
extern const uint32_t BGRC_P_FIRCOEFF_SHARP_8to3[];
extern const uint32_t BGRC_P_FIRCOEFF_SHARP_8to2[];
extern const uint32_t BGRC_P_FIRCOEFF_SHARP_8to1[];
extern const uint32_t BGRC_P_FIRCOEFF_SHARP_1toN[];
extern const uint32_t BGRC_P_FIRCOEFF_SHARPER_1toN[];
extern const uint32_t BGRC_P_FIRCOEFF_ANTIFLUTTER[];
extern const uint32_t BGRC_P_FIRCOEFF_ANTIFLUTTERSCALE[];
extern const uint32_t BGRC_P_FIRCOEFF_ANTIFLUTTERBLURRY[];
extern const uint32_t BGRC_P_FIRCOEFF_ANTIFLUTTERSHARP[];

/***************************************************************************/
/* SOURCE PAD ARRAYS FOR SPLIT FILTERED BLITS                              */
/***************************************************************************/
static const uint32_t BGRC_P_FILTER_SRCPADLEFT[] = 
{
	0,                            /* BGRC_FilterCoeffs_ePointSample */
	0,                            /* BGRC_FilterCoeffs_eBilinear */
	BGRC_P_FIR_TAP_COUNT / 2 - 1, /* BGRC_FilterCoeffs_eAnisotropic */
	BGRC_P_FIR_TAP_COUNT / 2 - 1, /* BGRC_FilterCoeffs_eSharp */
	BGRC_P_FIR_TAP_COUNT / 2 - 1, /* BGRC_FilterCoeffs_eSharper */
	BGRC_P_FIR_TAP_COUNT / 2 - 1, /* BGRC_FilterCoeffs_eBlurry */
	BGRC_P_FIR_TAP_COUNT / 2 - 1, /* BGRC_FilterCoeffs_eAntiFlutter */
	BGRC_P_FIR_TAP_COUNT / 2 - 1, /* BGRC_FilterCoeffs_eAntiFlutterBlurry */
	BGRC_P_FIR_TAP_COUNT / 2 - 1  /* BGRC_FilterCoeffs_eAntiFlutterSharp */
};

static const uint32_t BGRC_P_FILTER_SRCPADRIGHT[] = 
{
	0,                            /* BGRC_FilterCoeffs_ePointSample */
	2,                            /* BGRC_FilterCoeffs_eBilinear */
	BGRC_P_FIR_TAP_COUNT / 2,     /* BGRC_FilterCoeffs_eAnisotropic */
	BGRC_P_FIR_TAP_COUNT / 2,     /* BGRC_FilterCoeffs_eSharp */
	BGRC_P_FIR_TAP_COUNT / 2,     /* BGRC_FilterCoeffs_eSharper */
	BGRC_P_FIR_TAP_COUNT / 2,     /* BGRC_FilterCoeffs_eBlurry */
	BGRC_P_FIR_TAP_COUNT / 2,     /* BGRC_FilterCoeffs_eAntiFlutter */
	BGRC_P_FIR_TAP_COUNT / 2,     /* BGRC_FilterCoeffs_eAntiFlutterBlurry */
	BGRC_P_FIR_TAP_COUNT / 2      /* BGRC_FilterCoeffs_eAntiFlutterSharp */
};

/***************************************************************************/
/* TABLE OF FIR FILTER TABLE RETRIEVAL FUNCTIONS                           */
/***************************************************************************/
typedef const uint32_t *(*GetFilterCoefficients)( uint32_t ulScalerStep );

static const uint32_t *BGRC_P_GetFilterCoefficients_PointSample( uint32_t ulScalerStep );
static const uint32_t *BGRC_P_GetFilterCoefficients_Bilinear( uint32_t ulScalerStep );
static const uint32_t *BGRC_P_GetFilterCoefficients_Anisotropic( uint32_t ulScalerStep );
static const uint32_t *BGRC_P_GetFilterCoefficients_Sharp( uint32_t ulScalerStep );
static const uint32_t *BGRC_P_GetFilterCoefficients_Sharper( uint32_t ulScalerStep );
static const uint32_t *BGRC_P_GetFilterCoefficients_Blurry( uint32_t ulScalerStep );
static const uint32_t *BGRC_P_GetFilterCoefficients_AntiFlutter( uint32_t ulScalerStep );
static const uint32_t *BGRC_P_GetFilterCoefficients_AntiFlutterBlurry( uint32_t ulScalerStep );
static const uint32_t *BGRC_P_GetFilterCoefficients_AntiFlutterSharp( uint32_t ulScalerStep );

/***************************************************************************/
static const GetFilterCoefficients BGRC_P_GET_FILTER_COEFFICIENTS[] = 
{
	BGRC_P_GetFilterCoefficients_PointSample,
	BGRC_P_GetFilterCoefficients_Bilinear,
	BGRC_P_GetFilterCoefficients_Anisotropic,
	BGRC_P_GetFilterCoefficients_Sharp,
	BGRC_P_GetFilterCoefficients_Sharper,
	BGRC_P_GetFilterCoefficients_Blurry,
	BGRC_P_GetFilterCoefficients_AntiFlutter,
	BGRC_P_GetFilterCoefficients_AntiFlutterBlurry,
	BGRC_P_GetFilterCoefficients_AntiFlutterSharp
};

/***************************************************************************/
/* FUNCTIONS FOR RETRIEVING FIR FILTER COEFFICIENT TABLES                  */
/***************************************************************************/
static const uint32_t *BGRC_P_GetFilterCoefficients_PointSample(
	uint32_t ulScalerStep )
{
	BSTD_UNUSED( ulScalerStep );

	return (uint32_t *) BGRC_P_FIRCOEFF_POINTSAMPLE;
}

/***************************************************************************/
static const uint32_t *BGRC_P_GetFilterCoefficients_Bilinear(
	uint32_t ulScalerStep )
{
	BSTD_UNUSED( ulScalerStep );

	return (uint32_t *) BGRC_P_FIRCOEFF_BILINEAR;
}

/***************************************************************************/
static const uint32_t *BGRC_P_GetFilterCoefficients_Blurry(
	uint32_t ulScalerStep )
{
	BSTD_UNUSED( ulScalerStep );

	return (uint32_t *) BGRC_P_FIRCOEFF_BLURRY_3to1;
}

/***************************************************************************/
static const uint32_t *BGRC_P_GetFilterCoefficients_Anisotropic(
	uint32_t ulScalerStep )
{
	if( ulScalerStep < (2 << BGRC_P_SCALER_STEP_FRAC_BITS) )
		return (uint32_t *) BGRC_P_FIRCOEFF_ANISOTROPIC_1to1;
	else if( ulScalerStep < (3 << BGRC_P_SCALER_STEP_FRAC_BITS) )
		return (uint32_t *) BGRC_P_FIRCOEFF_ANISOTROPIC_2to1;
	else if( ulScalerStep < (4 << BGRC_P_SCALER_STEP_FRAC_BITS) )
		return (uint32_t *) BGRC_P_FIRCOEFF_ANISOTROPIC_3to1;
	else if( ulScalerStep < (5 << BGRC_P_SCALER_STEP_FRAC_BITS) )
		return (uint32_t *) BGRC_P_FIRCOEFF_ANISOTROPIC_4to1;
	else if( ulScalerStep < (6 << BGRC_P_SCALER_STEP_FRAC_BITS) )
		return (uint32_t *) BGRC_P_FIRCOEFF_ANISOTROPIC_5to1;
	else if( ulScalerStep < (7 << BGRC_P_SCALER_STEP_FRAC_BITS) )
		return (uint32_t *) BGRC_P_FIRCOEFF_ANISOTROPIC_6to1;
	else if( ulScalerStep < (8 << BGRC_P_SCALER_STEP_FRAC_BITS) )
		return (uint32_t *) BGRC_P_FIRCOEFF_ANISOTROPIC_7to1;
	else
		return (uint32_t *) BGRC_P_FIRCOEFF_ANISOTROPIC_8to1;
}

/***************************************************************************/
static const uint32_t *BGRC_P_GetFilterCoefficients_Sharp(
	uint32_t ulScalerStep )
{
	if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 9) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_1toN;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 8) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to9;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 7) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to8;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 6) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to7;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 5) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to6;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 4) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to5;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 3) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to4;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 2) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to3;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 1) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to2;
	else
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to1;
}

/***************************************************************************/
static const uint32_t *BGRC_P_GetFilterCoefficients_Sharper(
	uint32_t ulScalerStep )
{
	if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 9) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARPER_1toN;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 8) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to9;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 7) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to8;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 6) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to7;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 5) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to6;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 4) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to5;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 3) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to4;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 2) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to3;
	else if( ulScalerStep < ((8 << BGRC_P_SCALER_STEP_FRAC_BITS) / 1) )
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to2;
	else
		return (uint32_t *) BGRC_P_FIRCOEFF_SHARP_8to1;
}

/***************************************************************************/
static const uint32_t *BGRC_P_GetFilterCoefficients_AntiFlutter(
	uint32_t ulScalerStep )
{
	if( ulScalerStep == (1 << BGRC_P_SCALER_STEP_FRAC_BITS) )
		return (uint32_t *) BGRC_P_FIRCOEFF_ANTIFLUTTER;
	else
		return (uint32_t *) BGRC_P_FIRCOEFF_ANTIFLUTTERSCALE;
}

/***************************************************************************/
static const uint32_t *BGRC_P_GetFilterCoefficients_AntiFlutterBlurry(
	uint32_t ulScalerStep )
{
	if( ulScalerStep == (1 << BGRC_P_SCALER_STEP_FRAC_BITS) )
		return (uint32_t *) BGRC_P_FIRCOEFF_ANTIFLUTTERBLURRY;
	else
		return (uint32_t *) BGRC_P_FIRCOEFF_ANTIFLUTTERSCALE;
}

/***************************************************************************/
static const uint32_t *BGRC_P_GetFilterCoefficients_AntiFlutterSharp(
	uint32_t ulScalerStep )
{
	if( ulScalerStep == (1 << BGRC_P_SCALER_STEP_FRAC_BITS) )
		return (uint32_t *) BGRC_P_FIRCOEFF_ANTIFLUTTERSHARP;
	else
		return (uint32_t *) BGRC_P_FIRCOEFF_ANTIFLUTTERSHARP;
}

/***************************************************************************/
static const uint8_t BGRC_P_BLEND_OP[] = 
{
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_ZERO),           /* eZero */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_HALF),           /* eHalf */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_ZERO),           /* eOne */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_SOURCE_COLOR),   /* eSourceColor */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_SOURCE_COLOR),   /* eInverseSourceColor */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_SOURCE_ALPHA),   /* eSourceAlpha */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_SOURCE_ALPHA),   /* eInverseSourceAlpha */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_DEST_COLOR),     /* eDestinationColor */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_DEST_COLOR),     /* eInverseDestinationColor */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_DEST_ALPHA),     /* eDestinationAlpha */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_DEST_ALPHA),     /* eInverseDestinationAlpha */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_CONSTANT_COLOR), /* eConstantColor */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_CONSTANT_COLOR), /* eInverseConstantColor */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_CONSTANT_ALPHA), /* eConstantAlpha */
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_CONSTANT_ALPHA)  /* eInverseConstantAlpha */
};

/***************************************************************************/
static const uint8_t BGRC_P_BLEND_OP_INV[] = 
{
	0,                                            /* eZero */
	0,                                            /* eHalf */
	1,                                            /* eOne */
	0,                                            /* eSourceColor */
	1,                                            /* eInverseSourceColor */
	0,                                            /* eSourceAlpha */
	1,                                            /* eInverseSourceAlpha */
	0,                                            /* eDestinationColor */
	1,                                            /* eInverseDestinationColor */
	0,                                            /* eDestinationAlpha */
	1,                                            /* eInverseDestinationAlpha */
	0,                                            /* eConstantColor */
	1,                                            /* eInverseConstantColor */
	0,                                            /* eConstantAlpha */
	1                                             /* eInverseConstantAlpha */
};

/***************************************************************************/
uint32_t BGRC_P_GetBlendOp( 
	BGRC_Blend_Source eSource )
{
	return BGRC_P_BLEND_OP[eSource];
}

/***************************************************************************/
uint32_t BGRC_P_GetBlendOpInv( 
	BGRC_Blend_Source eSource )
{
	return BGRC_P_BLEND_OP_INV[eSource];
}

/***************************************************************************/
/* BLIT FUNCTIONS                                                          */
/***************************************************************************/
BERR_Code BGRC_P_Blit(
	BGRC_Handle hGrc,
	BGRC_Callback pUserCallback,
	void *pUserData,
	bool bSetEvent )
{
	BGRC_P_Operation *pOp;
	BGRC_P_Block *pBlock = NULL;
	uint32_t ulPacketHeader;
	uint32_t ulPacketSize = 0;
	uint32_t ulPacketOffset = 0;
	uint32_t *pulPacket;
	BERR_Code err;

	BKNI_ResetEvent( hGrc->hPeriodicEvent );

	/* prepare packet */
	err = BGRC_P_List_PreparePacket( hGrc, (pUserCallback || bSetEvent || hGrc->bPeriodicInterrupt) ? true : false );
	if( err != BERR_SUCCESS )
		return err;

	ulPacketHeader = hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(LIST_PACKET_HEADER_1)];

	/* free unused memory that is over the allocation limit */
	BGRC_P_Operation_CleanupList( hGrc );
	BGRC_P_List_CleanupPacketMemory( hGrc );

	/* allocate packet */
	pulPacket = BGRC_P_List_AllocPacket( hGrc, ulPacketHeader, &ulPacketOffset, &ulPacketSize, &pBlock );
	if( pulPacket == NULL )
		return BERR_OUT_OF_DEVICE_MEMORY;

	/* allocate operation */
	pOp = BGRC_P_Operation_Alloc( hGrc, pUserCallback, pUserData, pBlock, ulPacketOffset, bSetEvent );
	if( pOp == NULL )
	{
		hGrc->pCurrListBlock->ulRefCount--;
		return BERR_OUT_OF_SYSTEM_MEMORY;
	}

	/* set periodic event */
	if( hGrc->bPeriodicInterrupt )
	{
		pOp->bSetPeriodicEvent = true;
		hGrc->bPeriodicInterrupt = false;
	}

	/* add operation to list */
BKNI_EnterCriticalSection();
	BGRC_P_Operation_AddToList( hGrc, pOp );

	/* write packet */
	BGRC_P_List_WritePacket( hGrc, pulPacket, ulPacketHeader );

#if 1
	/* flush memory writes for register data */
	BMEM_FlushCache( hGrc->hMemory, pulPacket, ulPacketSize );
#endif

	/* initiate packet */
	BGRC_P_List_RenderPacket( hGrc, ulPacketOffset );
BKNI_LeaveCriticalSection();

	/* disable loading of all register groups */
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, SRC_FEEDER_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, OUTPUT_FEEDER_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, SRC_COLOR_KEY_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, DST_COLOR_KEY_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, SRC_COLOR_MATRIX_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, DST_COLOR_MATRIX_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, OUTPUT_COLOR_MATRIX_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, SCALE_COEF_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, BLEND_PARAM_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, ROP_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, SRC_CLUT_GRP_CNTRL, GRP_DISABLE );
	BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, DST_CLUT_GRP_CNTRL, GRP_DISABLE );

	/* store some previous packet information */
	hGrc->pulPrevPacket = pulPacket;
	hGrc->pPrevUserCallback = pUserCallback;

	return BERR_SUCCESS;
}

/***************************************************************************/
#define BGRC_P_FIXED_SHIFT_RIGHT( data, shift ) \
	(((data) >> (shift)) | (((data) & 0x80000000) ? ~(0xFFFFFFFF >> (shift)) : 0))
	
/***************************************************************************/
BERR_Code BGRC_P_FilterBlit(
	BGRC_Handle hGrc,
	BGRC_Callback pCallback,
	void *pData,
	bool bSetEvent )
{
	BGRC_P_State *pState = &hGrc->CurrentState;
	BERR_Code err;

	uint32_t ulSrcWidth = pState->SrcSurface.ulWidth;
	uint32_t ulOutWidth = pState->OutSurface.ulWidth;
	uint32_t ulSrcHeight = pState->SrcSurface.ulHeight;
	uint32_t ulOutHeight = pState->OutSurface.ulHeight;
	uint32_t ulHorzScalerStep;
	uint32_t ulVertScalerStep;

	/* set averager count and coefficient values */
	pState->ulHorzAveragerCount = 1;
	pState->ulHorzAveragerCoeff = 1 << BGRC_P_AVERAGER_COEFF_FRAC_BITS;
	pState->ulVertAveragerCount = 1;
	pState->ulVertAveragerCoeff = 1 << BGRC_P_AVERAGER_COEFF_FRAC_BITS;

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	while( ulSrcWidth > ulOutWidth * 8 )
	{
		ulSrcWidth /= 2;
		pState->ulHorzAveragerCount *= 2;
		pState->ulHorzAveragerCoeff /= 2;
	}

	while( ulSrcHeight > ulOutHeight * 8 )
	{
		ulSrcHeight /= 2;
		pState->ulVertAveragerCount *= 2;
		pState->ulVertAveragerCoeff /= 2;
	}

	ulSrcWidth = pState->SrcSurface.ulWidth;
	ulSrcHeight = pState->SrcSurface.ulHeight;
#else
	if( (ulSrcWidth > ulOutWidth * BGRC_P_SCALE_DOWN_MAX) || 
		(ulSrcHeight > ulOutHeight * BGRC_P_SCALE_DOWN_MAX) )
		return BERR_TRACE(BGRC_ERR_MAX_SCALE_DOWN_LIMIT_EXCEEDED);
#endif

	/* set scaler step and initial phase values */
	if( ((ulSrcWidth << BGRC_P_SCALER_STEP_FRAC_BITS) >> BGRC_P_SCALER_STEP_FRAC_BITS) != ulSrcWidth )
		ulHorzScalerStep = ((ulSrcWidth / 2) << BGRC_P_SCALER_STEP_FRAC_BITS) / (ulOutWidth / 2);
	else
		ulHorzScalerStep = (ulSrcWidth << BGRC_P_SCALER_STEP_FRAC_BITS) / ulOutWidth;

	if( ((ulSrcHeight << BGRC_P_SCALER_STEP_FRAC_BITS) >> BGRC_P_SCALER_STEP_FRAC_BITS) != ulSrcHeight )
		ulVertScalerStep = ((ulSrcHeight / 2) << BGRC_P_SCALER_STEP_FRAC_BITS) / (ulOutHeight / 2);
	else
		ulVertScalerStep = (ulSrcHeight << BGRC_P_SCALER_STEP_FRAC_BITS) / ulOutHeight;

	if ( (pState->ulHorzScalerDen == 0) || (pState->ulHorzScalerNum == 0) )
		pState->ulHorzScalerStep = ulHorzScalerStep;
	else
		pState->ulHorzScalerStep = (pState->ulHorzScalerNum << BGRC_P_SCALER_STEP_FRAC_BITS) / pState->ulHorzScalerDen;

	if ( (pState->ulVertScalerDen == 0) || (pState->ulVertScalerNum == 0) )
		pState->ulVertScalerStep = ulVertScalerStep;
	else
		pState->ulVertScalerStep = (pState->ulVertScalerNum << BGRC_P_SCALER_STEP_FRAC_BITS) / pState->ulVertScalerDen;

	pState->ulOverlapStrip = 0;

	/* set initial phases */
	pState->ulHorzInitPhase = BGRC_P_FIXED_SHIFT_RIGHT(pState->ulHorzScalerStep - BGRC_P_SCALER_STEP_FRAC_MASK - 1, 1) + 
		(pState->iHorzPhaseAdj << (BGRC_P_SCALER_STEP_FRAC_BITS - pState->ulPhaseShift));
	pState->ulVertInitPhase = BGRC_P_FIXED_SHIFT_RIGHT(pState->ulVertScalerStep - BGRC_P_SCALER_STEP_FRAC_MASK - 1, 1) + 
		(pState->iVertPhaseAdj << (BGRC_P_SCALER_STEP_FRAC_BITS - pState->ulPhaseShift));

	/* check if both src and out widths are greater than max striping width */
	if( (ulSrcWidth > BGRC_P_STRIP_WIDTH_MAX) && (ulOutWidth > BGRC_P_STRIP_WIDTH_MAX) )
	{
		bool bSrcYCbCr422 = BPXL_IS_YCbCr422_FORMAT(pState->SrcSurface.eFormat) | 
			BPXL_IS_YCbCr420_FORMAT(pState->SrcSurface.eFormat);
		bool bOutYCbCr422 = BPXL_IS_YCbCr422_FORMAT(pState->OutSurface.eFormat);

#if ((BCHP_CHIP!=7038) || (BCHP_VER >= BCHP_VER_C0))

#ifdef BGRC_P_SW_STRIPE_SCALE_YCBCR42X
		if( (pState->DstSurface.hSurface == NULL) && (!hGrc->bYCbCr420Source) && (!bSrcYCbCr422) && 
#else
		if( (pState->DstSurface.hSurface == NULL) && (!bSrcYCbCr422) && 
#endif
			(ulOutHeight * BGRC_P_SCALE_DOWN_MAX_Y >= ulSrcHeight) )
		{
			/* setup hardware striping */
			uint32_t ulOverlap = ((pState->ulHorzScalerStep + BGRC_P_SCALER_STEP_FRAC_MASK) >> BGRC_P_SCALER_STEP_FRAC_BITS);
			uint32_t ulOverlapClamp = BGRC_P_MIN(ulOverlap + BGRC_P_FIR_OVERLAP_MIN, BGRC_P_FIR_OVERLAP_MIN * 2);
			uint32_t ulStripWidth = BGRC_P_STRIP_WIDTH_MAX - ulOverlapClamp * 2;
			if( ulSrcWidth < ulOutWidth )
			{
				pState->ulHorzScalerStep--;
				if( pState->ulHorzScalerNum && pState->ulHorzScalerDen )
					pState->ulOutStripWidth = (ulStripWidth * pState->ulHorzScalerDen) / pState->ulHorzScalerNum;
				else
					pState->ulOutStripWidth = (ulStripWidth * ulOutWidth) / ulSrcWidth;
			}
			else
			{
				pState->ulOutStripWidth = ulStripWidth;
			}

			if( (ulOutWidth % pState->ulOutStripWidth) <= ulOverlapClamp )
				pState->ulOutStripWidth -= ulOverlapClamp;

			if( bOutYCbCr422 && (pState->ulOutStripWidth & 1) )
			{
				pState->ulOutStripWidth--;
				if( (ulOutWidth % pState->ulOutStripWidth) == 0 )
					pState->ulOutStripWidth -= 2;
			}

#if defined(BCHP_M2MC_BLIT_CTRL_BLOCK_AUTO_SPLIT_FIFO_MASK)
{
			/* make sure last stripe is less pixels than the rest */
			uint32_t scale_factor = ((BGRC_P_SCALER_STEP_FRAC_MASK+1) << 2) / ulHorzScalerStep;
			if( pState->ulOutStripWidth && ((ulOutWidth % pState->ulOutStripWidth) >= pState->ulOutStripWidth - scale_factor) )
				pState->ulOutStripWidth += 2;
}
#endif

			pState->ulSrcStripWidth = (pState->ulOutStripWidth * ulHorzScalerStep) >> BGRC_P_SCALER_STEP_TO_STRIPE_WIDTH_SHIFT;
			pState->ulOverlapStrip = ulOverlapClamp;
		}
		else
#endif
		{
			uint32_t ulSrcX = pState->SrcSurface.ulX;
			uint32_t ulStripWidth;
			uint32_t ulStripCount;
			uint32_t ulSrcPadLeft;
			uint32_t ulSrcPadRight;
			uint32_t ii;

			/* calculate fixed shift for large source widths */
			uint32_t ulFixedLeft;
			uint32_t ulFixedRight;
			uint32_t ulFixedShift = 0;
			if ( (pState->ulHorzScalerDen == 0) || (pState->ulHorzScalerNum == 0) )
			{
				uint32_t ulSrcSize = ulSrcWidth;
				while( ((ulSrcSize << (BGRC_P_SCALER_STEP_FRAC_BITS + 1)) >> (BGRC_P_SCALER_STEP_FRAC_BITS + 1)) != ulSrcSize )
				{
					ulSrcSize /= 2;
					ulFixedShift++;;
				}
			}

			/* break scale into multiple vertical strip blits */
/*			ulFixedLeft = (pState->ulHorzInitPhase & BCHP_M2MC_HORIZ_SCALER_INITIAL_PHASE_PHASE_MASK) >> ulFixedShift;*/
			ulFixedLeft = pState->ulHorzInitPhase >> ulFixedShift;
			ulStripWidth = (ulSrcWidth > ulOutWidth) ? (BGRC_P_STRIP_WIDTH_MAX - 4) : (((BGRC_P_STRIP_WIDTH_MAX - 7) * ulOutWidth) / ulSrcWidth);
			ulStripCount = (ulOutWidth + ulStripWidth - 1) / ulStripWidth;

			ulSrcPadLeft = 0;
			ulSrcPadRight = BGRC_P_FILTER_SRCPADRIGHT[pState->eHorzCoeffs];
			if( bSrcYCbCr422 && (pState->SrcSurface.ulX & 1) )
			{
				pState->SrcSurface.ulX++;
				ulSrcPadLeft--;
			}

			/* set strip's output width */
			pState->OutSurface.ulWidth = (ulOutWidth + ulStripCount - 1) / ulStripCount;
			pState->DstSurface.ulWidth = pState->OutSurface.ulWidth;
			if( bOutYCbCr422 && (pState->OutSurface.ulWidth & 1) )
			{
				pState->OutSurface.ulWidth--;
				pState->DstSurface.ulWidth--;
			}

			/* blit all but last strip */
			for( ii = 0; ii < ulOutWidth - pState->OutSurface.ulWidth; ii += pState->OutSurface.ulWidth )
			{
				ulFixedRight = ulFixedLeft + ((pState->OutSurface.ulWidth * pState->ulHorzScalerStep) >> ulFixedShift);

				/* set strip's source width */
				pState->SrcSurface.ulWidth = (BGRC_P_FIXED_SHIFT_RIGHT(ulFixedRight, BGRC_P_SCALER_STEP_FRAC_BITS - ulFixedShift) - 
					BGRC_P_FIXED_SHIFT_RIGHT(ulFixedLeft, BGRC_P_SCALER_STEP_FRAC_BITS - ulFixedShift)) + ulSrcPadLeft + ulSrcPadRight;

				if( BGRC_P_Blit( hGrc, NULL, NULL, false ) != BERR_SUCCESS )
				{
					BGRC_WaitForOperationReady( hGrc, NULL, NULL );
					err = BGRC_P_Blit( hGrc, NULL, NULL, false );
					if( err != BERR_SUCCESS )
						return err;
				}

				ulFixedLeft = ulFixedRight;
				ulSrcPadLeft = BGRC_P_FILTER_SRCPADLEFT[pState->eHorzCoeffs];

				/* set next strip's positions and initial phase */
				pState->SrcSurface.ulX = ulSrcX + BGRC_P_FIXED_SHIFT_RIGHT(ulFixedLeft, BGRC_P_SCALER_STEP_FRAC_BITS - ulFixedShift) - ulSrcPadLeft;
				if( bSrcYCbCr422 && (pState->SrcSurface.ulX & 1) )
				{
					pState->SrcSurface.ulX++;
					ulSrcPadLeft--;
				}
				pState->OutSurface.ulX += pState->OutSurface.ulWidth;
				pState->DstSurface.ulX += pState->OutSurface.ulWidth;
				pState->ulHorzInitPhase = ((ulFixedLeft << ulFixedShift) & BGRC_P_SCALER_STEP_FRAC_MASK) + (ulSrcPadLeft << BGRC_P_SCALER_STEP_FRAC_BITS);
			}

			/* set last strip's widths */
			pState->SrcSurface.ulWidth = ulSrcWidth + ulSrcX - pState->SrcSurface.ulX;
			pState->OutSurface.ulWidth = ulOutWidth - ii;
			pState->DstSurface.ulWidth = pState->OutSurface.ulWidth;
		}
	}

	/* blit last or only strip */
	if( BGRC_P_Blit( hGrc, pCallback, pData, bSetEvent ) != BERR_SUCCESS )
	{
		BGRC_WaitForOperationReady( hGrc, NULL, NULL );
		err = BGRC_P_Blit( hGrc, pCallback, pData, bSetEvent );
		if( err != BERR_SUCCESS )
			return err;
	}

	return BERR_SUCCESS;
}

/***************************************************************************/
/* STATE COPY FUNCTIONS                                                    */
/***************************************************************************/
void BGRC_P_Source_CopyState(
	BGRC_P_State *pDstState,
	BGRC_P_State *pSrcState,
	uint32_t aulDstRegs[],
	uint32_t aulSrcRegs[] )
{
	/* store current source registers */
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_FEEDER_ENABLE );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_SURFACE_ADDR_0 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_SURFACE_STRIDE_0 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_SURFACE_ADDR_1 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_SURFACE_STRIDE_1 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_SURFACE_FORMAT_DEF_1 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_SURFACE_FORMAT_DEF_2 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_SURFACE_FORMAT_DEF_3 );
#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_SURFACE_1_FORMAT_DEF_1 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_SURFACE_1_FORMAT_DEF_2 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_SURFACE_1_FORMAT_DEF_3 );
#endif
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_FEEDER_GRP_CNTRL, SRC_CONSTANT_COLOR );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_KEY_GRP_CNTRL, SRC_COLOR_KEY_LOW );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_KEY_GRP_CNTRL, SRC_COLOR_KEY_HIGH );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_KEY_GRP_CNTRL, SRC_COLOR_KEY_MASK );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_KEY_GRP_CNTRL, SRC_COLOR_KEY_REPLACEMENT );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_KEY_GRP_CNTRL, SRC_COLOR_KEY_REPLACEMENT_MASK );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C00_C01 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C02_C03 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C04 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C10_C11 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C12_C13 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C14 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C20_C21 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C22_C23 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C24 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C30_C31 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C32_C33 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, SRC_COLOR_MATRIX_GRP_CNTRL, SRC_CM_C34 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_SRC_TOP_LEFT );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_SRC_SIZE );
#if defined(BCHP_M2MC_BLIT_SRC_TOP_LEFT_1)
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_SRC_TOP_LEFT_1 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_SRC_SIZE_1 );
#endif
#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_0)
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_SRC_STRIPE_HEIGHT_WIDTH_0 );
#endif
#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_1)
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_SRC_STRIPE_HEIGHT_WIDTH_1 );
#endif

	/* store current source register fields */
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, SRC_COLOR_KEY_ENABLE );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, SRC_COLOR_KEY_COMPARE );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, SRC_COLOR_MATRIX_ENABLE );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, SRC_COLOR_MATRIX_ROUNDING );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, CBAR_SRC_COLOR );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, SCALER_CTRL, SCALER_ORDER );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, SCALER_CTRL, ROUNDING_MODE );

#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_ADJUST_ENABLE)
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, SCALER_CTRL, ALPHA_ADJUST );
#else
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, SCALER_CTRL, OFFSET_ADJUST );
#endif
#if defined(BCHP_M2MC_SCALER_CTRL_EDGE_CONDITION_REPLICATE)
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, SCALER_CTRL, EDGE_CONDITION );
#endif
#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_PRE_MULTIPLY_ENABLE)
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, SCALER_CTRL, ALPHA_PRE_MULTIPLY );
#endif
#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_PRE_MULTIPLY_ENABLE_ENABLE)
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, SCALER_CTRL, ALPHA_PRE_MULTIPLY_ENABLE );
#endif
#if defined(BCHP_M2MC_SCALER_CTRL_CLUT_SCALE_MODE_ENABLE)
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, SCALER_CTRL, CLUT_SCALE_MODE );
#endif

	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, HORIZ_SCALER_INITIAL_PHASE, PHASE );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, HORIZ_SCALER_STEP, STEP );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, VERT_SCALER_INITIAL_PHASE, PHASE );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, VERT_SCALER_STEP, STEP );

#if defined(BCHP_M2MC_HORIZ_SCALER_1_INITIAL_PHASE)
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, VERT_SCALER_1_INITIAL_PHASE, PHASE );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, VERT_SCALER_1_STEP, STEP );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, HORIZ_SCALER_1_INITIAL_PHASE, PHASE );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, SCALE_PARAM_GRP_CNTRL, HORIZ_SCALER_1_STEP, STEP );
#endif

	/* store current source state information */
	pDstState->SrcSurface             = pSrcState->SrcSurface;
	pDstState->SrcAlphaSurface        = pSrcState->SrcAlphaSurface;
	pDstState->SrcRect                = pSrcState->SrcRect;
	pDstState->eHorzCoeffs            = pSrcState->eHorzCoeffs;
	pDstState->eVertCoeffs            = pSrcState->eVertCoeffs;
	pDstState->pulHorzFirCoeffs       = pSrcState->pulHorzFirCoeffs;
	pDstState->pulVertFirCoeffs       = pSrcState->pulVertFirCoeffs;
	pDstState->bHorzFilter            = pSrcState->bHorzFilter;
	pDstState->bVertFilter            = pSrcState->bVertFilter;
	pDstState->bSrcPaletteBypass      = pSrcState->bSrcPaletteBypass;
	pDstState->iHorzPhaseAdj          = pSrcState->iHorzPhaseAdj;
	pDstState->iVertPhaseAdj          = pSrcState->iVertPhaseAdj;
	pDstState->ulPhaseShift           = pSrcState->ulPhaseShift;
	pDstState->ulHorzScalerNum        = pSrcState->ulHorzScalerNum;
	pDstState->ulHorzScalerDen        = pSrcState->ulHorzScalerDen;
	pDstState->ulVertScalerNum        = pSrcState->ulVertScalerNum;
	pDstState->ulVertScalerDen        = pSrcState->ulVertScalerDen;
	pDstState->ulMacroBlockRangeY     = pSrcState->ulMacroBlockRangeY;
	pDstState->ulMacroBlockRangeC     = pSrcState->ulMacroBlockRangeC;
	pDstState->ulMacroBlockStripWidth = pSrcState->ulMacroBlockStripWidth;
	pDstState->bMacroBlockLinear      = pSrcState->bMacroBlockLinear;
}

/***************************************************************************/
void BGRC_P_Destination_CopyState(
	BGRC_P_State *pDstState,
	BGRC_P_State *pSrcState,
	uint32_t aulDstRegs[],
	uint32_t aulSrcRegs[] )
{
	/* store current destination registers */
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_FEEDER_GRP_CNTRL, DEST_FEEDER_ENABLE );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_FEEDER_GRP_CNTRL, DEST_SURFACE_ADDR_0 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_FEEDER_GRP_CNTRL, DEST_SURFACE_STRIDE_0 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_FEEDER_GRP_CNTRL, DEST_SURFACE_ADDR_1 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_FEEDER_GRP_CNTRL, DEST_SURFACE_STRIDE_1 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_FEEDER_GRP_CNTRL, DEST_SURFACE_FORMAT_DEF_1 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_FEEDER_GRP_CNTRL, DEST_SURFACE_FORMAT_DEF_2 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_FEEDER_GRP_CNTRL, DEST_SURFACE_FORMAT_DEF_3 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_FEEDER_GRP_CNTRL, DEST_CONSTANT_COLOR );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_KEY_GRP_CNTRL, DEST_COLOR_KEY_LOW );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_KEY_GRP_CNTRL, DEST_COLOR_KEY_HIGH );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_KEY_GRP_CNTRL, DEST_COLOR_KEY_MASK );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_KEY_GRP_CNTRL, DEST_COLOR_KEY_REPLACEMENT );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_KEY_GRP_CNTRL, DEST_COLOR_KEY_REPLACEMENT_MASK );
#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C00_C01 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C02_C03 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C04 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C10_C11 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C12_C13 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C14 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C20_C21 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C22_C23 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C24 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C30_C31 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C32_C33 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, DST_COLOR_MATRIX_GRP_CNTRL, DEST_CM_C34 );
#endif
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_DEST_TOP_LEFT );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_DEST_SIZE );

	/* store current destination register fields */
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, DEST_COLOR_KEY_ENABLE );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, DEST_COLOR_KEY_COMPARE );
#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, DEST_COLOR_MATRIX_ENABLE );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, DEST_COLOR_MATRIX_ROUNDING );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, CBAR_DEST_COLOR );
#endif

	/* store current destination state information */
	pDstState->DstSurface        = pSrcState->DstSurface;
	pDstState->DstAlphaSurface   = pSrcState->DstAlphaSurface;
	pDstState->DstRect           = pSrcState->DstRect;
	pDstState->bDstPaletteBypass = pSrcState->bDstPaletteBypass;
}

/***************************************************************************/
void BGRC_P_Pattern_CopyState(
	BGRC_P_State *pDstState,
	BGRC_P_State *pSrcState,
	uint32_t aulDstRegs[],
	uint32_t aulSrcRegs[] )
{
	/* store current rop registers */
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, ROP_GRP_CNTRL, ROP_OPERATION );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, ROP_GRP_CNTRL, ROP_PATTERN_TOP );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, ROP_GRP_CNTRL, ROP_PATTERN_BOTTOM );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, ROP_GRP_CNTRL, ROP_PATTERN_COLOR_0 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, ROP_GRP_CNTRL, ROP_PATTERN_COLOR_1 );

	/* store current pattern state information */
	BKNI_Memcpy( pDstState->aucPattern, pSrcState->aucPattern, 8 );
}

/***************************************************************************/
void BGRC_P_Blend_CopyState(
	BGRC_P_State *pDstState,
	BGRC_P_State *pSrcState,
	uint32_t aulDstRegs[],
	uint32_t aulSrcRegs[] )
{
	BSTD_UNUSED( pDstState );
	BSTD_UNUSED( pSrcState );

	/* store current blend registers */
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLEND_PARAM_GRP_CNTRL, BLEND_CONSTANT_COLOR );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLEND_PARAM_GRP_CNTRL, BLEND_COLOR_OP );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLEND_PARAM_GRP_CNTRL, BLEND_ALPHA_OP );
}

/***************************************************************************/
void BGRC_P_Output_CopyState(
	BGRC_P_State *pDstState,
	BGRC_P_State *pSrcState,
	uint32_t aulDstRegs[],
	uint32_t aulSrcRegs[] )
{
	/* store current output registers */
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_FEEDER_GRP_CNTRL, OUTPUT_FEEDER_ENABLE );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_FEEDER_GRP_CNTRL, OUTPUT_SURFACE_ADDR_0 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_FEEDER_GRP_CNTRL, OUTPUT_SURFACE_STRIDE_0 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_FEEDER_GRP_CNTRL, OUTPUT_SURFACE_ADDR_1 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_FEEDER_GRP_CNTRL, OUTPUT_SURFACE_STRIDE_1 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_FEEDER_GRP_CNTRL, OUTPUT_SURFACE_FORMAT_DEF_1 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_FEEDER_GRP_CNTRL, OUTPUT_SURFACE_FORMAT_DEF_2 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_FEEDER_GRP_CNTRL, OUTPUT_SURFACE_FORMAT_DEF_3 );
#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C00_C01 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C02_C03 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C04 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C10_C11 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C12_C13 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C14 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C20_C21 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C22_C23 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C24 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C30_C31 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C32_C33 );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, OUTPUT_COLOR_MATRIX_GRP_CNTRL, OUTPUT_CM_C34 );
#endif
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_OUTPUT_TOP_LEFT );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_OUTPUT_SIZE );
	BGRC_P_COPY_REGISTER( pDstState, aulDstRegs, aulSrcRegs, BLEND_PARAM_GRP_CNTRL, BLEND_COLOR_KEY_ACTION );

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	/* store current output register fields */
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, OUTPUT_COLOR_MATRIX_ENABLE );
	BGRC_P_COPY_FIELD( pDstState, aulDstRegs, aulSrcRegs, BLIT_GRP_CNTRL, BLIT_HEADER, OUTPUT_COLOR_MATRIX_ROUNDING );
#endif

	/* store current output state information */
	pDstState->OutSurface      = pSrcState->OutSurface;
	pDstState->OutAlphaSurface = pSrcState->OutAlphaSurface;
	pDstState->OutRect         = pSrcState->OutRect;
}

/***************************************************************************/
/* DEBUG DUMPING FUNCTIONS                                                 */
/***************************************************************************/
void BGRC_P_PrintRegisters(
	BGRC_Handle hGrc )
{
	void *pNext = 0;

	BSTD_UNUSED( hGrc );

	BMEM_ConvertOffsetToAddress( hGrc->hMemory, BGRC_P_READ_REG( LIST_CURR_PKT_ADDR ), &pNext );

	if( pNext )
		BDBG_ERR(( "NEXT_ADDR                      %08x", *((uint32_t *) pNext) ));

	BDBG_ERR(( "LIST_CTRL                      %08x", (unsigned int) BGRC_P_READ_REG( LIST_CTRL ) ));
	BDBG_ERR(( "LIST_STATUS                    %08x", (unsigned int) BGRC_P_READ_REG( LIST_STATUS ) ));
	BDBG_ERR(( "LIST_FIRST_PKT_ADDR            %08x", (unsigned int) BGRC_P_READ_REG( LIST_FIRST_PKT_ADDR ) ));
	BDBG_ERR(( "LIST_CURR_PKT_ADDR             %08x", (unsigned int) BGRC_P_READ_REG( LIST_CURR_PKT_ADDR ) ));
	BDBG_ERR(( "BLIT_STATUS                    %08x", (unsigned int) BGRC_P_READ_REG( BLIT_STATUS ) ));
	BDBG_ERR(( "BLIT_SRC_ADDRESS               %08x", (unsigned int) BGRC_P_READ_REG( BLIT_SRC_ADDRESS ) ));
	BDBG_ERR(( "BLIT_DEST_ADDRESS              %08x", (unsigned int) BGRC_P_READ_REG( BLIT_DEST_ADDRESS ) ));
	BDBG_ERR(( "BLIT_OUTPUT_ADDRESS            %08x", (unsigned int) BGRC_P_READ_REG( BLIT_OUTPUT_ADDRESS ) ));
	BDBG_ERR(( "BLIT_MEM_HI                    %08x", (unsigned int) BGRC_P_READ_REG( BLIT_MEM_HI ) ));
	BDBG_ERR(( "BLIT_MEM_LO                    %08x", (unsigned int) BGRC_P_READ_REG( BLIT_MEM_LO ) ));
	BDBG_ERR(( "SRC_FEEDER_ENABLE              %08x", (unsigned int) BGRC_P_READ_REG( SRC_FEEDER_ENABLE ) ));
	BDBG_ERR(( "SRC_SURFACE_ADDR_0             %08x", (unsigned int) BGRC_P_READ_REG( SRC_SURFACE_ADDR_0 ) ));
	BDBG_ERR(( "SRC_SURFACE_STRIDE_0           %08x", (unsigned int) BGRC_P_READ_REG( SRC_SURFACE_STRIDE_0 ) ));
	BDBG_ERR(( "SRC_SURFACE_ADDR_1             %08x", (unsigned int) BGRC_P_READ_REG( SRC_SURFACE_ADDR_1 ) ));
	BDBG_ERR(( "SRC_SURFACE_STRIDE_1           %08x", (unsigned int) BGRC_P_READ_REG( SRC_SURFACE_STRIDE_1 ) ));
	BDBG_ERR(( "SRC_SURFACE_FORMAT_DEF_1       %08x", (unsigned int) BGRC_P_READ_REG( SRC_SURFACE_FORMAT_DEF_1 ) ));
	BDBG_ERR(( "SRC_SURFACE_FORMAT_DEF_2       %08x", (unsigned int) BGRC_P_READ_REG( SRC_SURFACE_FORMAT_DEF_2 ) ));
	BDBG_ERR(( "SRC_SURFACE_FORMAT_DEF_3       %08x", (unsigned int) BGRC_P_READ_REG( SRC_SURFACE_FORMAT_DEF_3 ) ));
#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	BDBG_ERR(( "SRC_SURFACE_1_FORMAT_DEF_1     %08x", (unsigned int) BGRC_P_READ_REG( SRC_SURFACE_1_FORMAT_DEF_1 ) ));
	BDBG_ERR(( "SRC_SURFACE_1_FORMAT_DEF_2     %08x", (unsigned int) BGRC_P_READ_REG( SRC_SURFACE_1_FORMAT_DEF_2 ) ));
	BDBG_ERR(( "SRC_SURFACE_1_FORMAT_DEF_3     %08x", (unsigned int) BGRC_P_READ_REG( SRC_SURFACE_1_FORMAT_DEF_3 ) ));
#endif
	BDBG_ERR(( "SRC_W_ALPHA                    %08x", (unsigned int) BGRC_P_READ_REG( SRC_W_ALPHA ) ));
	BDBG_ERR(( "SRC_CONSTANT_COLOR             %08x", (unsigned int) BGRC_P_READ_REG( SRC_CONSTANT_COLOR ) ));
	BDBG_ERR(( "DEST_FEEDER_ENABLE             %08x", (unsigned int) BGRC_P_READ_REG( DEST_FEEDER_ENABLE ) ));
	BDBG_ERR(( "DEST_SURFACE_ADDR_0            %08x", (unsigned int) BGRC_P_READ_REG( DEST_SURFACE_ADDR_0 ) ));
	BDBG_ERR(( "DEST_SURFACE_STRIDE_0          %08x", (unsigned int) BGRC_P_READ_REG( DEST_SURFACE_STRIDE_0 ) ));
	BDBG_ERR(( "DEST_SURFACE_ADDR_1            %08x", (unsigned int) BGRC_P_READ_REG( DEST_SURFACE_ADDR_1 ) ));
	BDBG_ERR(( "DEST_SURFACE_STRIDE_1          %08x", (unsigned int) BGRC_P_READ_REG( DEST_SURFACE_STRIDE_1 ) ));
	BDBG_ERR(( "DEST_SURFACE_FORMAT_DEF_1      %08x", (unsigned int) BGRC_P_READ_REG( DEST_SURFACE_FORMAT_DEF_1 ) ));
	BDBG_ERR(( "DEST_SURFACE_FORMAT_DEF_2      %08x", (unsigned int) BGRC_P_READ_REG( DEST_SURFACE_FORMAT_DEF_2 ) ));
	BDBG_ERR(( "DEST_SURFACE_FORMAT_DEF_3      %08x", (unsigned int) BGRC_P_READ_REG( DEST_SURFACE_FORMAT_DEF_3 ) ));
	BDBG_ERR(( "DEST_W_ALPHA                   %08x", (unsigned int) BGRC_P_READ_REG( DEST_W_ALPHA ) ));
	BDBG_ERR(( "DEST_CONSTANT_COLOR            %08x", (unsigned int) BGRC_P_READ_REG( DEST_CONSTANT_COLOR ) ));
	BDBG_ERR(( "OUTPUT_FEEDER_ENABLE           %08x", (unsigned int) BGRC_P_READ_REG( OUTPUT_FEEDER_ENABLE ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_ADDR_0          %08x", (unsigned int) BGRC_P_READ_REG( OUTPUT_SURFACE_ADDR_0 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_STRIDE_0        %08x", (unsigned int) BGRC_P_READ_REG( OUTPUT_SURFACE_STRIDE_0 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_ADDR_1          %08x", (unsigned int) BGRC_P_READ_REG( OUTPUT_SURFACE_ADDR_1 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_STRIDE_1        %08x", (unsigned int) BGRC_P_READ_REG( OUTPUT_SURFACE_STRIDE_1 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_FORMAT_DEF_1    %08x", (unsigned int) BGRC_P_READ_REG( OUTPUT_SURFACE_FORMAT_DEF_1 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_FORMAT_DEF_2    %08x", (unsigned int) BGRC_P_READ_REG( OUTPUT_SURFACE_FORMAT_DEF_2 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_FORMAT_DEF_3    %08x", (unsigned int) BGRC_P_READ_REG( OUTPUT_SURFACE_FORMAT_DEF_3 ) ));
	BDBG_ERR(( "BLIT_HEADER                    %08x", (unsigned int) BGRC_P_READ_REG( BLIT_HEADER ) ));
	BDBG_ERR(( "BLIT_SRC_TOP_LEFT              %08x", (unsigned int) BGRC_P_READ_REG( BLIT_SRC_TOP_LEFT ) ));
	BDBG_ERR(( "BLIT_SRC_SIZE                  %08x", (unsigned int) BGRC_P_READ_REG( BLIT_SRC_SIZE ) ));
#if defined(BCHP_M2MC_BLIT_SRC_TOP_LEFT_1)
	BDBG_ERR(( "BLIT_SRC_TOP_LEFT_1            %08x", (unsigned int) BGRC_P_READ_REG( BLIT_SRC_TOP_LEFT_1 ) ));
	BDBG_ERR(( "BLIT_SRC_SIZE_1                %08x", (unsigned int) BGRC_P_READ_REG( BLIT_SRC_SIZE_1 ) ));
#endif
#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_0)
#endif
	BDBG_ERR(( "BLIT_SRC_STRIPE_HEIGHT_WIDTH_0 %08x", (unsigned int) BGRC_P_READ_REG( BLIT_SRC_STRIPE_HEIGHT_WIDTH_0 ) ));
#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_1)
	BDBG_ERR(( "BLIT_SRC_STRIPE_HEIGHT_WIDTH_1 %08x", (unsigned int) BGRC_P_READ_REG( BLIT_SRC_STRIPE_HEIGHT_WIDTH_1 ) ));
#endif
	BDBG_ERR(( "BLIT_DEST_TOP_LEFT             %08x", (unsigned int) BGRC_P_READ_REG( BLIT_DEST_TOP_LEFT ) ));
	BDBG_ERR(( "BLIT_DEST_SIZE                 %08x", (unsigned int) BGRC_P_READ_REG( BLIT_DEST_SIZE ) ));
	BDBG_ERR(( "BLIT_OUTPUT_TOP_LEFT           %08x", (unsigned int) BGRC_P_READ_REG( BLIT_OUTPUT_TOP_LEFT ) ));
	BDBG_ERR(( "BLIT_OUTPUT_SIZE               %08x", (unsigned int) BGRC_P_READ_REG( BLIT_OUTPUT_SIZE ) ));
	BDBG_ERR(( "BLIT_INPUT_STRIPE_WIDTH        %08x", (unsigned int) BGRC_P_READ_REG( BLIT_INPUT_STRIPE_WIDTH ) ));
#if defined(BCHP_M2MC_BLIT_INPUT_STRIPE_WIDTH_1)
	BDBG_ERR(( "BLIT_INPUT_STRIPE_WIDTH_1      %08x", (unsigned int) BGRC_P_READ_REG( BLIT_INPUT_STRIPE_WIDTH_1 ) ));
#endif
	BDBG_ERR(( "BLIT_OUTPUT_STRIPE_WIDTH       %08x", (unsigned int) BGRC_P_READ_REG( BLIT_OUTPUT_STRIPE_WIDTH ) ));
	BDBG_ERR(( "BLIT_STRIPE_OVERLAP            %08x", (unsigned int) BGRC_P_READ_REG( BLIT_STRIPE_OVERLAP ) ));
#if defined(BCHP_M2MC_BLIT_STRIPE_OVERLAP_1)
	BDBG_ERR(( "BLIT_STRIPE_OVERLAP_1          %08x", (unsigned int) BGRC_P_READ_REG( BLIT_STRIPE_OVERLAP_1 ) ));
#endif
	BDBG_ERR(( "BLIT_CTRL                      %08x", (unsigned int) BGRC_P_READ_REG( BLIT_CTRL ) ));
	BDBG_ERR(( "SCALER_CTRL                    %08x", (unsigned int) BGRC_P_READ_REG( SCALER_CTRL ) ));
	BDBG_ERR(( "HORIZ_AVERAGER_COUNT           %08x", (unsigned int) BGRC_P_READ_REG( HORIZ_AVERAGER_COUNT ) ));
	BDBG_ERR(( "HORIZ_AVERAGER_COEFF           %08x", (unsigned int) BGRC_P_READ_REG( HORIZ_AVERAGER_COEFF ) ));
	BDBG_ERR(( "VERT_AVERAGER_COUNT            %08x", (unsigned int) BGRC_P_READ_REG( VERT_AVERAGER_COUNT ) ));
	BDBG_ERR(( "VERT_AVERAGER_COEFF            %08x", (unsigned int) BGRC_P_READ_REG( VERT_AVERAGER_COEFF ) ));
	BDBG_ERR(( "HORIZ_SCALER_INITIAL_PHASE     %08x", (unsigned int) BGRC_P_READ_REG( HORIZ_SCALER_INITIAL_PHASE ) ));
	BDBG_ERR(( "HORIZ_SCALER_STEP              %08x", (unsigned int) BGRC_P_READ_REG( HORIZ_SCALER_STEP ) ));
#if defined(BCHP_M2MC_HORIZ_SCALER_1_INITIAL_PHASE)
	BDBG_ERR(( "HORIZ_SCALER_1_INITIAL_PHASE   %08x", (unsigned int) BGRC_P_READ_REG( HORIZ_SCALER_1_INITIAL_PHASE ) ));
	BDBG_ERR(( "HORIZ_SCALER_1_STEP            %08x", (unsigned int) BGRC_P_READ_REG( HORIZ_SCALER_1_STEP ) ));
#endif
	BDBG_ERR(( "VERT_SCALER_INITIAL_PHASE      %08x", (unsigned int) BGRC_P_READ_REG( VERT_SCALER_INITIAL_PHASE ) ));
	BDBG_ERR(( "VERT_SCALER_STEP               %08x", (unsigned int) BGRC_P_READ_REG( VERT_SCALER_STEP ) ));
#if defined(BCHP_M2MC_VERT_SCALER_1_INITIAL_PHASE)
	BDBG_ERR(( "VERT_SCALER_1_INITIAL_PHASE    %08x", (unsigned int) BGRC_P_READ_REG( VERT_SCALER_1_INITIAL_PHASE ) ));
	BDBG_ERR(( "VERT_SCALER_1_STEP             %08x", (unsigned int) BGRC_P_READ_REG( VERT_SCALER_1_STEP ) ));
#endif
	BDBG_ERR(( "BLEND_COLOR_OP                 %08x", (unsigned int) BGRC_P_READ_REG( BLEND_COLOR_OP ) ));
	BDBG_ERR(( "BLEND_ALPHA_OP                 %08x", (unsigned int) BGRC_P_READ_REG( BLEND_ALPHA_OP ) ));
	BDBG_ERR(( "BLEND_CONSTANT_COLOR           %08x", (unsigned int) BGRC_P_READ_REG( BLEND_CONSTANT_COLOR ) ));
	BDBG_ERR(( "BLEND_COLOR_KEY_ACTION         %08x", (unsigned int) BGRC_P_READ_REG( BLEND_COLOR_KEY_ACTION ) ));
	BDBG_ERR(( "SRC_CM_C00_C01                 %08x", (unsigned int) BGRC_P_READ_REG( SRC_CM_C00_C01 ) ));
	BDBG_ERR(( "SRC_CM_C02_C03                 %08x", (unsigned int) BGRC_P_READ_REG( SRC_CM_C02_C03 ) ));
	BDBG_ERR(( "SRC_CM_C04                     %08x", (unsigned int) BGRC_P_READ_REG( SRC_CM_C04 ) ));
	BDBG_ERR(( "SRC_CM_C10_C11                 %08x", (unsigned int) BGRC_P_READ_REG( SRC_CM_C10_C11 ) ));
	BDBG_ERR(( "SRC_CM_C12_C13                 %08x", (unsigned int) BGRC_P_READ_REG( SRC_CM_C12_C13 ) ));
	BDBG_ERR(( "SRC_CM_C14                     %08x", (unsigned int) BGRC_P_READ_REG( SRC_CM_C14 ) ));
	BDBG_ERR(( "SRC_CM_C20_C21                 %08x", (unsigned int) BGRC_P_READ_REG( SRC_CM_C20_C21 ) ));
	BDBG_ERR(( "SRC_CM_C22_C23                 %08x", (unsigned int) BGRC_P_READ_REG( SRC_CM_C22_C23 ) ));
	BDBG_ERR(( "SRC_CM_C24                     %08x", (unsigned int) BGRC_P_READ_REG( SRC_CM_C24 ) ));
#if ((BCHP_CHIP==3548) && (BCHP_VER >= BCHP_VER_B0)) || \
	((BCHP_CHIP==3556) && (BCHP_VER >= BCHP_VER_B0))
	BDBG_ERR(( "DCE_PRED_CFG                   %08x", (unsigned int) BGRC_P_READ_REG( DCE_PRED_CFG ) ));
	BDBG_ERR(( "DCE_COMPR_CFG1                 %08x", (unsigned int) BGRC_P_READ_REG( DCE_COMPR_CFG1 ) ));
	BDBG_ERR(( "DCE_COMPR_CFG2                 %08x", (unsigned int) BGRC_P_READ_REG( DCE_COMPR_CFG2 ) ));
	BDBG_ERR(( "DCE_RANGE0                     %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE0 ) ));
	BDBG_ERR(( "DCE_RANGE1                     %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE1 ) ));
	BDBG_ERR(( "DCE_RANGE2                     %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE2 ) ));
	BDBG_ERR(( "DCE_RANGE3                     %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE3 ) ));
	BDBG_ERR(( "DCE_RANGE4                     %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE4 ) ));
	BDBG_ERR(( "DCE_RANGE5                     %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE5 ) ));
	BDBG_ERR(( "DCE_RANGE6                     %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE6 ) ));
	BDBG_ERR(( "DCE_RANGE7                     %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE7 ) ));
	BDBG_ERR(( "DCE_RANGE8                     %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE8 ) ));
	BDBG_ERR(( "DCE_RANGE9                     %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE9 ) ));
	BDBG_ERR(( "DCE_RANGE10                    %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE10 ) ));
	BDBG_ERR(( "DCE_RANGE11                    %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE11 ) ));
	BDBG_ERR(( "DCE_RANGE12                    %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE12 ) ));
	BDBG_ERR(( "DCE_RANGE13                    %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE13 ) ));
	BDBG_ERR(( "DCE_RANGE14                    %08x", (unsigned int) BGRC_P_READ_REG( DCE_RANGE14 ) ));
	BDBG_ERR(( "DCE_VIDEO_CFG                  %08x", (unsigned int) BGRC_P_READ_REG( DCE_VIDEO_CFG ) ));
	BDBG_ERR(( "DCE_DIAG                       %08x", (unsigned int) BGRC_P_READ_REG( DCE_DIAG ) ));
#endif
	BDBG_ERR(( "" ));
}

/***************************************************************************/
/* LIST PROCESSING FUNCTIONS                                               */
/***************************************************************************/
static const uint8_t BGRC_P_LIST_GROUP_COUNT[] = 
{
	BGRC_P_LIST_DST_CLUT_GRP_CNTRL_COUNT,
	BGRC_P_LIST_SRC_CLUT_GRP_CNTRL_COUNT,
	BGRC_P_LIST_OUTPUT_COLOR_MATRIX_GRP_CNTRL_COUNT,
	BGRC_P_LIST_DST_COLOR_MATRIX_GRP_CNTRL_COUNT,
	BGRC_P_LIST_SRC_COLOR_MATRIX_GRP_CNTRL_COUNT,
	BGRC_P_LIST_SCALE_COEF_GRP_CNTRL_COUNT,
	BGRC_P_LIST_DST_COLOR_KEY_GRP_CNTRL_COUNT,
	BGRC_P_LIST_SRC_COLOR_KEY_GRP_CNTRL_COUNT,
	BGRC_P_LIST_ROP_GRP_CNTRL_COUNT,
	BGRC_P_LIST_BLEND_PARAM_GRP_CNTRL_COUNT,
	BGRC_P_LIST_SCALE_PARAM_GRP_CNTRL_COUNT,
	BGRC_P_LIST_BLIT_GRP_CNTRL_COUNT,
	BGRC_P_LIST_OUTPUT_FEEDER_GRP_CNTRL_COUNT,
	BGRC_P_LIST_DST_FEEDER_GRP_CNTRL_COUNT,
	BGRC_P_LIST_SRC_FEEDER_GRP_CNTRL_COUNT
};

/***************************************************************************/
static bool BGRC_P_IsBlockDone( 
	BGRC_Handle hGrc,
	BGRC_P_Block *pBlock )
{
	if( pBlock->ulRefCount == 0 )
	{
		uint32_t ulDeviceOffset = BGRC_P_READ_REG( LIST_CURR_PKT_ADDR ) & BGRC_M2MC(LIST_CURR_PKT_ADDR_CURR_PKT_ADDR_MASK);

		if( (ulDeviceOffset < pBlock->ulOffset) || (ulDeviceOffset >= pBlock->ulOffset + BGRC_P_LIST_BLOCK_SIZE) )
			return true;
	}

	return false;
}

/***************************************************************************/
#define BGRC_P_GET_PACKET_SIZE( ulHeader, ulSize ) \
{ \
	uint32_t ii; \
	ulSize = 0; \
	for( ii = 0; ii < BGRC_P_GROUP_COUNT; ++ii ) \
		ulSize += (((ulHeader) >> ii) & 1) ? BGRC_P_LIST_GROUP_COUNT[ii] * sizeof (uint32_t) : 0; \
	ulSize += (BGRC_P_HEADER_COUNT + BGRC_P_USERDATA_COUNT) * sizeof (uint32_t); \
}

/***************************************************************************/
#define BGRC_P_ALIGN_PACKET_SIZE( ulSize ) \
{ \
	ulSize += (1 << BGRC_P_LIST_BLOCK_ALIGN) - 1; \
	ulSize &= ~((1 << BGRC_P_LIST_BLOCK_ALIGN) - 1); \
}

/***************************************************************************/
bool BGRC_P_List_InitPacketMemory(
	BGRC_Handle hGrc,
	uint32_t ulMemorySize )
{
#if 1
	BGRC_P_Block *pCurrBlock = NULL;
	BGRC_P_Block *pPrevBlock = NULL;
	uint32_t ulBlockCount = (ulMemorySize + BGRC_P_LIST_BLOCK_SIZE - 1) / BGRC_P_LIST_BLOCK_SIZE;
	uint32_t ii;

	for( ii = 0; ii < ulBlockCount; ++ii )
	{
		/* create memory block */
		pCurrBlock = BGRC_P_List_CreateBlock( hGrc );
		if( pCurrBlock == NULL )
			return false;

		/* attach to previous block */
		if( pPrevBlock )
			pPrevBlock->pNextBlock = pCurrBlock;
		else
			hGrc->pCurrListBlock = pCurrBlock;

		pPrevBlock = pCurrBlock;
	}

	/* set last block to point to first block */
	if( pCurrBlock )
		pCurrBlock->pNextBlock = hGrc->pCurrListBlock;

	hGrc->pPrevListBlock = pCurrBlock;
#else
	/* create memory block */
	hGrc->pCurrListBlock = BGRC_P_List_CreateBlock( hGrc );
	if( hGrc->pCurrListBlock == NULL )
		return false;

	/* attach block to itself */
	hGrc->pCurrListBlock->pNextBlock = hGrc->pCurrListBlock;
#endif
	hGrc->ulListBlockPos = 0;

	return true;
}

/***************************************************************************/
void BGRC_P_List_FreePacketMemory(
	BGRC_Handle hGrc )
{
	/* free all list blocks */
	BGRC_P_Block *pBlock = hGrc->pCurrListBlock;
	while( pBlock )
	{
		pBlock = BGRC_P_List_DestroyBlock( hGrc, pBlock );
	}
}

/***************************************************************************/
void BGRC_P_List_CleanupPacketMemory(
	BGRC_Handle hGrc )
{
	BGRC_P_Block *pBlock = hGrc->pCurrListBlock;

	/* free packet memory blocks while total allocation size is greater than max */
	while( pBlock && hGrc->ulPacketMemoryMax && 
		(hGrc->ulPacketMemorySize > hGrc->ulPacketMemoryMax) && BGRC_P_IsBlockDone( hGrc, pBlock ) )
	{
		pBlock = BGRC_P_List_DestroyBlock( hGrc, pBlock );
	}
}

/***************************************************************************/
static uint32_t *BGRC_P_List_AllocPacket(
	BGRC_Handle hGrc,
	uint32_t ulPacketHeader,
	uint32_t *pulPacketOffset,
	uint32_t *pulPacketSize,
	BGRC_P_Block **ppBlock )
{
	uint32_t *pulPacket;
	uint32_t ulPacketSize;

	/* get packet size */
	BGRC_P_GET_PACKET_SIZE( ulPacketHeader, ulPacketSize );
	BGRC_P_ALIGN_PACKET_SIZE( ulPacketSize );

	/* check if current memory block is full */
	if( hGrc->ulListBlockPos + ulPacketSize > BGRC_P_LIST_BLOCK_SIZE )
	{
		/* if next block is busy, check if it can be set to not busy */
		BGRC_P_Block *pCurrBlock = hGrc->pCurrListBlock->pNextBlock;
		if( pCurrBlock->bBusy && BGRC_P_IsBlockDone( hGrc, pCurrBlock ) )
			pCurrBlock->bBusy = false;

		/* check if next block is still busy */
		if( pCurrBlock->bBusy )
		{
			/* return error if memory is pre-allocated */
			if( hGrc->bPreAllocMemory )
				return NULL;
			
			/* create new memory block */
			pCurrBlock = BGRC_P_List_CreateBlock( hGrc );
			if( pCurrBlock == NULL )
				return NULL;

			/* attach block to list */
			pCurrBlock->pNextBlock = hGrc->pCurrListBlock->pNextBlock;
			hGrc->pCurrListBlock->pNextBlock = pCurrBlock;
		}

		/* clear new block */
		hGrc->pPrevListBlock = hGrc->pCurrListBlock;
		hGrc->pCurrListBlock = pCurrBlock;
		hGrc->ulListBlockPos = 0;
		pCurrBlock->bBusy = true;
	}

	/* get packet address */
#if 1
	pulPacket = (uint32_t *) ((uint8_t *) hGrc->pCurrListBlock->pvCached + hGrc->ulListBlockPos);
#else
	pulPacket = (uint32_t *) ((uint8_t *) hGrc->pCurrListBlock->pvMemory + hGrc->ulListBlockPos);
#endif
	*pulPacketOffset = hGrc->pCurrListBlock->ulOffset + hGrc->ulListBlockPos;
	*pulPacketSize = ulPacketSize;

	hGrc->ulListBlockPos += ulPacketSize;
	hGrc->pCurrListBlock->ulRefCount++;

	hGrc->ulPacketMemorySinceInterrupt += ulPacketSize;

	*ppBlock = hGrc->pCurrListBlock;
	return pulPacket;
}

/***************************************************************************/
static BERR_Code BGRC_P_List_PreparePacket(
	BGRC_Handle hGrc,
	bool bEnableInterrupt )
{
	BGRC_P_State *pState = &hGrc->CurrentState;

	/* set W alpha for alpha channel expansion for ARGB1555 formats */
	if( pState->SrcSurface.hSurface && (!BPXL_IS_WINDOW_FORMAT(pState->SrcSurface.eFormat)) &&
		BGRC_P_COMPARE_VALUE( SRC_SURFACE_FORMAT_DEF_1, FORMAT_TYPE, 1 ) )
	{
		BGRC_P_SET_FIELD_DATA( SRC_W_ALPHA, W0_ALPHA, 0 );
		BGRC_P_SET_FIELD_DATA( SRC_W_ALPHA, W1_ALPHA, 
			BGRC_P_COMPARE_FIELD( SRC_SURFACE_FORMAT_DEF_3, ZERO_PAD, REPLICATE ) ? 0xFF : 0x80 );
		if( BGRC_P_REGISTER_CHANGED( SRC_W_ALPHA ) )
			BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
	}

	if( pState->DstSurface.hSurface && (!BPXL_IS_WINDOW_FORMAT(pState->DstSurface.eFormat)) &&
		BGRC_P_COMPARE_VALUE( DEST_SURFACE_FORMAT_DEF_1, FORMAT_TYPE, 1 ) )
	{
		BGRC_P_SET_FIELD_DATA( DEST_W_ALPHA, W0_ALPHA, 0 );
		BGRC_P_SET_FIELD_DATA( DEST_W_ALPHA, W1_ALPHA, 
			BGRC_P_COMPARE_FIELD( DEST_SURFACE_FORMAT_DEF_3, ZERO_PAD, REPLICATE ) ? 0xFF : 0x80 );
		if( BGRC_P_REGISTER_CHANGED( DEST_W_ALPHA ) )
			BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
	}

	/* set rectangle fields */
	BGRC_P_SET_SURFACE_DIMENSIONS( SRC, pState->SrcSurface.hSurface, 
		pState->SrcSurface.ulX, pState->SrcSurface.ulY, pState->SrcSurface.ulWidth, pState->SrcSurface.ulHeight );
	BGRC_P_SET_SURFACE_DIMENSIONS( DEST, pState->DstSurface.hSurface, 
		pState->DstSurface.ulX, pState->DstSurface.ulY, pState->DstSurface.ulWidth, pState->DstSurface.ulHeight );
	BGRC_P_SET_SURFACE_DIMENSIONS( OUTPUT, pState->OutSurface.hSurface, 
		pState->OutSurface.ulX, pState->OutSurface.ulY, pState->OutSurface.ulWidth, pState->OutSurface.ulHeight );

#if defined(BCHP_M2MC_BLIT_SRC_TOP_LEFT_1)
	/* set rectangle registers for YCbCr 420 format */
	if( hGrc->bYCbCr420Source )
	{
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_TOP_LEFT_1, LEFT, pState->SrcSurface.ulX / 2 );
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_TOP_LEFT_1, TOP, pState->SrcSurface.ulY / 2 );
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_SIZE_1, SURFACE_WIDTH, pState->SrcSurface.ulWidth / 2 );
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_SIZE_1, SURFACE_HEIGHT, pState->SrcSurface.ulHeight / 2 );
	}
	else
	{
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_TOP_LEFT_1, LEFT, 0 );
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_TOP_LEFT_1, TOP, 0 );
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_SIZE_1, SURFACE_WIDTH, 0 );
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_SIZE_1, SURFACE_HEIGHT, 0 );
	}
#endif

	/* check if there is no source surface */
	if( pState->SrcSurface.hSurface == 0 )
	{
		/* setup for source fill */
		BGRC_P_SET_FIELD_ENUM( SRC_FEEDER_ENABLE, ENABLE, ENABLE );
		BGRC_P_SET_FIELD_CHANNELS( SRC, BPXL_eA8_R8_G8_B8, pState->OutSurface.hSurface );

		/* set source width and height registers to same as output */
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_SIZE, SURFACE_WIDTH, BGRC_P_GET_FIELD_DATA(BLIT_OUTPUT_SIZE, SURFACE_WIDTH) );
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_SIZE, SURFACE_HEIGHT, BGRC_P_GET_FIELD_DATA(BLIT_OUTPUT_SIZE, SURFACE_HEIGHT) );

		/* set register load fields */
		if( BGRC_P_REGISTER_CHANGED( SRC_FEEDER_ENABLE ) )
			BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );

		if( BGRC_P_REGISTER_CHANGED( BLIT_SRC_SIZE ) )
			BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
	}

	/* check if there is no destination surface */
	{
		if( pState->DstSurface.hSurface == 0 )
		{
			/* setup for destination fill */
			BGRC_P_SET_FIELD_ENUM( DEST_FEEDER_ENABLE, ENABLE, ENABLE );
			BGRC_P_SET_FIELD_CHANNELS( DEST, BPXL_eA8_R8_G8_B8, pState->OutSurface.hSurface );

			/* set destination width and height registers to same as output */
			BGRC_P_SET_FIELD_DATA( BLIT_DEST_SIZE, SURFACE_WIDTH, BGRC_P_GET_FIELD_DATA(BLIT_OUTPUT_SIZE, SURFACE_WIDTH) );
			BGRC_P_SET_FIELD_DATA( BLIT_DEST_SIZE, SURFACE_HEIGHT, BGRC_P_GET_FIELD_DATA(BLIT_OUTPUT_SIZE, SURFACE_HEIGHT) );

			/* set register load fields */
			if( BGRC_P_REGISTER_CHANGED( DEST_FEEDER_ENABLE ) )
				BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );

			if( BGRC_P_REGISTER_CHANGED( BLIT_DEST_SIZE ) )
				BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
		}
	}

	/* set blit direction fields */
	if( pState->bSrcRightToLeft || pState->bSrcBottomToTop ||
		pState->bDstRightToLeft || pState->bDstBottomToTop ||
		pState->bOutRightToLeft || pState->bOutBottomToTop )
	{
		BGRC_P_SET_FIELD_COMP( BLIT_CTRL, SRC_H_DIRECTION, RIGHT_TO_LEFT, LEFT_TO_RIGHT, pState->bSrcRightToLeft );
		BGRC_P_SET_FIELD_COMP( BLIT_CTRL, SRC_V_DIRECTION, BOTTOM_TO_TOP, TOP_TO_BOTTOM, pState->bSrcBottomToTop );
		BGRC_P_SET_FIELD_COMP( BLIT_CTRL, DEST_H_DIRECTION, RIGHT_TO_LEFT, LEFT_TO_RIGHT, pState->bDstRightToLeft );
		BGRC_P_SET_FIELD_COMP( BLIT_CTRL, DEST_V_DIRECTION, BOTTOM_TO_TOP, TOP_TO_BOTTOM, pState->bDstBottomToTop );
		BGRC_P_SET_FIELD_COMP( BLIT_CTRL, OUTPUT_H_DIRECTION, RIGHT_TO_LEFT, LEFT_TO_RIGHT, pState->bOutRightToLeft );
		BGRC_P_SET_FIELD_COMP( BLIT_CTRL, OUTPUT_V_DIRECTION, BOTTOM_TO_TOP, TOP_TO_BOTTOM, pState->bOutBottomToTop );
	}
	else if( pState->SrcSurface.hSurface && BGRC_P_SURFACE_INTERSECT( SRC, OUTPUT ) )
	{
		BGRC_P_SET_FIELD_DIRECTION( SRC, OUTPUT, DEST, H, LEFT, LEFT_TO_RIGHT, RIGHT_TO_LEFT );
		BGRC_P_SET_FIELD_DIRECTION( SRC, OUTPUT, DEST, V, TOP, TOP_TO_BOTTOM, BOTTOM_TO_TOP );
	}
	else if( pState->DstSurface.hSurface && BGRC_P_SURFACE_INTERSECT( DEST, OUTPUT ) )
	{
		BGRC_P_SET_FIELD_DIRECTION( DEST, OUTPUT, SRC, H, LEFT, LEFT_TO_RIGHT, RIGHT_TO_LEFT );
		BGRC_P_SET_FIELD_DIRECTION( DEST, OUTPUT, SRC, V, TOP, TOP_TO_BOTTOM, BOTTOM_TO_TOP );
	}
	else
	{
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, SRC_H_DIRECTION, LEFT_TO_RIGHT );
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, SRC_V_DIRECTION, TOP_TO_BOTTOM );
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, DEST_H_DIRECTION, LEFT_TO_RIGHT );
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, DEST_V_DIRECTION, TOP_TO_BOTTOM );
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, OUTPUT_H_DIRECTION, LEFT_TO_RIGHT );
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, OUTPUT_V_DIRECTION, TOP_TO_BOTTOM );
	}

	/* set scaler fields */
	if( pState->SrcSurface.hSurface && (hGrc->bNoScaleFilter || 
		(pState->ulHorzScalerStep != (1 << BGRC_P_SCALER_STEP_FRAC_BITS)) ||
		(pState->ulVertScalerStep != (1 << BGRC_P_SCALER_STEP_FRAC_BITS))) )
	{
		/* get fir filter coefficient tables */
		const uint32_t *pulHorzFirCoeffs = BGRC_P_GET_FILTER_COEFFICIENTS[pState->eHorzCoeffs]( pState->ulHorzScalerStep );
		const uint32_t *pulVertFirCoeffs = BGRC_P_GET_FILTER_COEFFICIENTS[pState->eVertCoeffs]( pState->ulVertScalerStep );

		/* check if scaler step has changed */
		if( (pState->pulHorzFirCoeffs != pulHorzFirCoeffs) ||
			(pState->pulVertFirCoeffs != pulVertFirCoeffs) )
		{
#if defined(BCHP_M2MC_SCALER_CTRL_CLUT_SCALE_MODE_ENABLE)
			BGRC_P_SET_FIELD_COMP( SCALER_CTRL, CLUT_SCALE_MODE, ENABLE, DISABLE, 
				(pState->eHorzCoeffs == BGRC_FilterCoeffs_ePointSample) && 
				(pState->eVertCoeffs == BGRC_FilterCoeffs_ePointSample) );
#endif

			BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SCALE_COEF_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
			pState->pulHorzFirCoeffs = pulHorzFirCoeffs;
			pState->pulVertFirCoeffs = pulVertFirCoeffs;
		}

		/* set scaler enable and scale order */
		BGRC_P_SET_FIELD_ENUM( BLIT_HEADER, SRC_SCALER_ENABLE, ENABLE );
		BGRC_P_SET_FIELD_COMP( SCALER_CTRL, SCALER_ORDER, VERT_THEN_HORIZ, HORIZ_THEN_VERT, 
			pState->ulHorzScalerStep < (1 << BGRC_P_SCALER_STEP_FRAC_BITS) );

		/* set horizontal scaler */
		BGRC_P_SET_FIELD_ENUM( SCALER_CTRL, HORIZ_SCALER_ENABLE, ENABLE );
		BGRC_P_SET_FIELD_DATA( HORIZ_SCALER_INITIAL_PHASE, PHASE, pState->ulHorzInitPhase / pState->ulHorzAveragerCount )
		BGRC_P_SET_FIELD_DATA( HORIZ_SCALER_STEP, STEP, pState->ulHorzScalerStep / pState->ulHorzAveragerCount );

#if defined(BCHP_M2MC_HORIZ_SCALER_1_INITIAL_PHASE)
		/* set scaler registers for YCbCr 420 format */
		if( hGrc->bYCbCr420Source )
		{
			BGRC_P_SET_FIELD_DATA( HORIZ_SCALER_1_INITIAL_PHASE, PHASE, pState->ulHorzInitPhase / 2 );/*- 
				pState->ulHorzScalerStep / 2 );*/
			BGRC_P_SET_FIELD_DATA( HORIZ_SCALER_1_STEP, STEP, pState->ulHorzScalerStep / 2 );
		}
#endif

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
		/* set horizontal averager */
		BGRC_P_SET_FIELD_COMP( SCALER_CTRL, HORIZ_AVERAGER_ENABLE, ENABLE, DISABLE, 
			pState->ulHorzAveragerCount > 1 );
		BGRC_P_SET_FIELD_DATA( HORIZ_AVERAGER_COUNT, COUNT, pState->ulHorzAveragerCount );
		BGRC_P_SET_FIELD_DATA( HORIZ_AVERAGER_COEFF, COEFF, pState->ulHorzAveragerCoeff );
#endif

		/* check if striping */
		if( (pState->SrcSurface.ulWidth > BGRC_P_STRIP_WIDTH_MAX) && (pState->OutSurface.ulWidth > BGRC_P_STRIP_WIDTH_MAX) )
		{
			/* set striping */
			BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, STRIPE_ENABLE, ENABLE );
			BGRC_P_SET_FIELD_DATA( BLIT_INPUT_STRIPE_WIDTH, STRIPE_WIDTH, pState->ulSrcStripWidth );
			BGRC_P_SET_FIELD_DATA( BLIT_OUTPUT_STRIPE_WIDTH, STRIPE_WIDTH, pState->ulOutStripWidth );
			BGRC_P_SET_FIELD_DATA( BLIT_STRIPE_OVERLAP, STRIPE_WIDTH, pState->ulOverlapStrip );

#if defined(BCHP_M2MC_BLIT_INPUT_STRIPE_WIDTH_1)
			if( hGrc->bYCbCr420Source )
			{
				BGRC_P_SET_FIELD_DATA( BLIT_INPUT_STRIPE_WIDTH_1, STRIPE_WIDTH, pState->ulSrcStripWidth / 2 );
				BGRC_P_SET_FIELD_DATA( BLIT_STRIPE_OVERLAP_1, STRIPE_WIDTH, pState->ulOverlapStrip );
			}
#endif

			/* disable dst feeder when striping enabled */
			BGRC_P_SET_FIELD_ENUM( DEST_FEEDER_ENABLE, ENABLE, DISABLE );
			
			if( BGRC_P_REGISTER_CHANGED( DEST_FEEDER_ENABLE ) )
				BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );

			/* set register load field */
			if( BGRC_P_REGISTER_CHANGED( BLIT_INPUT_STRIPE_WIDTH ) ||
				BGRC_P_REGISTER_CHANGED( BLIT_OUTPUT_STRIPE_WIDTH ) ||
				BGRC_P_REGISTER_CHANGED( BLIT_STRIPE_OVERLAP ) )
				BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
		}
		else
		{
			/* disable striping */
			BGRC_P_SET_FIELD_ENUM ( BLIT_CTRL, STRIPE_ENABLE, DISABLE );
		}

		/* check if vertical scaling */
		if( pState->bVertFilter || (pState->ulVertScalerStep != (1 << BGRC_P_SCALER_STEP_FRAC_BITS)) || hGrc->bYCbCr420Source )
		{
			/* set vertical scaler */
			BGRC_P_SET_FIELD_ENUM( SCALER_CTRL, VERT_SCALER_ENABLE, ENABLE );
			BGRC_P_SET_FIELD_DATA( VERT_SCALER_INITIAL_PHASE, PHASE, pState->ulVertInitPhase / pState->ulVertAveragerCount );
			BGRC_P_SET_FIELD_DATA( VERT_SCALER_STEP, STEP, pState->ulVertScalerStep / pState->ulVertAveragerCount );

#if defined(BCHP_M2MC_VERT_SCALER_1_INITIAL_PHASE)
		/* set scaler registers for YCbCr 420 format */
		if( hGrc->bYCbCr420Source )
		{
			BGRC_P_SET_FIELD_DATA( VERT_SCALER_1_INITIAL_PHASE, PHASE, pState->ulVertInitPhase / 2 );/*- 
				pState->ulVertScalerStep / 2 );*/
			BGRC_P_SET_FIELD_DATA( VERT_SCALER_1_STEP, STEP, pState->ulVertScalerStep / 2 );
		}
#endif

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
			/* set vertical averager */
			BGRC_P_SET_FIELD_COMP( SCALER_CTRL, VERT_AVERAGER_ENABLE, ENABLE, DISABLE, 
				pState->ulVertAveragerCount > 1 );
			BGRC_P_SET_FIELD_DATA( VERT_AVERAGER_COUNT, COUNT, pState->ulVertAveragerCount );
			BGRC_P_SET_FIELD_DATA( VERT_AVERAGER_COEFF, COEFF, pState->ulVertAveragerCoeff );
#endif
		}
		else
		{
			/* disable vertical scaling and averagering */
			BGRC_P_SET_FIELD_ENUM( SCALER_CTRL, VERT_SCALER_ENABLE, DISABLE );
#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
			BGRC_P_SET_FIELD_ENUM( SCALER_CTRL, VERT_AVERAGER_ENABLE, DISABLE );
#endif
		}

		/* set register load field */
		if( BGRC_P_REGISTER_CHANGED( SCALER_CTRL ) ||
			BGRC_P_REGISTER_CHANGED( HORIZ_AVERAGER_COUNT ) ||
			BGRC_P_REGISTER_CHANGED( HORIZ_AVERAGER_COEFF ) ||
			BGRC_P_REGISTER_CHANGED( HORIZ_SCALER_INITIAL_PHASE ) ||
			BGRC_P_REGISTER_CHANGED( HORIZ_SCALER_STEP ) ||
			BGRC_P_REGISTER_CHANGED( VERT_AVERAGER_COUNT ) ||
			BGRC_P_REGISTER_CHANGED( VERT_AVERAGER_COEFF ) ||
			BGRC_P_REGISTER_CHANGED( VERT_SCALER_INITIAL_PHASE ) ||
			BGRC_P_REGISTER_CHANGED( VERT_SCALER_STEP ) )
			BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
	}
	else
	{
		/* disable scaling */
		BGRC_P_SET_FIELD_ENUM( BLIT_HEADER, SRC_SCALER_ENABLE, DISABLE );
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, STRIPE_ENABLE, DISABLE );
		BGRC_P_SET_FIELD_ENUM( SCALER_CTRL, HORIZ_SCALER_ENABLE, DISABLE );
		BGRC_P_SET_FIELD_ENUM( SCALER_CTRL, VERT_SCALER_ENABLE, DISABLE );
#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
		BGRC_P_SET_FIELD_ENUM( SCALER_CTRL, HORIZ_AVERAGER_ENABLE, DISABLE );
		BGRC_P_SET_FIELD_ENUM( SCALER_CTRL, VERT_AVERAGER_ENABLE, DISABLE );
#endif

		/* set register load field */
		if( BGRC_P_REGISTER_CHANGED( SCALER_CTRL ) )
			BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
	}

	/* set feedthrough field */
#if ((BCHP_CHIP==7038) && (BCHP_VER <  BCHP_VER_C0)) || \
	((BCHP_CHIP==7438) && (BCHP_VER >= BCHP_VER_A0))
	if( BGRC_P_COMPARE_FIELD( BLIT_HEADER, SRC_COLOR_KEY_ENABLE, ENABLE ) || 
		BGRC_P_COMPARE_FIELD( BLIT_HEADER, DEST_COLOR_KEY_ENABLE, ENABLE ) || 
		BGRC_P_COMPARE_FIELD( BLIT_HEADER, SRC_COLOR_MATRIX_ENABLE, ENABLE ) || 
#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
		BGRC_P_COMPARE_FIELD( BLIT_HEADER, DEST_COLOR_MATRIX_ENABLE, ENABLE ) || 
		BGRC_P_COMPARE_FIELD( BLIT_HEADER, OUTPUT_COLOR_MATRIX_ENABLE, ENABLE ) || 
#endif
		BGRC_P_COMPARE_FIELD( BLIT_HEADER, SRC_SCALER_ENABLE, ENABLE ) || (
		BGRC_P_COMPARE_VALUE( SRC_SURFACE_FORMAT_DEF_1, FORMAT_TYPE, 3 ) &&
		BGRC_P_COMPARE_FIELD( SRC_SURFACE_FORMAT_DEF_3, PALETTE_BYPASS, LOOKUP )) || (
		BGRC_P_COMPARE_VALUE( DEST_SURFACE_FORMAT_DEF_1, FORMAT_TYPE, 3 ) &&
		BGRC_P_COMPARE_FIELD( DEST_SURFACE_FORMAT_DEF_3, PALETTE_BYPASS, LOOKUP )) )
	{
		BGRC_P_SET_FIELD_ENUM( BLIT_HEADER, FEEDTHROUGH, ONE_PIXEL );
	}
	else
	{
#if ((BCHP_CHIP==7438) && (BCHP_VER >= BCHP_VER_A0))
		if( hGrc->ulDeviceNum == 0 )
		{
			BGRC_P_SET_FIELD_ENUM( BLIT_HEADER, FEEDTHROUGH, ONE_PIXEL );
		}
		else
#endif
		{
			BGRC_P_SET_FIELD_ENUM( BLIT_HEADER, FEEDTHROUGH, FOUR_PIXELS );
		}
	}
#endif

	/* set interrupt enable field */
	BGRC_P_SET_FIELD_DATA( BLIT_HEADER, INTERRUPT_ENABLE, bEnableInterrupt ? 1 : 0 );

	/* check if loading a source palette */
	if( BGRC_P_COMPARE_VALUE( SRC_SURFACE_FORMAT_DEF_1, FORMAT_TYPE, 3 ) &&
		BGRC_P_COMPARE_FIELD( SRC_SURFACE_FORMAT_DEF_3, PALETTE_BYPASS, LOOKUP ) )
	{
		if( pState->SrcSurface.ulPaletteOffset )
			BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_CLUT_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
	}

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	/* check if loading a destination palette */
	if( BGRC_P_COMPARE_VALUE( DEST_SURFACE_FORMAT_DEF_1, FORMAT_TYPE, 3 ) &&
		BGRC_P_COMPARE_FIELD( DEST_SURFACE_FORMAT_DEF_3, PALETTE_BYPASS, LOOKUP ) )
	{
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_CLUT_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
	}
#endif

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) ||
		BGRC_P_REGISTER_CHANGED( BLIT_CTRL ) ||
		(hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(LIST_PACKET_HEADER_1)] == 0) )
	{
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
	}

	return BERR_SUCCESS;
}

#define BGRC_P_OPTIMIZE_SET_REGISTER
/***************************************************************************/
static void BGRC_P_List_WritePacket(
	BGRC_Handle hGrc,
	uint32_t *pulPacket,
	uint32_t ulPacketHeader )
{
	BGRC_P_State *pState = &hGrc->CurrentState;

	/* write list header */
	*pulPacket++ = BCHP_FIELD_ENUM(M2MC_LIST_PACKET_HEADER_0, LAST_PKT_IND, NextPktInvalid);
	*pulPacket++ = ulPacketHeader;

	/* write source group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, SRC_FEEDER_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( SRC_FEEDER, SRC_FEEDER_ENABLE );
#else
		BGRC_P_SET_REGISTER( SRC_FEEDER_ENABLE );
		BGRC_P_SET_REGISTER( SRC_SURFACE_ADDR_0 );
		BGRC_P_SET_REGISTER( SRC_SURFACE_STRIDE_0 );
		BGRC_P_SET_REGISTER( SRC_SURFACE_ADDR_1 );
		BGRC_P_SET_REGISTER( SRC_SURFACE_STRIDE_1 );
		BGRC_P_SET_REGISTER( SRC_SURFACE_FORMAT_DEF_1 );
		BGRC_P_SET_REGISTER( SRC_SURFACE_FORMAT_DEF_2 );
		BGRC_P_SET_REGISTER( SRC_SURFACE_FORMAT_DEF_3 );
#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
		BGRC_P_SET_REGISTER( SRC_SURFACE_1_FORMAT_DEF_1 );
		BGRC_P_SET_REGISTER( SRC_SURFACE_1_FORMAT_DEF_2 );
		BGRC_P_SET_REGISTER( SRC_SURFACE_1_FORMAT_DEF_3 );
#endif
		BGRC_P_SET_REGISTER( SRC_W_ALPHA );
		BGRC_P_SET_REGISTER( SRC_CONSTANT_COLOR );
#endif
	}

	/* write destination group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( DST_FEEDER, DEST_FEEDER_ENABLE );
#else
		BGRC_P_SET_REGISTER( DEST_FEEDER_ENABLE );
		BGRC_P_SET_REGISTER( DEST_SURFACE_ADDR_0 );
		BGRC_P_SET_REGISTER( DEST_SURFACE_STRIDE_0 );
		BGRC_P_SET_REGISTER( DEST_SURFACE_ADDR_1 );
		BGRC_P_SET_REGISTER( DEST_SURFACE_STRIDE_1 );
		BGRC_P_SET_REGISTER( DEST_SURFACE_FORMAT_DEF_1 );
		BGRC_P_SET_REGISTER( DEST_SURFACE_FORMAT_DEF_2 );
		BGRC_P_SET_REGISTER( DEST_SURFACE_FORMAT_DEF_3 );
		BGRC_P_SET_REGISTER( DEST_W_ALPHA );
		BGRC_P_SET_REGISTER( DEST_CONSTANT_COLOR );
#endif
	}

	/* write output group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, OUTPUT_FEEDER_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( OUTPUT_FEEDER, OUTPUT_FEEDER_ENABLE );
#else
		BGRC_P_SET_REGISTER( OUTPUT_FEEDER_ENABLE );
		BGRC_P_SET_REGISTER( OUTPUT_SURFACE_ADDR_0 );
		BGRC_P_SET_REGISTER( OUTPUT_SURFACE_STRIDE_0 );
		BGRC_P_SET_REGISTER( OUTPUT_SURFACE_ADDR_1 );
		BGRC_P_SET_REGISTER( OUTPUT_SURFACE_STRIDE_1 );
		BGRC_P_SET_REGISTER( OUTPUT_SURFACE_FORMAT_DEF_1 );
		BGRC_P_SET_REGISTER( OUTPUT_SURFACE_FORMAT_DEF_2 );
		BGRC_P_SET_REGISTER( OUTPUT_SURFACE_FORMAT_DEF_3 );
#endif
	}

	/* write blit group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( BLIT, BLIT_HEADER );
#else
		BGRC_P_SET_REGISTER( BLIT_HEADER );
		BGRC_P_SET_REGISTER( BLIT_SRC_TOP_LEFT );
		BGRC_P_SET_REGISTER( BLIT_SRC_SIZE );
#if defined(BCHP_M2MC_BLIT_SRC_TOP_LEFT_1)
		BGRC_P_SET_REGISTER( BLIT_SRC_TOP_LEFT_1 );
		BGRC_P_SET_REGISTER( BLIT_SRC_SIZE_1 );
		BGRC_P_SET_REGISTER( BLIT_SRC_STRIPE_HEIGHT_WIDTH_0 );
		BGRC_P_SET_REGISTER( BLIT_SRC_STRIPE_HEIGHT_WIDTH_1 );
#endif
		BGRC_P_SET_REGISTER( BLIT_DEST_TOP_LEFT );
		BGRC_P_SET_REGISTER( BLIT_DEST_SIZE );
		BGRC_P_SET_REGISTER( BLIT_OUTPUT_TOP_LEFT );
		BGRC_P_SET_REGISTER( BLIT_OUTPUT_SIZE );
		BGRC_P_SET_REGISTER( BLIT_INPUT_STRIPE_WIDTH );
#if defined(BCHP_M2MC_BLIT_INPUT_STRIPE_WIDTH_1)
		BGRC_P_SET_REGISTER( BLIT_INPUT_STRIPE_WIDTH_1 );
#endif
		BGRC_P_SET_REGISTER( BLIT_OUTPUT_STRIPE_WIDTH );
		BGRC_P_SET_REGISTER( BLIT_STRIPE_OVERLAP );
#if defined(BCHP_M2MC_BLIT_STRIPE_OVERLAP_1)
		BGRC_P_SET_REGISTER( BLIT_STRIPE_OVERLAP_1 );
#endif
		BGRC_P_SET_REGISTER( BLIT_CTRL );
#endif
	}

	/* write scaler group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( SCALE_PARAM, SCALER_CTRL );
#else
		BGRC_P_SET_REGISTER( SCALER_CTRL );
		BGRC_P_SET_REGISTER( HORIZ_AVERAGER_COUNT );
		BGRC_P_SET_REGISTER( HORIZ_AVERAGER_COEFF );
		BGRC_P_SET_REGISTER( VERT_AVERAGER_COUNT );
		BGRC_P_SET_REGISTER( VERT_AVERAGER_COEFF );
		BGRC_P_SET_REGISTER( HORIZ_SCALER_INITIAL_PHASE );
		BGRC_P_SET_REGISTER( HORIZ_SCALER_STEP );
#if defined(BCHP_M2MC_HORIZ_SCALER_1_INITIAL_PHASE)
		BGRC_P_SET_REGISTER( HORIZ_SCALER_1_INITIAL_PHASE );
		BGRC_P_SET_REGISTER( HORIZ_SCALER_1_STEP );
#endif
		BGRC_P_SET_REGISTER( VERT_SCALER_INITIAL_PHASE );
		BGRC_P_SET_REGISTER( VERT_SCALER_STEP );
#if defined(BCHP_M2MC_VERT_SCALER_1_INITIAL_PHASE)
		BGRC_P_SET_REGISTER( VERT_SCALER_1_INITIAL_PHASE );
		BGRC_P_SET_REGISTER( VERT_SCALER_1_STEP );
#endif
#endif
	}

	/* write blend group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, BLEND_PARAM_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( BLEND_PARAM, BLEND_COLOR_OP );
#else
		BGRC_P_SET_REGISTER( BLEND_COLOR_OP );
		BGRC_P_SET_REGISTER( BLEND_ALPHA_OP );
		BGRC_P_SET_REGISTER( BLEND_CONSTANT_COLOR );
		BGRC_P_SET_REGISTER( BLEND_COLOR_KEY_ACTION );
#endif
	}

	/* write rop group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, ROP_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( ROP, ROP_OPERATION );
#else
		BGRC_P_SET_REGISTER( ROP_OPERATION );
		BGRC_P_SET_REGISTER( ROP_PATTERN_TOP );
		BGRC_P_SET_REGISTER( ROP_PATTERN_BOTTOM );
		BGRC_P_SET_REGISTER( ROP_PATTERN_COLOR_0 );
		BGRC_P_SET_REGISTER( ROP_PATTERN_COLOR_1 );
#endif
	}

	/* write source color key group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, SRC_COLOR_KEY_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( SRC_COLOR_KEY, SRC_COLOR_KEY_HIGH );
#else
		BGRC_P_SET_REGISTER( SRC_COLOR_KEY_HIGH );
		BGRC_P_SET_REGISTER( SRC_COLOR_KEY_LOW );
		BGRC_P_SET_REGISTER( SRC_COLOR_KEY_MASK );
		BGRC_P_SET_REGISTER( SRC_COLOR_KEY_REPLACEMENT );
		BGRC_P_SET_REGISTER( SRC_COLOR_KEY_REPLACEMENT_MASK );
#endif
	}

	/* write destination color key group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, DST_COLOR_KEY_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( DST_COLOR_KEY, DEST_COLOR_KEY_HIGH );
#else
		BGRC_P_SET_REGISTER( DEST_COLOR_KEY_HIGH );
		BGRC_P_SET_REGISTER( DEST_COLOR_KEY_LOW );
		BGRC_P_SET_REGISTER( DEST_COLOR_KEY_MASK );
		BGRC_P_SET_REGISTER( DEST_COLOR_KEY_REPLACEMENT );
		BGRC_P_SET_REGISTER( DEST_COLOR_KEY_REPLACEMENT_MASK );
#endif
	}

	/* write scaler coefficient group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, SCALE_COEF_GRP_CNTRL, GRP_ENABLE ) )
	{
		uint32_t ii, jj;

#if defined(BCHP_M2MC_HORIZ_FIR_1_COEFF_PHASE0_01)
		uint32_t kk;

		/* write horizontal coefficient group registers */
		for( kk = 0; kk < 2; ++kk )
#endif
			for( jj = 0; jj < BGRC_P_FIR_PHASE_COUNT; ++jj )
				for( ii = 0; ii < BGRC_P_FIR_TAP_COUNT / 2; ++ii )
					*pulPacket++ = pState->pulHorzFirCoeffs[ii + jj * BGRC_P_FIR_TAP_COUNT / 2];
	
		/* write vertical coefficient group registers */
#if defined(BCHP_M2MC_HORIZ_FIR_1_COEFF_PHASE0_01)
		for( kk = 0; kk < 2; ++kk )
#endif
			for( jj = 0; jj < BGRC_P_FIR_PHASE_COUNT; ++jj )
				for( ii = 0; ii < BGRC_P_FIR_TAP_COUNT / 2; ++ii )
					*pulPacket++ = pState->pulVertFirCoeffs[ii + jj * BGRC_P_FIR_TAP_COUNT / 2];

#if ((BCHP_CHIP==7401) && (BCHP_VER == BCHP_VER_A0)) || \
	((BCHP_CHIP==3563) && (BCHP_VER >= BCHP_VER_A0))
		/* Some M2MCs overwrite the pulldown registers when writing the scaler 
		   coefficient registers. Compensating for this requires writing a zero 
		   into the memory that will overwrite the pulldown enable register. */
		pulPacket += BGRC_P_LIST_SRC_COLOR_MATRIX_GRP_CNTRL_COUNT;
		*pulPacket = 0;
		pulPacket += BGRC_P_LIST_SCALE_COEF_GRP_CNTRL_COUNT - BGRC_P_LIST_SRC_COLOR_MATRIX_GRP_CNTRL_COUNT - 8;
#endif
	}

	/* write source color matrix group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, SRC_COLOR_MATRIX_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( SRC_COLOR_MATRIX, SRC_CM_C00_C01 );
#else
		BGRC_P_SET_REGISTER( SRC_CM_C00_C01 );
		BGRC_P_SET_REGISTER( SRC_CM_C02_C03 );
		BGRC_P_SET_REGISTER( SRC_CM_C04 );
		BGRC_P_SET_REGISTER( SRC_CM_C10_C11 );
		BGRC_P_SET_REGISTER( SRC_CM_C12_C13 );
		BGRC_P_SET_REGISTER( SRC_CM_C14 );
		BGRC_P_SET_REGISTER( SRC_CM_C20_C21 );
		BGRC_P_SET_REGISTER( SRC_CM_C22_C23 );
		BGRC_P_SET_REGISTER( SRC_CM_C24 );
		BGRC_P_SET_REGISTER( SRC_CM_C30_C31 );
		BGRC_P_SET_REGISTER( SRC_CM_C32_C33 );
		BGRC_P_SET_REGISTER( SRC_CM_C34 );
#endif
	}

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	/* write destination color matrix group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, DST_COLOR_MATRIX_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( DST_COLOR_MATRIX, DEST_CM_C00_C01 );
#else
		BGRC_P_SET_REGISTER( DEST_CM_C00_C01 );
		BGRC_P_SET_REGISTER( DEST_CM_C02_C03 );
		BGRC_P_SET_REGISTER( DEST_CM_C04 );
		BGRC_P_SET_REGISTER( DEST_CM_C10_C11 );
		BGRC_P_SET_REGISTER( DEST_CM_C12_C13 );
		BGRC_P_SET_REGISTER( DEST_CM_C14 );
		BGRC_P_SET_REGISTER( DEST_CM_C20_C21 );
		BGRC_P_SET_REGISTER( DEST_CM_C22_C23 );
		BGRC_P_SET_REGISTER( DEST_CM_C24 );
		BGRC_P_SET_REGISTER( DEST_CM_C30_C31 );
		BGRC_P_SET_REGISTER( DEST_CM_C32_C33 );
		BGRC_P_SET_REGISTER( DEST_CM_C34 );
#endif
	}

	/* write output color matrix group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, OUTPUT_COLOR_MATRIX_GRP_CNTRL, GRP_ENABLE ) )
	{
#ifdef BGRC_P_OPTIMIZE_SET_REGISTER
		BGRC_P_SET_REGISTERS( OUTPUT_COLOR_MATRIX, OUTPUT_CM_C00_C01 );
#else
		BGRC_P_SET_REGISTER( OUTPUT_CM_C00_C01 );
		BGRC_P_SET_REGISTER( OUTPUT_CM_C02_C03 );
		BGRC_P_SET_REGISTER( OUTPUT_CM_C04 );
		BGRC_P_SET_REGISTER( OUTPUT_CM_C10_C11 );
		BGRC_P_SET_REGISTER( OUTPUT_CM_C12_C13 );
		BGRC_P_SET_REGISTER( OUTPUT_CM_C14 );
		BGRC_P_SET_REGISTER( OUTPUT_CM_C20_C21 );
		BGRC_P_SET_REGISTER( OUTPUT_CM_C22_C23 );
		BGRC_P_SET_REGISTER( OUTPUT_CM_C24 );
		BGRC_P_SET_REGISTER( OUTPUT_CM_C30_C31 );
		BGRC_P_SET_REGISTER( OUTPUT_CM_C32_C33 );
		BGRC_P_SET_REGISTER( OUTPUT_CM_C34 );
#endif
	}
#endif

	/* write source palette group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, SRC_CLUT_GRP_CNTRL, GRP_ENABLE ) )
		*pulPacket++ = pState->SrcSurface.ulPaletteOffset;

	/* write destination palette group registers */
	if( BGRC_P_COMPARE_FIELD( LIST_PACKET_HEADER_1, DST_CLUT_GRP_CNTRL, GRP_ENABLE ) )
		*pulPacket++ = pState->DstSurface.ulPaletteOffset;
}

/***************************************************************************/
static void BGRC_P_List_RenderPacket(
	BGRC_Handle hGrc,
	uint32_t ulPacketOffset )
{
	/* write packet offset to previous packet header */
	if( hGrc->pulPrevPacket )
	{
		*hGrc->pulPrevPacket = ulPacketOffset | BCHP_FIELD_DATA(M2MC_LIST_PACKET_HEADER_0, LAST_PKT_IND, 
			BCHP_M2MC_LIST_PACKET_HEADER_0_LAST_PKT_IND_NextPktValid);
#if 1
		BMEM_FlushCache( hGrc->hMemory, hGrc->pulPrevPacket, 4 );
#endif
	}

	if( hGrc->pulPrevPacket == NULL )
	{
		/* write first packet offset and initiate */
		BGRC_P_WRITE_REG( LIST_FIRST_PKT_ADDR, ulPacketOffset );
		BGRC_P_WRITE_REG( LIST_CTRL, 
			BCHP_FIELD_ENUM(M2MC_LIST_CTRL, WAKE_MODE, ResumeFromFirst) | 
			BCHP_FIELD_ENUM(M2MC_LIST_CTRL, RUN, Run) | 
			BCHP_FIELD_ENUM(M2MC_LIST_CTRL, WAKE, Ack) );
	}
	else
	{
		/* initiate packet */
		BGRC_P_WRITE_REG( LIST_CTRL, 
			BCHP_FIELD_ENUM(M2MC_LIST_CTRL, WAKE_MODE, ResumeFromLast) | 
			BCHP_FIELD_ENUM(M2MC_LIST_CTRL, RUN, Run) | 
			BCHP_FIELD_ENUM(M2MC_LIST_CTRL, WAKE, WakeUp) );
	}
}

/***************************************************************************/
void BGRC_P_List_PacketIsr(
	void *pvParam1,
	int iParam2 )
{
 	BGRC_Handle hGrc = (BGRC_Handle) pvParam1;
	BGRC_P_Operation *pOp;
	uint32_t ulDeviceOffset;
	bool bDeviceBusy;

	BSTD_UNUSED( iParam2 );

	if( hGrc == 0 )
		return;

	ulDeviceOffset = BGRC_P_READ_REG( LIST_CURR_PKT_ADDR ) & BGRC_M2MC(LIST_CURR_PKT_ADDR_CURR_PKT_ADDR_MASK);

	bDeviceBusy = (((BGRC_P_READ_REG( BLIT_STATUS ) & BGRC_M2MC(BLIT_STATUS_STATUS_MASK)) == BGRC_M2MC(BLIT_STATUS_STATUS_RUNNING))/* ||
		((BGRC_P_READ_REG( LIST_STATUS ) & BGRC_M2MC(LIST_STATUS_BUSY_MASK)) == BGRC_M2MC(LIST_STATUS_BUSY_Busy)) ||
		((BGRC_P_READ_REG( LIST_STATUS ) & BGRC_M2MC(LIST_STATUS_FINISHED_MASK)) == BGRC_M2MC(LIST_STATUS_FINISHED_NotFinished)) */);

	/* find first active operation */
	pOp = hGrc->pLastOp;
	while( pOp && (!pOp->bActive) )
		pOp = pOp->pPrevOp;

	if( pOp == NULL )
		return;

	if( hGrc->pLastOp == NULL )
		return;

	while( ((!bDeviceBusy) && (pOp->pPrevOp == NULL)) || (ulDeviceOffset != pOp->ulPacketOffset) )
	{
		BGRC_P_Operation *pPrevOp = pOp->pPrevOp;
		BGRC_P_Operation *pCurrOp = pOp;

		/* call user callback function */
		if( pOp->pUserCallback )
			(*pOp->pUserCallback)( hGrc, pOp->pUserData );

		/* set events */
		if( pOp->bSetEvent )
			BKNI_SetEvent( hGrc->hInterruptEvent );

		/* set period event and call callback */
		if( pOp->bSetPeriodicEvent )
		{
			if( hGrc->pPeriodicCallback )
			{
				(*hGrc->pPeriodicCallback)( hGrc, hGrc->pPeriodicData );
				hGrc->pPeriodicCallback = NULL;
			}

			if( hGrc->ulPeriodicInterrupts )
				hGrc->ulPeriodicInterrupts--;

			BKNI_SetEvent( hGrc->hPeriodicEvent );
		}

		if( pOp->pUserCallback || pOp->bSetEvent )
			hGrc->ulIntReceived++;

		/* get next operation and inactivate current operation */
		pOp = pPrevOp;
		pCurrOp->bActive = false;

		/* exit loop if no more operations */
		if( pOp == NULL )
			break;
	}
}

/***************************************************************************/
static BGRC_P_Block *BGRC_P_List_CreateBlock(
	BGRC_Handle hGrc )
{
	/* allocate memory for block structure */
	BGRC_P_Block *pBlock = (BGRC_P_Block *) BKNI_Malloc( sizeof (BGRC_P_Block) );
	if( pBlock == NULL )
		return NULL;

	/* allocate device memory for list block */
	pBlock->pvMemory = BMEM_AllocAligned( hGrc->hMemory, BGRC_P_LIST_BLOCK_SIZE, BGRC_P_LIST_BLOCK_ALIGN, 0 );
	if( pBlock->pvMemory == NULL )
	{
		BKNI_Free( (void *) pBlock );
		return NULL;
	}

	/* convert block address to offset */
	if( BMEM_ConvertAddressToOffset( hGrc->hMemory, pBlock->pvMemory, &pBlock->ulOffset ) != BERR_SUCCESS )
	{
		BMEM_Free( hGrc->hMemory, pBlock->pvMemory );
		BKNI_Free( (void *) pBlock );
		return NULL;
	}

	/* get cached address */
	if( BMEM_ConvertAddressToCached( hGrc->hMemory, pBlock->pvMemory, &pBlock->pvCached ) != BERR_SUCCESS )
	{
		BMEM_Free( hGrc->hMemory, pBlock->pvMemory );
		BKNI_Free( (void *) pBlock );
		return NULL;
	}

	hGrc->ulPacketMemorySize += BGRC_P_LIST_BLOCK_SIZE;
	pBlock->ulRefCount = 0;
/*	pBlock->bBusy = true;*/

	return pBlock;
}

/***************************************************************************/
static BGRC_P_Block *BGRC_P_List_DestroyBlock(
	BGRC_Handle hGrc,
	BGRC_P_Block *pBlock )
{
	BGRC_P_Block *pNextBlock = pBlock->pNextBlock;

	/* free block memory */
	BMEM_Free( hGrc->hMemory, pBlock->pvMemory );
	BKNI_Free( (void *) pBlock );
	hGrc->ulPacketMemorySize -= BGRC_P_LIST_BLOCK_SIZE;

	/* dettach block from list */
	if( hGrc->pCurrListBlock == pNextBlock )
	{
		hGrc->pPrevListBlock = NULL;
		pNextBlock = NULL;
	}

	if( hGrc->pPrevListBlock )
		hGrc->pPrevListBlock->pNextBlock = pNextBlock;

	hGrc->pCurrListBlock = pNextBlock;
	return pNextBlock;
}

/***************************************************************************/
/* OPERATION PROCESSING FUNCTIONS                                          */
/***************************************************************************/
static BGRC_P_Operation *BGRC_P_Operation_Alloc( 
	BGRC_Handle hGrc,
	BGRC_Callback pCallback,
	void *pData,
	BGRC_P_Block *pBlock,
	uint32_t ulPacketOffset,
	bool bSetEvent )
{
	BGRC_P_Operation *pOp;

	/* check if there are any free operation structures */
	if( hGrc->pFreeOp )
	{
		/* get a free operation structure */
		pOp = hGrc->pFreeOp;
		hGrc->pFreeOp = pOp->pNextOp;
	}
	else
	{
		/* return error if operation max reached */
		if( hGrc->bPreAllocMemory && (hGrc->ulOperationCount == hGrc->ulOperationMax) )
			return NULL;

		/* allocate operation structure */
		pOp = (BGRC_P_Operation *) BKNI_Malloc( sizeof (BGRC_P_Operation) );
		if( pOp == NULL )
			return NULL;
		hGrc->ulOperationCount++;
	}

	pOp->pPrevOp = NULL;
	pOp->pNextOp = NULL;
	pOp->pUserCallback = pCallback;
	pOp->pUserData = pData;
	pOp->pBlock = pBlock;
	pOp->ulPacketOffset = ulPacketOffset;
	pOp->bSetEvent = bSetEvent;
	pOp->bActive = true;
	pOp->bSetPeriodicEvent = false;

	if( pCallback || bSetEvent )
		hGrc->ulIntExpected++;

	return pOp;
}

/***************************************************************************/
void BGRC_P_Operation_FreeAll( 
	BGRC_Handle hGrc )
{
	BGRC_P_Operation *pOp = hGrc->pFreeOp;
	while( pOp )
	{
		BGRC_P_Operation *pNextOp = pOp->pNextOp;
		BKNI_Free( pOp );
		pOp = pNextOp;
	}

	hGrc->pFreeOp = NULL;
}

/***************************************************************************/
static void BGRC_P_Operation_AddToList( 
	BGRC_Handle hGrc,
	BGRC_P_Operation *pOp )
{
	if( hGrc->pCurrOp )
		hGrc->pCurrOp->pPrevOp = pOp;

	if( hGrc->pLastOp == NULL )
		hGrc->pLastOp = pOp;

	pOp->pNextOp = hGrc->pCurrOp;
	hGrc->pCurrOp = pOp;
}

/***************************************************************************/
static void BGRC_P_Operation_RemoveFromList( 
	BGRC_Handle hGrc,
	BGRC_P_Operation *pOp )
{
	BGRC_P_Operation *pPrevOp = pOp->pPrevOp;

	/* dettach operation from list */
	if( hGrc->pCurrOp == hGrc->pLastOp )
		hGrc->pCurrOp = NULL;

	hGrc->pLastOp = pPrevOp;
	if( pPrevOp )
	{
		pPrevOp->pNextOp = NULL;
		pOp->pPrevOp = NULL;
	}

	/* decrement reference count of packet memory block */
	pOp->pBlock->ulRefCount--;

	/* check if more operations allocated than allowed */
	if( hGrc->ulOperationMax && (!hGrc->bPreAllocMemory) && (hGrc->ulOperationCount > hGrc->ulOperationMax) )
	{
		/* free operation memory */
		BKNI_Free( pOp );
		hGrc->ulOperationCount--;
	}
	else
	{
		/* attach operation to free list */
		pOp->pNextOp = hGrc->pFreeOp;
		hGrc->pFreeOp = pOp;
	}
}

/***************************************************************************/
void BGRC_P_Operation_CleanupList( 
	BGRC_Handle hGrc )
{
	BGRC_P_Operation *pOp;

	/* remove inactive operations from list */
	pOp = hGrc->pLastOp;
	while( pOp && (!pOp->bActive) )
	{
		BGRC_P_Operation *pPrevOp = pOp->pPrevOp;
		BGRC_P_Operation_RemoveFromList( hGrc, pOp );
		pOp = pPrevOp;
	}
}

/***************************************************************************/
bool BGRC_P_Operation_Prealloc( 
	BGRC_Handle hGrc,
	uint32_t ulCount )
{
	BGRC_P_Operation *pPrevOp = NULL;
	uint32_t ii;

	/* setup free operations list */
	for( ii = 0; ii < ulCount; ++ii )
	{
		BGRC_P_Operation *pOp = (BGRC_P_Operation *) BKNI_Malloc( sizeof (BGRC_P_Operation) );
		if( pOp == NULL )
			return false;

		if( pPrevOp )
			pPrevOp->pNextOp = pOp;
		else
			hGrc->pFreeOp = pOp;

		pPrevOp = pOp;
	}

	if( pPrevOp )
		pPrevOp->pNextOp = NULL;

	hGrc->ulOperationCount = ulCount;
	return true;
}

/* End of File */
