/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgrc.c $
 * $brcm_Revision: Hydra_Software_Devel/235 $
 * $brcm_Date: 8/24/12 1:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/grc/7435/bgrc.c $
 * 
 * Hydra_Software_Devel/235   8/24/12 1:50p nissen
 * SW7445-22: Added support for the dual M2MCs on the 7445.
 * 
 * Hydra_Software_Devel/234   8/2/12 2:21p nissen
 * SW7563-13: Added support for the 7563.
 * 
 * Hydra_Software_Devel/233   5/3/12 3:18p nissen
 * SW7584-11: Added more support for 7584.
 * 
 * Hydra_Software_Devel/232   3/2/12 1:28p nissen
 * SW7435-13 : Added support for 2nd M2MC in 7435 A0.
 * 
 * Hydra_Software_Devel/231   2/16/12 3:35p nissen
 * SW7405-5427: Adding flags for output mirroring when blitting
 * 
 * Hydra_Software_Devel/230   2/1/12 1:51p nissen
 * SW7346-660: Fixed BGRC_WaitForOperationReady timeout by checking ref
 * count before waiting for event.
 * 
 * Hydra_Software_Devel/229   1/6/12 1:25p nissen
 * SW7435-13 : Fixed support for 7435 A0.
 * 
 * Hydra_Software_Devel/228   12/21/11 5:21p nissen
 * SW7360-5: Added support for the 7360 A0.
 * 
 * Hydra_Software_Devel/227   12/15/11 6:36p nissen
 * SW7435-13 : Added support for 7435 A0.
 * 
 * Hydra_Software_Devel/226   12/14/11 4:41p nissen
 * SW7435-13 : Added support for 7435 A0.
 * 
 * Hydra_Software_Devel/225   12/8/11 2:09p nissen
 * SW7425-1413: Disabled split fifo.
 * 
 * Hydra_Software_Devel/224   10/5/11 5:27p nissen
 * SW7425-248: Updated use of BDBG_OBJECTs.
 * 
 * Hydra_Software_Devel/223   10/5/11 12:53p nissen
 * SW7425-248: Removed BDBG_OBJECT support for the 7038.
 * 
 * Hydra_Software_Devel/222   9/23/11 12:36p nissen
 * SW7429-4: Added support for 7429.
 * 
 * Hydra_Software_Devel/221   9/17/11 7:18p nissen
 * SW7425-248: Added BDBG_OBJECT_SET.
 * 
 * Hydra_Software_Devel/220   9/17/11 7:08p nissen
 * SW7425-248: Added BDBG_OBJECT_ID.
 * 
 * Hydra_Software_Devel/219   9/12/11 2:21p nissen
 * SWBLURAY-27238: Fixing support for 7640 B0.
 * 
 * Hydra_Software_Devel/218   9/9/11 7:26p nissen
 * SWBLURAY-27238: Added support for 7640 B0.
 * 
 * Hydra_Software_Devel/217   4/20/11 1:25p nissen
 * SW7630-104: Added support for A8_Y8 format and big endian 420
 * conversion.
 * 
 * Hydra_Software_Devel/216   3/24/11 1:35p jhaberf
 * SW35330-13: Added support for 35233 DTV chip
 * 
 * Hydra_Software_Devel/215   3/2/11 11:24p nissen
 * SW7038-3500: Fixed build errors on 7038.
 * 
 * Hydra_Software_Devel/214   12/3/10 4:21p nissen
 * SW35125-33: Added support for the 35125 DTV chip
 * 
 * Hydra_Software_Devel/213   12/3/10 11:42a nissen
 * SW7344-10: Added support for the 7344 and 7346.
 * 
 * Hydra_Software_Devel/212   11/25/10 12:31p nissen
 * SW7552-13: Fixed support for 7552.
 * 
 * Hydra_Software_Devel/211   11/24/10 9:34p nissen
 * SW7552-13: Added support for 7552.
 * 
 * Hydra_Software_Devel/210   11/19/10 12:47p nissen
 * SW7425-19: Fixed HW reset.
 * 
 * Hydra_Software_Devel/209   10/12/10 1:15p nissen
 * SW7231-9: Added support for the 7231.
 * 
 * Hydra_Software_Devel/208   10/12/10 12:58p nissen
 * SW7358-14: Added support for the 7358.
 * 
 * Hydra_Software_Devel/207   9/24/10 1:41p nissen
 * SW7550-579: Added GRC support for 7550 B0.
 * 
 * Hydra_Software_Devel/206   9/20/10 12:13p nissen
 * SW7405-4755: Fixed function for setting KeyMatrixScale order.
 * 
 * Hydra_Software_Devel/205   9/17/10 5:58p nissen
 * SW7325-809: Returning error if rectangle coordinates are greater than
 * the max of 8191.
 * 
 * Hydra_Software_Devel/204   9/1/10 8:03p nissen
 * SW7135-11: Added support for building for the 7135.
 * 
 * Hydra_Software_Devel/203   8/28/10 4:47p nissen
 * SW7425-19: Added support for building for the 7422 and 7425.
 * 
 * Hydra_Software_Devel/202   8/3/10 2:30p nissen
 * SW7420-925: Removed BERR_TRACE when returning out of memory error.
 * 
 * Hydra_Software_Devel/201   7/29/10 1:17p nissen
 * SW7208-24: Fixed luma surface format for YUV420 blits.
 * 
 * Hydra_Software_Devel/200   7/16/10 3:16p nissen
 * SW35230-369: Fixing setting raster order for YUV420 secondary surface.
 * 
 * Hydra_Software_Devel/199   7/2/10 3:12p nissen
 * SW35230-369: Fixing method of setting raster order for YUV420.
 * 
 * Hydra_Software_Devel/198   7/2/10 1:51p nissen
 * SW35230-369: Changed raster order back to striped for YUV420 destripe
 * blits.
 * 
 * Hydra_Software_Devel/197   7/1/10 3:05p nissen
 * SW35230-369: Changed raster order to linear for YUV420 destripe blits.
 * 
 * Hydra_Software_Devel/196   7/1/10 2:26p nissen
 * SW35230-369: Fixed raster order for YUV420 destripe blit.
 * 
 * Hydra_Software_Devel/195   7/1/10 12:55p nissen
 * SW35230-369: Fixed defines for YUV420 destripe blit.
 * 
 * Hydra_Software_Devel/194   6/30/10 5:11p nissen
 * SW35230-369: Fixing problem with YUV420 destripe blit.
 * 
 * Hydra_Software_Devel/193   6/1/10 6:10p nissen
 * SW7400-2716: Fixed pattern compile warning.
 * 
 * Hydra_Software_Devel/192   5/28/10 2:55p nissen
 * SW7400-2716: Fixed compiler warning.
 * 
 * Hydra_Software_Devel/191   3/9/10 4:14p mward
 * SW7400-2716: Cast return from BGRC_WaitForOperationsComplete() to void
 * to indicate intentional ignore of return value to prevent Coverity
 * error.
 * 
 * Hydra_Software_Devel/190   1/10/10 3:22p nissen
 * SW7400-2644: Changed default back to ScaleThenKeyThenMatrix, otherwise
 * YCbCr420 blits hang.
 * 
 * Hydra_Software_Devel/189   12/7/09 5:19p nissen
 * SW7468-40: Added support for 7468.
 * 
 * Hydra_Software_Devel/188   12/2/09 2:43p nissen
 * SW35230-12: Added support for 35230.
 * 
 * Hydra_Software_Devel/187   11/19/09 2:59p nissen
 * SW7405-3429: Changed KeyMatrixScale order default to
 * KeyThenScaleThenMatrix.
 * 
 * Hydra_Software_Devel/186   11/17/09 9:47a nissen
 * SW7408-16: Added support for 7408.
 * 
 * Hydra_Software_Devel/185   11/4/09 4:29p nissen
 * SW3556-849: Added support for 10-bit 422 and 444 formats for the
 * 3548/3556.
 * 
 * Hydra_Software_Devel/184   8/17/09 8:43p nissen
 * PR 57469: Fixed WaitForOperationsComplete by restoring state when blit
 * fails.
 * 
 * Hydra_Software_Devel/183   8/12/09 3:30p nissen
 * PR 57469: Added dummy blit to OperationsComplete to flush operation
 * processing when device is idle.
 * 
 * Hydra_Software_Devel/182   8/12/09 1:08p nissen
 * PR 57469: Set min packet and operation sizes when opening.
 * 
 * Hydra_Software_Devel/181   8/10/09 8:06p nissen
 * PR 57469: Added code to return failure for WaitForOperationReady.
 * 
 * Hydra_Software_Devel/180   8/10/09 6:14p nissen
 * PR 57469: Fixed problem with WaitForOperationReady function.
 * 
 * Hydra_Software_Devel/179   8/5/09 11:58a nissen
 * PR 57328: Fixed BGRC_OperationsComplete.
 * 
 * Hydra_Software_Devel/178   8/5/09 11:28a nissen
 * PR 57328: Fixed BGRC_OperationsComplete to report when all operations
 * have been processed, not just when the device is done.
 * 
 * Hydra_Software_Devel/177   8/4/09 8:54p nissen
 * PR 57093: Added support for 7125.
 * 
 * Hydra_Software_Devel/176   8/4/09 5:52p nissen
 * PR 57180: Added code to return error message when attempting to use a
 * 10-bit source on devices that don't support it.
 * 
 * Hydra_Software_Devel/175   7/8/09 6:09p nissen
 * PR 55223: Added support for 7340 and 7342.
 * 
 * Hydra_Software_Devel/174   7/7/09 5:52p nissen
 * PR 56610: Moved local declaration to GRC handle structure.
 * 
 * Hydra_Software_Devel/173   6/22/09 5:15p nissen
 * PR 56219: Added support for disabling channels on the primary source
 * surface, that will be provided by the secondary/alpha source surface.
 * 
 * Hydra_Software_Devel/172   6/9/09 4:19p nissen
 * PR 52504: Changed global and static variables to constants.
 * 
 * Hydra_Software_Devel/171   6/8/09 6:34p nissen
 * PR 51077: Fixed return value when getting error in
 * BGRC_WaitForOperationsComplete().
 * 
 * Hydra_Software_Devel/170   6/7/09 5:13p nissen
 * PR 51077: Fixed while loop condition when waiting for periodic event.
 * 
 * Hydra_Software_Devel/169   6/3/09 9:41p nissen
 * PR 51077: Fixed condition when waiting for available memory.
 * 
 * Hydra_Software_Devel/168   6/3/09 7:13p nissen
 * PR 51077: Fixed problem with waiting for available memory.
 * 
 * Hydra_Software_Devel/167   6/3/09 5:41p nissen
 * PR 51077: Added memory cleanup before waiting on periodic event.
 * 
 * Hydra_Software_Devel/166   6/2/09 9:44p nissen
 * PR 51077: Fixed problem with waiting for available memory.
 * 
 * Hydra_Software_Devel/165   6/1/09 8:37p nissen
 * PR 51077: Changed periodic interrupt to happen more frequently.
 * 
 * Hydra_Software_Devel/164   5/20/09 4:19p nissen
 * PR 54637: Added support for using TestFeature1 when the M2MC operation
 * queue is running.
 * 
 * Hydra_Software_Devel/163   5/15/09 7:02p nissen
 * PR 54637: Added wait after disabling TestFeature1.
 * 
 * Hydra_Software_Devel/162   5/15/09 4:56p nissen
 * PR 54637: Added code to reset TestFeature1 after blit.
 * 
 * Hydra_Software_Devel/161   5/15/09 12:59p nissen
 * PR 52465: Reseting test feature before using.
 * 
 * Hydra_Software_Devel/160   5/13/09 1:49p nissen
 * PR 52465: Disabling test feature when not in use.
 * 
 * Hydra_Software_Devel/159   5/13/09 10:46a nissen
 * PR 51077: Fixed problem with waiting for operations to complete.
 * 
 * Hydra_Software_Devel/158   4/24/09 1:06p katrep
 * PR54146: Fixed coveirty issue
 * 
 * Hydra_Software_Devel/157   4/17/09 5:27p nissen
 * PR 51077: Added code to wait function to check if event has already
 * been set.
 * 
 * Hydra_Software_Devel/156   4/17/09 4:35p nissen
 * PR 51077: Added callbacks to wait functions.
 * 
 * Hydra_Software_Devel/155   4/14/09 6:13p nissen
 * PR 53698: Fixing destination palette bypass.
 * 
 * Hydra_Software_Devel/154   4/13/09 4:48p nissen
 * PR 54114: Fixed compile error for 7400.
 * 
 * Hydra_Software_Devel/153   4/9/09 2:18p nissen
 * PR 53698: Removed support for loading a destination palette because the
 * M2MC does not support it.
 * 
 * Hydra_Software_Devel/152   4/7/09 2:38p nissen
 * PR 52976: Added support for 7635/7630 and future chips using RDB
 * defines.
 * 
 * Hydra_Software_Devel/151   4/1/09 8:31p nissen
 * PR 51077: Added pattern and blend states to saving and restoring when
 * waiting for operations to complete.
 * 
 * Hydra_Software_Devel/150   4/1/09 4:03p nissen
 * PR 51077: Added code to save and restore state when waiting for
 * operations to complete.
 * 
 * Hydra_Software_Devel/149   3/25/09 9:39p nissen
 * PR 51077: Fixed memory leak when closing module.
 * 
 * Hydra_Software_Devel/148   3/20/09 4:26p nissen
 * PR 51077: Added functions to wait on operations, and better handling
 * for running out of memory.
 * 
 * Hydra_Software_Devel/147   3/17/09 2:55p nissen
 * PR 52321: Added functions for enabling Alpha Premultiply.
 * 
 * Hydra_Software_Devel/146   3/6/09 2:55p nissen
 * PR 48325: Fixed Coverity Defect ID:12036 and 12037.
 * 
 * Hydra_Software_Devel/145   2/26/09 4:26p nissen
 * PR 52465: Using TestFeature1 prediction mode field.
 * 
 * Hydra_Software_Devel/144   2/25/09 5:04p nissen
 * PR 52465: Fixed device completion test in BGRC_OperationsComplete.
 * 
 * Hydra_Software_Devel/143   2/25/09 4:29p nissen
 * PR 48447: Set 3548/3556 B2 to use NOLEFT when output surface height is
 * 1080.
 * 
 * Hydra_Software_Devel/142   1/31/09 1:17a jrubio
 * PR51629: add 7336 support
 * 
 * Hydra_Software_Devel/141   12/3/08 7:46p nissen
 * PR 47763: Fixed support for 7420.
 * 
 * Hydra_Software_Devel/140   12/3/08 6:59p nissen
 * PR 47763: Added support for 7420.
 * 
 * Hydra_Software_Devel/139   10/23/08 9:45p nissen
 * PR 46310: Changed TestFeature1 parameter to Normal instead of NoLeft.
 * 
 * Hydra_Software_Devel/138   10/3/08 3:56p nissen
 * PR 46310: Updated TestFeature1.
 * 
 * Hydra_Software_Devel/137   10/1/08 6:10p nissen
 * PR 46310: Added support for outputing to TestFeature1 surface for
 * 3548/3556 B0.
 * 
 * Hydra_Software_Devel/136   9/26/08 11:17a nissen
 * PR 46715: Fixed setting alpha premultiply field for 7601.
 * 
 * Hydra_Software_Devel/135   7/30/08 8:04p nissen
 * PR 33687: Added support for the 7601.
 * 
 * Hydra_Software_Devel/134   7/25/08 12:48p nissen
 * PR 32979: Added memory fix for mosaic captures for 7405 only.
 * 
 * Hydra_Software_Devel/133   6/23/08 11:28a nissen
 * PR 43693: Added fix to init macro block when moduled opened for 3548
 * and 3556.
 * 
 * Hydra_Software_Devel/132   6/4/08 8:05p nissen
 * PR 43321: Updated for 3548 and 3556.
 * 
 * Hydra_Software_Devel/131   4/4/08 11:58a nissen
 * PR 39460: Added support for the 3556.
 * 
 * Hydra_Software_Devel/130   4/4/08 10:38a nissen
 * PR 35948: Renamed callback function.
 * 
 * Hydra_Software_Devel/129   2/8/08 2:09p nissen
 * PR 38945: Added support for the 3548.
 * 
 * Hydra_Software_Devel/128   12/17/07 9:54a nissen
 * PR 38269: Added support for inverting sources during a blit.
 * 
 * Hydra_Software_Devel/127   12/4/07 5:21p nissen
 * PR 36878: Added support for the 7335.
 * 
 * Hydra_Software_Devel/126   11/16/07 3:07p nissen
 * PR 36759: Added support for setting the scale factor independently of
 * the provided rectangle sizes.
 * 
 * Hydra_Software_Devel/125   10/24/07 2:39p nissen
 * PR 36107: Including bint.h instead of bint_plat.h.
 * 
 * Hydra_Software_Devel/124   10/12/07 11:24a nissen
 * PR 35747: Added fix for YCbCr 420 sources for 7405 by using surface
 * pitch as stripe width, and surface height as stripe height.
 * 
 * Hydra_Software_Devel/123   10/12/07 7:47a nissen
 * PR 35920: Added support for preallocating system memory for operation
 * structures when opening module.
 * 
 * Hydra_Software_Devel/122   9/19/07 11:24a nissen
 * PR 34538: Added support for building for the 7325.
 * 
 * Hydra_Software_Devel/121   9/18/07 1:32p nissen
 * PR 34852: Added support for building for the 3573.
 * 
 * Hydra_Software_Devel/120   8/20/07 3:43p nissen
 * PR 34132: Fixed problem with building for chips that don't support 420
 * linear format.
 * 
 * Hydra_Software_Devel/119   8/20/07 2:11p nissen
 * PR 34132: Added support for setting a linear 420 source.
 * 
 * Hydra_Software_Devel/118   7/13/07 9:15a yuxiaz
 * PR29724, PR32979: Added support for 7405.
 * 
 * Hydra_Software_Devel/117   6/25/07 8:51a nissen
 * PR 32459: Added support for building for the 7400 C0.
 * 
 * Hydra_Software_Devel/116   6/18/07 11:52a nissen
 * PR 32140: Added support for setting scale, color key, and color matrix
 * order.
 * 
 * Hydra_Software_Devel/115   5/2/07 12:23p nissen
 * PR 29724: Fixed problem with MacroBlock strip width.
 * 
 * Hydra_Software_Devel/114   5/2/07 11:37a nissen
 * PR 29724: Added support for 7405 A0 including new MacroBlock strip
 * width setting.
 * 
 * Hydra_Software_Devel/113   3/30/07 8:52a nissen
 * PR 29200: Moved M2MC reset before initialization of interrupts to
 * prevent any previous interrupts from firing.
 * 
 * Hydra_Software_Devel/112   3/28/07 9:45p nissen
 * PR 29156: Fixed bug with palette surface identification.
 * 
 * Hydra_Software_Devel/111   3/28/07 10:50a nissen
 * PR 29136: Added code to disabled 3-2 pulldown cadence on 3560 and 3563
 * when opeing module.
 * 
 * Hydra_Software_Devel/110   3/28/07 9:54a nissen
 * PR 29122: Fixed problem with using an XRGB source surface on 7440.
 * 
 * Hydra_Software_Devel/109   3/6/07 7:53p nissen
 * PR 25991: Fixed problem with checking device number on 7400 A0.
 * 
 * Hydra_Software_Devel/108   3/2/07 3:05p nissen
 * PR 28010: Added support for new AlphaPremultiply on 7440 B0.
 * 
 * Hydra_Software_Devel/107   3/2/07 11:32a nissen
 * PR 28010: Added Macroblock feature support for 7440 B0.
 * 
 * Hydra_Software_Devel/106   3/1/07 1:27p nissen
 * PR 25991: Switched to supporting one M2MC on 7400 B0.
 * 
 * Hydra_Software_Devel/105   3/1/07 12:37p nissen
 * PR 25991: Fixed MacroBlock feature for 7400 B0.
 * 
 * Hydra_Software_Devel/104   1/12/07 12:48p nissen
 * PR 27040: Fixed problem with using uninitialized surface structure in
 * functions that set surfaces.
 * 
 * Hydra_Software_Devel/103   12/11/06 1:57p nissen
 * PR 26405: Added surface address information to debug message dump.
 * 
 * Hydra_Software_Devel/102   12/7/06 3:36p nissen
 * PR 26405: Added wait information to debug message dump.
 * 
 * Hydra_Software_Devel/101   12/7/06 12:49p nissen
 * PR 26405: Added calls to DBG_MSG to log blits.
 * 
 * Hydra_Software_Devel/100   12/1/06 10:56a nissen
 * PR 25991: Added support for 7400 B0.
 * 
 * Hydra_Software_Devel/99   11/15/06 2:36p nissen
 * PR 25683: Fixed problem with using new surface id.
 * 
 * Hydra_Software_Devel/98   11/15/06 11:42a nissen
 * PR 25668: Added support for 7403.
 * 
 * Hydra_Software_Devel/97   11/8/06 11:07a nissen
 * PR 25683: Added support for using a surface ID to identify new
 * surfaces.
 * 
 * Hydra_Software_Devel/96   10/24/06 7:34p nissen
 * PR 24648: Fixed problem with VC-1 macroblock
 * 
 * Hydra_Software_Devel/95   10/20/06 10:08a nissen
 * PR 25023: Fixed build warnings.
 * 
 * Hydra_Software_Devel/94   10/19/06 10:54a nissen
 * PR 24647: Added support for preallocting all device memory when module
 * is opened.
 * 
 * Hydra_Software_Devel/93   10/12/06 11:14a nissen
 * PR 24648: Disabling macro block range support because of a problem with
 * it.
 * 
 * Hydra_Software_Devel/92   10/5/06 6:44p nissen
 * PR 24375: Fixed build problem.
 * 
 * Hydra_Software_Devel/91   10/5/06 1:58p nissen
 * PR 24648: Added support for VC-1 macro block range remapping and
 * expansion for 7401 C0 and 7118.
 * 
 * Hydra_Software_Devel/90   9/19/06 4:04p nissen
 * PR 24375: Set RANGE_EXP_MAP_SCALE_FACTOR_Y/C to 0x8 for source alpha
 * surface on 7401 C0 and 7118 A0.
 * 
 * Hydra_Software_Devel/89   9/15/06 10:52p nissen
 * PR 23977: set RANGE_EXP_MAP_SCALE_FACTOR_Y/C to 0x8 for 7401 C0.
 * 
 * Hydra_Software_Devel/88   9/8/06 3:54p nissen
 * PR 23517: Added support for 7440.
 * 
 * Hydra_Software_Devel/87   9/6/06 2:18a syang
 * PR 23977: set RANGE_EXP_MAP_SCALE_FACTOR_Y/C to 0x8
 * 
 * Hydra_Software_Devel/86   8/28/06 4:29p nissen
 * PR 23941: Re-added function to toggle destination palette bypass.
 * 
 * Hydra_Software_Devel/85   8/25/06 9:39a nissen
 * PR 23179: Fixed define for running on 3563.
 * 
 * Hydra_Software_Devel/84   8/25/06 9:08a nissen
 * PR 21940: Fixed problem with opening module on 7118.
 * 
 * Hydra_Software_Devel/83   8/17/06 11:26a nissen
 * PR 23179: Added support for building and running on the 3563.
 * 
 * Hydra_Software_Devel/82   7/13/06 7:27a nissen
 * PR 22396: Moved chip macro.
 * 
 * Hydra_Software_Devel/81   7/12/06 9:29p nissen
 * PR 22396: Added code to clear secondary source surface related
 * registers when not using a YCbCr 420 source surface.
 * 
 * Hydra_Software_Devel/80   6/22/06 10:41a nissen
 * PR 21940: Added support for 7118.
 * 
 * Hydra_Software_Devel/79   5/30/06 10:58a nissen
 * PR 21613: Fixed surface format validation when setting destination and
 * output alpha surfaces.
 * 
 * Hydra_Software_Devel/78   5/30/06 10:41a nissen
 * PR 21878: Added code to check if the device is really hung when waiting
 * times out, and added support for user setting the time out interval.
 * 
 * Hydra_Software_Devel/77   5/8/06 1:52p nissen
 * PR 21399: Added BGRC_OperationsComplete function that will read the
 * M2MC registers to determine if all pending operations are complete.
 * 
 * Hydra_Software_Devel/76   5/5/06 3:34p nissen
 * PR 20236: Switched scale/key/matrix order.
 * 
 * Hydra_Software_Devel/75   5/3/06 12:41p nissen
 * PR 21171: Added support for building and running 7401 B0.
 * 
 * Hydra_Software_Devel/74   4/18/06 1:52p nissen
 * PR 21007: Fixed warnings.
 * 
 * Hydra_Software_Devel/73   4/18/06 1:23p nissen
 * PR 18369: Added support for hardware striping when scaling on the 7400
 * and 7401.
 * 
 * Hydra_Software_Devel/72   4/13/06 3:33p nissen
 * PR 20352: Added support for all filter coefficients for 7400 and 7401.
 * 
 * Hydra_Software_Devel/71   3/22/06 1:15p nissen
 * PR 19551: Fixed use of format type register field.
 * 
 * Hydra_Software_Devel/70   3/22/06 12:05p nissen
 * PR 19551: Using new macro to set color register values.
 * 
 * Hydra_Software_Devel/69   3/15/06 1:22p nissen
 * PR 15840: Fixed problem with YCbCr420 format without filtering enabled.
 * 
 * Hydra_Software_Devel/68   3/15/06 12:05p nissen
 * PR 15840: Added support for using YCbCr420 surfaces as a source.
 * 
 * Hydra_Software_Devel/67   3/8/06 2:52p nissen
 * PR 19575: Added support for building and running second m2mc on 7438.
 * 
 * Hydra_Software_Devel/66   3/7/06 3:24p nissen
 * PR 19084: Removed & from function paramater when creating interrupt
 * callback.
 * 
 * Hydra_Software_Devel/65   3/7/06 11:06a nissen
 * PR 19084: Fixed vxworks build problem with 7400 interrupt macro.
 * 
 * Hydra_Software_Devel/64   2/21/06 4:42p nissen
 * PR 19553: Added support for point sample and bilinear filtering for
 * 7400.
 * 
 * Hydra_Software_Devel/63   2/21/06 3:36p nissen
 * PR 19084: Fixed problem with 7400 interrupt macro.
 * 
 * Hydra_Software_Devel/62   2/21/06 12:02p nissen
 * PR 19084: Added support for second m2mc device.
 * 
 * Hydra_Software_Devel/61   2/18/06 8:31a nissen
 * PR 19553: Switched scale/key/matrix order macro for 7400.
 * 
 * Hydra_Software_Devel/60   2/1/06 5:31p nissen
 * PR 19084: Added support for building for the 7400 A0.
 * 
 * Hydra_Software_Devel/59   1/12/06 5:59p nissen
 * PR 18761: Added support for BCHP_VER macro.
 * 
 * Hydra_Software_Devel/58   11/7/05 10:16a nissen
 * PR 17518: Removed offset adjust and clut scale functions for 7401.
 * 
 * Hydra_Software_Devel/57   10/26/05 1:46p nissen
 * PR 17518: Added new entry point functions for 7401 scaling parameters.
 * Added code to return error if not using Sharp filtering while scaling
 * for 7401.
 * 
 * Hydra_Software_Devel/56   9/19/05 11:53a nissen
 * PR 17201: Removed calls from BGRC_Open that are not supported by 7401.
 * 
 * Hydra_Software_Devel/55   9/13/05 4:30p nissen
 * PR 16499: Added code to free memory past a default or specified
 * threshold.
 * 
 * Hydra_Software_Devel/54   7/7/05 5:42p nissen
 * PR 15108: Fixed problem with getting palette offset.
 * 
 * Hydra_Software_Devel/53   7/5/05 5:30p nissen
 * PR 16056: Added support for building for the 7401 A0.
 * 
 * Hydra_Software_Devel/52   6/14/05 10:12a nissen
 * PR 15811: Re-added support for destination color key for Cx devices.
 * 
 * Hydra_Software_Devel/51   5/18/05 4:24p agin
 * PR14720: B2, C1, C2 compilation support.
 * 
 * Hydra_Software_Devel/51   5/18/05 11:53a agin
 * PR14720: B2, C1, C2 compilation support.
 * 
 * Hydra_Software_Devel/50   5/12/05 4:36p nissen
 * PR 15264: Modified support for user defined phase adjustment when
 * filtering.
 * 
 * Hydra_Software_Devel/49   5/12/05 2:47p nissen
 * PR 15264: Added support for user defined phase adjustment when
 * filtering.
 * 
 * Hydra_Software_Devel/48   5/12/05 10:28a nissen
 * PR 15226: Modified code to store surface information when a surface is
 * set instead of looking it up later.
 * 
 * Hydra_Software_Devel/47   5/6/05 1:34p nissen
 * PR 13488: Added support for M2MC C1.
 * 
 * Hydra_Software_Devel/46   3/16/05 12:46p nissen
 * PR 14457: Added code to return an error if destination and output
 * rectangle sizes are not the same.
 * 
 * Hydra_Software_Devel/45   3/16/05 12:21p nissen
 * PR 14329: Fixed use of W alpha.
 * 
 * Hydra_Software_Devel/44   2/24/05 1:36p nissen
 * PR 14207: Added bstd.h include.
 * 
 * Hydra_Software_Devel/43   2/17/05 1:14p nissen
 * PR 14086: Added support for new color matrix register precision on the
 * 3560.
 * 
 * Hydra_Software_Devel/42   1/14/05 8:07p nissen
 * PR 13804: Added register writes to reset M2MC when opening module.
 * 
 * Hydra_Software_Devel/41   1/14/05 4:50p nissen
 * PR 13804: Reinserted M2MC register write to stop op list for C0 code
 * when opening module.
 * 
 * Hydra_Software_Devel/40   12/13/04 2:14p nissen
 * PR 13488: Added support for M2MC C0.
 * 
 * Hydra_Software_Devel/39   9/24/04 4:44p nissen
 * PR 12598: Added code to check if YCbCr422 surfaces have odd rectangle
 * edges.
 * 
 * Hydra_Software_Devel/38   7/23/04 11:25a nissen
 * PR 12031: Removed waiting on finished field for list status register
 * when closing module.
 * 
 * Hydra_Software_Devel/37   6/11/04 10:30p nissen
 * PR 11486: Fixed problemss with scaling.
 * 
 * Hydra_Software_Devel/36   6/3/04 8:29p nissen
 * PR 11349: Added code to wait for all blit operations to complete when
 * closing module.
 * 
 * Hydra_Software_Devel/35   5/24/04 5:08p jasonh
 * PR 11189: Merge down from B0 to main-line
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   4/28/04 2:46p nissen
 * PR 9635: Removed comments.
 * 
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/27/04 7:18p nissen
 * PR 9635: Added support for M2MC B0.
 * 
 * Hydra_Software_Devel/34   4/16/04 11:03a nissen
 * PR 10622: Removed const from pattern variable declaration.
 * 
 * Hydra_Software_Devel/33   4/12/04 11:52p nissen
 * PR 10428: Fixed problem with setting surfaces to null.
 * 
 * Hydra_Software_Devel/32   4/12/04 9:53a pntruong
 * PR 10548: Changed private shift function to non-private.
 * 
 * Hydra_Software_Devel/31   4/9/04 2:19p nissen
 * PR 10380: Modified field setting macros to do mask and set operation
 * for more than one field at a time.
 * 
 * Hydra_Software_Devel/30   3/31/04 11:57a nissen
 * PR 10165: Added code to validate surface dimensions.
 * 
 * Hydra_Software_Devel/29   3/12/04 2:16p nissen
 * PR 10024: Added more error checking.
 * PR 10072: Added support for no-scale filtering.
 * 
 * Hydra_Software_Devel/28   3/5/04 2:33p nissen
 * PR 9856: Fixed problem with rectangle width's and height's being set to
 * zero.
 * 
 * Hydra_Software_Devel/27   2/27/04 1:37p nissen
 * PR 9856: Fixed problem with rectangles when doing a fill.
 * 
 * Hydra_Software_Devel/26   1/30/04 1:43p nissen
 * PR 9537: Removed unnecessary loading of certain register groups.
 * 
 * Hydra_Software_Devel/25   1/21/04 1:58p nissen
 * PR 9432: Added more error checking.
 * 
 * Hydra_Software_Devel/24   12/29/03 6:41p pntruong
 * PR 9117: Refactor BINT to no longer use strings to specify interrupt
 * name.
 * 
 * Hydra_Software_Devel/23   12/23/03 11:26a nissen
 * PR 9078: Reduced interrupt name strings for GFX.
 * 
 * Hydra_Software_Devel/22   12/21/03 8:47p nissen
 * PR 9033: Fixed setting surface address registers when surface is null.
 * 
 * Hydra_Software_Devel/21   12/18/03 2:04p nissen
 * PR 9016: Added support for splitting scaled/filtered blits into
 * vertical strips.
 * 
 * Hydra_Software_Devel/20   12/9/03 1:40p nissen
 * PR 8900: Added code to disable 3-2 pulldown cadence detect on init.
 * 
 * Hydra_Software_Devel/19   11/21/03 6:36p nissen
 * Removed IKOS hacks.
 * 
 * Hydra_Software_Devel/18   11/20/03 6:56p nissen
 * Changed module pointers to handles. Added support for interrupts.
 * 
 * Hydra_Software_Devel/17   10/20/03 1:45p nissen
 * Eliminated warnings.
 * 
 * Hydra_Software_Devel/16   10/9/03 1:35p nissen
 * Removed IKOS hacks.
 * 
 * Hydra_Software_Devel/15   9/29/03 1:29p nissen
 * Modified Close function to return void.
 * 
 * Hydra_Software_Devel/14   9/24/03 10:03a maivu
 * Added missing semicolon(s) to fix build errors.
 * 
 * Hydra_Software_Devel/13   9/23/03 5:51p pntruong
 * Removed the 7038_ from header filename generated from RDB.
 * 
 * Hydra_Software_Devel/12   9/19/03 3:45p nissen
 * Added support for aligning ROP pattern to output x and y coordinates.
 * 
 * Hydra_Software_Devel/11   8/26/03 4:08p nissen
 * Added support for W formats.  Added all to macro to get surface data.
 * 
 * Hydra_Software_Devel/10   8/26/03 10:23a maivu
 * Added the double parathesis for BDBG calls (this fixed compile error  *
 * for msvc60).
 * 
 * Hydra_Software_Devel/9   8/21/03 1:33p nissen
 * Added code to initialize state when module is opened.
 * 
 * Hydra_Software_Devel/8   8/20/03 4:53p nissen
 * Added support for setting default states.
 * 
 * Hydra_Software_Devel/7   8/19/03 10:19a nissen
 * Added functions to reflect changes made to module header file. Removed
 * functions to set component sizes. Added code to check if registers
 * have changed.
 * 
 * Hydra_Software_Devel/6   8/12/03 11:29a nissen
 * Changed usage of field setting macros. Added dither enable function.
 * 
 * Hydra_Software_Devel/5   8/6/03 2:30p nissen
 * Added entry point functions for enabling zero pad/replicate for source
 * and dest. Added entry point functions for setting pixel component
 * sizes for source, dest, and output.
 * 
 * Hydra_Software_Devel/4   6/25/03 12:07p nissen
 * Added code to write registers, initiate blit opertion, and wait for it
 * to finish.
 * 
 * Hydra_Software_Devel/3   6/5/03 5:38p jasonh
 * Changed register file name and added setting of debug module.
 * 
 * Hydra_Software_Devel/2   6/5/03 11:45a nissen
 * Added calls to macros to set surface format type.
 * 
 * Hydra_Software_Devel/1   5/28/03 3:20p nissen
 * Graphics compositor.
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bstd_defs.h"
#include "berr.h"
#include "bkni.h"
#include "bint.h"
#include "bchp.h"

#if ((BCHP_CHIP==7405) && (BCHP_VER >= BCHP_VER_A0))
#include "bchp_int_id_memc16_gfx_l2.h"
#else
#include "bchp_hif_cpu_intr1.h"
#endif

#if ((BCHP_CHIP==7400) && (BCHP_VER >= BCHP_VER_B0))
#include "bchp_memc16_gfx_l2.h"
#include "bchp_memc16_gfx_grb.h"
#elif ((BCHP_CHIP==7405) && (BCHP_VER >= BCHP_VER_A0))
#include "bchp_memc16_gfx_l2.h"
#include "bchp_memc_gfx_grb.h"
#elif ((BCHP_CHIP==7325) && (BCHP_VER >= BCHP_VER_B0)) || \
	  ((BCHP_CHIP==7335) && (BCHP_VER >= BCHP_VER_B0)) || \
	  ((BCHP_CHIP==7336) && (BCHP_VER >= BCHP_VER_A0))
#include "bchp_graphics_l2.h"
#include "bchp_graphics_grb.h"
#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556) || (BCHP_CHIP==35230) || (BCHP_CHIP==35125) || (BCHP_CHIP==35233)
#include "bchp_int_id_gfx_l2.h"
#include "bchp_gfx_rgrb.h"
#elif (BCHP_CHIP==7420) || (BCHP_CHIP==7125)
#include "bchp_gfx_l2.h"
#include "bchp_gfx_gr.h"
#elif (BCHP_CHIP==7344) || (BCHP_CHIP==7346) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || (BCHP_CHIP==7408) || \
	  (BCHP_CHIP==7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7135) || (BCHP_CHIP==7231) || (BCHP_CHIP==7429) || (BCHP_CHIP==7584)
#include "bchp_int_id_gfx_l2.h"
#include "bchp_gfx_gr.h"
#elif (BCHP_CHIP==7468)
#include "bchp_m2mc_wrap_l2.h"
#include "bchp_m2mc_wrap_gr_bridge.h"
#elif (BCHP_CHIP==7552) || (BCHP_CHIP==7550) || (BCHP_CHIP==7358) || (BCHP_CHIP==7360) || (BCHP_CHIP==7563)
#include "bchp_m2mc_top_l2.h"
#include "bchp_m2mc_top_gr_bridge.h"
#elif ((BCHP_CHIP==7640) && (BCHP_VER >= BCHP_VER_B0))
#include "bchp_wrap_m2mc_l2.h"
#include "bchp_wrap_m2mc_grb.h"
#include "bchp_gfx_grb.h"
#elif (BCHP_CHIP==7435) || (BCHP_CHIP==7445)
#include "bchp_m2mc_l2.h"
#include "bchp_m2mc1_l2.h"
#include "bchp_m2mc_gr.h"
#include "bchp_m2mc1_gr.h"
#include "bchp_int_id_m2mc1_l2.h"
#else
#include "bchp_int_id_gfx_l2.h"
#include "bchp_gfx_grb.h"
#endif

#if (BCHP_CHIP==7435) || (BCHP_CHIP==7445)
#include "bchp_m2mc.h"
#include "bchp_m2mc1.h"
#else
#include "bchp_m2mc.h"
#endif

#if ((BCHP_CHIP==7400) && (BCHP_VER == BCHP_VER_A0)) || \
	((BCHP_CHIP==7440) && (BCHP_VER >= BCHP_VER_A0)) || \
	((BCHP_CHIP==7640) && (BCHP_VER >= BCHP_VER_B0))
#include "bchp_m2mc_1.h"
#elif ((BCHP_CHIP==7438) && (BCHP_VER >= BCHP_VER_A0))
#include "bchp_m2mc_1.h"
#include "bchp_m2mc_1_grb.h"
#include "bchp_int_id_m2mc_1_l2.h"
#endif

#if ((BCHP_CHIP==7405) && (BCHP_VER >= BCHP_VER_A0))
#include "bchp_memc_1_1.h"
#endif

#include "bchp_common.h"
#include "bgrc.h"
#include "bgrc_private.h"

BDBG_MODULE(BGRC);
BDBG_OBJECT_ID(BGRC);

/***************************************************************************/
#define BGRC_P_SURFACE_RECT_SIZE_MAX    8191

/***************************************************************************/
static const BGRC_Settings BGRC_P_DEFAULT_SETTINGS =
{
	BGRC_PACKET_MEMORY_MAX,   /* ulPacketMemoryMax */
	BGRC_OPERATION_MAX,       /* ulOperationMax */
	0,                        /* ulDeviceNum */
	BGRC_WAIT_TIMEOUT,        /* ulWaitTimeout */
	true                      /* bPreAllocMemory */
};

/***************************************************************************/
BERR_Code BGRC_GetDefaultSettings(
	BGRC_Settings *pDefSettings )
{
	BDBG_ENTER(BGRC_GetDefaultSettings);

	if( pDefSettings )
		*pDefSettings = BGRC_P_DEFAULT_SETTINGS;

	BDBG_LEAVE(BGRC_GetDefaultSettings);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Open( 
	BGRC_Handle *phGrc,
	BCHP_Handle hChip,
	BREG_Handle hRegister,
	BMEM_Handle hMemory,
	BINT_Handle hInterrupt,
	const BGRC_Settings *pDefSettings )
{
	BGRC_Handle hGrc = 0;
	BGRC_P_State *pState;
	BERR_Code err = BERR_SUCCESS;
	const int32_t ai32_Matrix[20] = { 0 };
	const uint8_t aucPattern[8] = { 0 };

#if ((BCHP_CHIP==7438) && (BCHP_VER >= BCHP_VER_A0))
	BINT_Id IntID = BCHP_INT_ID_GFX_L2_M2MC_INTR;
#elif ((BCHP_CHIP==7400) && (BCHP_VER >= BCHP_VER_B0))
	BINT_Id IntID = BCHP_INT_ID_CREATE(BCHP_MEMC16_GFX_L2_CPU_STATUS, BCHP_MEMC16_GFX_L2_CPU_STATUS_M2MC_INTR_SHIFT);
#elif ((BCHP_CHIP==7405) && (BCHP_VER >= BCHP_VER_A0))
	BINT_Id IntID = BCHP_INT_ID_M2MC_0_INTR;
#elif ((BCHP_CHIP==7325) && (BCHP_VER >= BCHP_VER_B0)) || \
	  ((BCHP_CHIP==7335) && (BCHP_VER >= BCHP_VER_B0)) ||  \
	  ((BCHP_CHIP==7336) && (BCHP_VER >= BCHP_VER_A0))
	BINT_Id IntID = BCHP_INT_ID_CREATE(BCHP_GRAPHICS_L2_CPU_STATUS, BCHP_GRAPHICS_L2_CPU_STATUS_M2MC_0_INTR_SHIFT);
#elif (BCHP_CHIP==7420) || (BCHP_CHIP==7125)
	BINT_Id IntID = BCHP_INT_ID_CREATE(BCHP_GFX_L2_CPU_STATUS, BCHP_GFX_L2_CPU_STATUS_M2MC_INTR_SHIFT);
#elif (BCHP_CHIP==7468)
	BINT_Id IntID = BCHP_INT_ID_CREATE(BCHP_M2MC_WRAP_L2_CPU_STATUS, BCHP_M2MC_WRAP_L2_CPU_STATUS_M2MC_INTR_SHIFT);
#elif (BCHP_CHIP==7552) || (BCHP_CHIP==7550) || (BCHP_CHIP==7358) || (BCHP_CHIP==7360) || (BCHP_CHIP==7563)
	BINT_Id IntID = BCHP_INT_ID_CREATE(BCHP_M2MC_TOP_L2_CPU_STATUS, BCHP_M2MC_TOP_L2_CPU_STATUS_M2MC_INTR_SHIFT);
#elif ((BCHP_CHIP==7640) && (BCHP_VER >= BCHP_VER_B0))
	BINT_Id IntID = BCHP_INT_ID_CREATE(BCHP_WRAP_M2MC_L2_CPU_STATUS, BCHP_WRAP_M2MC_L2_CPU_STATUS_M2MC_0_INTR_SHIFT);
#elif (BCHP_CHIP==7435) || (BCHP_CHIP==7445)
	BINT_Id IntID = BCHP_INT_ID_CREATE(BCHP_M2MC_L2_CPU_STATUS, BCHP_M2MC_L2_CPU_STATUS_M2MC_INTR_SHIFT);
#else
	BINT_Id IntID = BCHP_INT_ID_M2MC_INTR;
#endif

	BDBG_ENTER(BGRC_Open);
	BDBG_ASSERT( phGrc );
	BDBG_ASSERT( hChip );
	BDBG_ASSERT( hRegister );
	BDBG_ASSERT( hMemory );
	BDBG_ASSERT( hInterrupt );

	/* allocate memory for private data */
	hGrc = (BGRC_Handle) BKNI_Malloc( sizeof (BGRC_P_Handle) );
	if( hGrc == 0 )
	{
		err = BERR_OUT_OF_SYSTEM_MEMORY;
		goto fail;
	}

	/* clear main data structure */
	BKNI_Memset( (void *) hGrc, 0, sizeof (BGRC_P_Handle) );
	BDBG_OBJECT_SET(hGrc, BGRC);

	/* create interrupt events */
	err = BKNI_CreateEvent( &hGrc->hInterruptEvent );
	if( err != BERR_SUCCESS )
		goto fail;

	BKNI_ResetEvent( hGrc->hInterruptEvent );

	err = BKNI_CreateEvent( &hGrc->hPeriodicEvent );
	if( err != BERR_SUCCESS )
		goto fail;

	BKNI_ResetEvent( hGrc->hPeriodicEvent );
	BKNI_SetEvent( hGrc->hPeriodicEvent );

	/* set private data */
	hGrc->hChip = hChip;
	hGrc->hRegister = hRegister;
	hGrc->hMemory = hMemory;
	hGrc->hInterrupt = hInterrupt;
	hGrc->ulPacketMemoryMax = pDefSettings ? pDefSettings->ulPacketMemoryMax : BGRC_P_DEFAULT_SETTINGS.ulPacketMemoryMax;
	hGrc->ulOperationMax = pDefSettings ? pDefSettings->ulOperationMax : BGRC_P_DEFAULT_SETTINGS.ulOperationMax;
	hGrc->ulDeviceNum = pDefSettings ? pDefSettings->ulDeviceNum : BGRC_P_DEFAULT_SETTINGS.ulDeviceNum;
	hGrc->ulWaitTimeout = pDefSettings ? pDefSettings->ulWaitTimeout : BGRC_P_DEFAULT_SETTINGS.ulWaitTimeout;
	hGrc->bPreAllocMemory = pDefSettings ? pDefSettings->bPreAllocMemory : BGRC_P_DEFAULT_SETTINGS.bPreAllocMemory;
	hGrc->bUninitialized = true;
	pState = &hGrc->CurrentState;

	hGrc->ulPacketMemoryMax = BGRC_P_MAX(hGrc->ulPacketMemoryMax, BGRC_P_LIST_BLOCK_MIN_SIZE);
	hGrc->ulOperationMax = BGRC_P_MAX(hGrc->ulOperationMax, BGRC_P_OPERATION_MIN);

	/* validate choosen m2mc device */
#if ((BCHP_CHIP!=7400) && (BCHP_CHIP!=7438) && (BCHP_CHIP!=7440) && (BCHP_CHIP!=7435)) || (BCHP_CHIP!=7445) || \
	((BCHP_CHIP==7400) && (BCHP_VER >= BCHP_VER_B0))
	if( hGrc->ulDeviceNum )
	{
		err = BGRC_ERR_M2MC_DEVICE_NUM_INVALID;
		goto fail;
	}
#endif

	if( hGrc->ulDeviceNum > 1 )
	{
		err = BGRC_ERR_M2MC_DEVICE_NUM_INVALID;
		goto fail;
	}

	/* reset m2mc */
#if ((BCHP_CHIP==7400) && (BCHP_VER >= BCHP_VER_B0))
	BREG_Write32( hGrc->hRegister, BCHP_MEMC16_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(MEMC16_GFX_GRB_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_MEMC16_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(MEMC16_GFX_GRB_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
#elif ((BCHP_CHIP==7405) && (BCHP_VER >= BCHP_VER_A0))
	BREG_Write32( hGrc->hRegister, BCHP_MEMC_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(MEMC_GFX_GRB_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_MEMC_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(MEMC_GFX_GRB_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
#elif ((BCHP_CHIP==7325) && (BCHP_VER >= BCHP_VER_B0)) || \
	  ((BCHP_CHIP==7335) && (BCHP_VER >= BCHP_VER_B0)) || \
	  ((BCHP_CHIP==7336) && (BCHP_VER >= BCHP_VER_A0))
	BREG_Write32( hGrc->hRegister, BCHP_GRAPHICS_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GRAPHICS_GRB_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_GRAPHICS_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GRAPHICS_GRB_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556) || (BCHP_CHIP==35230) || (BCHP_CHIP==35125) || (BCHP_CHIP==35233)
	BREG_Write32( hGrc->hRegister, BCHP_GFX_RGRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_RGRB_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_GFX_RGRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_RGRB_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
#elif (BCHP_CHIP==7420) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || (BCHP_CHIP==7125) || (BCHP_CHIP==7408)
	BREG_Write32( hGrc->hRegister, BCHP_GFX_GR_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GR_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_GFX_GR_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GR_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
#elif (BCHP_CHIP==7468)
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_WRAP_GR_BRIDGE_SW_RESET_0, BCHP_FIELD_ENUM(M2MC_WRAP_GR_BRIDGE_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_WRAP_GR_BRIDGE_SW_RESET_0, BCHP_FIELD_ENUM(M2MC_WRAP_GR_BRIDGE_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
#elif (BCHP_CHIP==7344) || (BCHP_CHIP==7346) || (BCHP_CHIP==7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7135) || (BCHP_CHIP==7231) || (BCHP_CHIP==7429) || (BCHP_CHIP==7584)

	BREG_Write32( hGrc->hRegister, BCHP_GFX_GR_SW_INIT_0, BCHP_FIELD_ENUM(GFX_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_GFX_GR_SW_INIT_0, BCHP_FIELD_ENUM(GFX_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL, BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL_START_SW_INIT_MASK );

	while( BREG_Read32(hRegister, BCHP_M2MC_BLIT_STATUS) == 0 );

	BREG_Write32( hGrc->hRegister, BCHP_GFX_GR_SW_INIT_0, BCHP_FIELD_ENUM(GFX_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_GFX_GR_SW_INIT_0, BCHP_FIELD_ENUM(GFX_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );

#elif (BCHP_CHIP==7552)
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_TOP_GR_BRIDGE_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_TOP_GR_BRIDGE_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_TOP_GR_BRIDGE_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_TOP_GR_BRIDGE_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );
#elif (BCHP_CHIP==7550)
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_TOP_GR_BRIDGE_SW_RESET_0, BCHP_FIELD_ENUM(M2MC_TOP_GR_BRIDGE_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_TOP_GR_BRIDGE_SW_RESET_0, BCHP_FIELD_ENUM(M2MC_TOP_GR_BRIDGE_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
#elif (BCHP_CHIP==7358) || (BCHP_CHIP==7360) || (BCHP_CHIP==7563)
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_TOP_GR_BRIDGE_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_TOP_GR_BRIDGE_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_TOP_GR_BRIDGE_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_TOP_GR_BRIDGE_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );
#elif (BCHP_CHIP==7435) || (BCHP_CHIP==7445)
	if( hGrc->ulDeviceNum )
	{
		BREG_Write32( hGrc->hRegister, BCHP_M2MC1_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC1_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC1_CLK_GATE_AND_SW_INIT_CONTROL, BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL_START_SW_INIT_MASK );
		while( BREG_Read32(hRegister, BCHP_M2MC1_BLIT_STATUS) == 0 );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC1_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC1_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );
	}
	else
	{
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL, BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL_START_SW_INIT_MASK );
		while( BREG_Read32(hRegister, BCHP_M2MC_BLIT_STATUS) == 0 );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );
	}
#else
#if ((BCHP_CHIP==7400) && (BCHP_VER == BCHP_VER_A0)) || \
	((BCHP_CHIP==7440) && (BCHP_VER >= BCHP_VER_A0))
	if( hGrc->ulDeviceNum )
	{
		BREG_Write32( hGrc->hRegister, BCHP_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GRB_SW_RESET_0, M2MC_1_SW_RESET, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GRB_SW_RESET_0, M2MC_1_SW_RESET, DEASSERT) );
	}
	else
#elif ((BCHP_CHIP==7438) && (BCHP_VER >= BCHP_VER_A0))
	if( hGrc->ulDeviceNum )
	{
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_1_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GRB_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_1_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GRB_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
	}
	else
#endif
	{
		BREG_Write32( hGrc->hRegister, BCHP_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GRB_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GRB_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
	}
#endif

	/* reset m2mc list */
	BGRC_P_WRITE_REG( LIST_CTRL, BCHP_FIELD_ENUM(M2MC_LIST_CTRL, RUN, Stop) );

	/* create interrupt callback */
#if ((BCHP_CHIP==7400) && (BCHP_VER == BCHP_VER_A0)) || \
	((BCHP_CHIP==7440) && (BCHP_VER >= BCHP_VER_A0))
	IntID = hGrc->ulDeviceNum ? BCHP_INT_ID_M2MC_1_INTR : BCHP_INT_ID_M2MC_INTR;
#elif ((BCHP_CHIP==7438) && (BCHP_VER >= BCHP_VER_A0))
	IntID = hGrc->ulDeviceNum ? BCHP_INT_ID_M2MC_1_L2_M2MC_INTR : BCHP_INT_ID_GFX_L2_M2MC_INTR;
#elif (BCHP_CHIP==7435) || (BCHP_CHIP==7445)
	IntID = hGrc->ulDeviceNum ? BCHP_INT_ID_M2MC1_L2_M2MC_INTR : IntID;
#endif

	err = BINT_CreateCallback( &hGrc->hInterruptCallback, hInterrupt, IntID, BGRC_P_List_PacketIsr, hGrc, 0 );
	if( err != BERR_SUCCESS )
		goto fail;

	/* clear interrupt callback */
	err = BINT_ClearCallback( hGrc->hInterruptCallback );
	if( err != BERR_SUCCESS )
		goto fail;

	/* enable interrupt callback */
	err = BINT_EnableCallback( hGrc->hInterruptCallback );
	if( err != BERR_SUCCESS )
		goto fail;

	/* initialize list packet memory */
	if( !BGRC_P_List_InitPacketMemory( hGrc, hGrc->bPreAllocMemory ? hGrc->ulPacketMemoryMax : BGRC_P_LIST_BLOCK_SIZE ) )
	{
		err = BERR_OUT_OF_DEVICE_MEMORY;
		goto fail;
	}

	/* create surface for waiting */
	err = BSUR_Surface_Create( hGrc->hMemory, 1, 1, 0, NULL, BPXL_eA8_R8_G8_B8, 0, 0, NULL, &hGrc->hWaitSurface );
	if( err != BERR_SUCCESS )
		return BERR_TRACE(err);

	/* preallocate memory for operation structures */
	if( hGrc->bPreAllocMemory )
	{
		if( !BGRC_P_Operation_Prealloc( hGrc, hGrc->ulOperationMax ) )
		{
			err = BERR_OUT_OF_SYSTEM_MEMORY;
			goto fail;
		}
	}

#if defined(BCHP_M2MC_BLIT_CTRL_BLOCK_AUTO_SPLIT_FIFO_MASK)
	BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, BLOCK_AUTO_SPLIT_FIFO, ENABLE );
#endif

	/* set palette bypass field */
	BGRC_P_SET_FIELD_ENUM( DEST_SURFACE_FORMAT_DEF_3, PALETTE_BYPASS, DONT_LOOKUP );
	pState->bDstPaletteBypass = true;

	/* set default source state */
	BGRC_Source_SetSurface( hGrc, NULL );
	BGRC_Source_SetAlphaSurface( hGrc, NULL );
	BGRC_Source_TogglePaletteBypass( hGrc, false );
	BGRC_Source_SetChromaExpansion( hGrc, BGRC_ChromaExpansion_eReplicate );
	BGRC_Source_SetZeroPad( hGrc, false );
	BGRC_Source_SetColorKey( hGrc, 0, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF, false );
	BGRC_Source_ToggleColorKey( hGrc, false );
	BGRC_Source_SetColorMatrix5x4( hGrc, ai32_Matrix, 0 );
	BGRC_Source_ToggleColorMatrix( hGrc, false );
	BGRC_Source_SetColorMatrixRounding( hGrc, BGRC_Rounding_eTruncate );
	BGRC_Source_SetColor( hGrc, 0xFF000000 );
	BGRC_Source_SetRectangle( hGrc, 0, 0, 0, 0 );
	BGRC_Source_SetFilterCoeffs( hGrc, BGRC_FilterCoeffs_eSharp, BGRC_FilterCoeffs_eSharp );
	BGRC_Source_ToggleFilter( hGrc, false, false );
	BGRC_Source_SetFilterPhaseAdjustment( hGrc, 0, 0, 0 );
	BGRC_Source_SetScaleRounding( hGrc, BGRC_Rounding_eNearest );
	BGRC_Source_SetScaleAlphaAdjust( hGrc, false );

#if 0/*def BCHP_M2MC_BLIT_HEADER_CBAR_SRC_COLOR_SCALE_THEN_KEY_THEN_MATRIX*/
	BGRC_Source_SetKeyMatrixScaleOrder( hGrc, BGRC_KeyMatrixScaleOrder_eKeyThenScaleThenMatrix );
#else
	BGRC_Source_SetKeyMatrixOrder( hGrc, BGRC_KeyMatrixOrder_eKeyThenMatrix );
#endif

#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_PRE_MULTIPLY_MASK)
	BGRC_Source_SetScaleAlphaPreMultiply( hGrc, false );
#endif

#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_PRE_MULTIPLY_ENABLE_MASK)
	BGRC_Source_SetAlphaPreMultiply( hGrc, false );
#endif

#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_PRE_MULTIPLY_OFFSET_EN_MASK)
	BGRC_Source_SetAlphaPreMultiplyOffset( hGrc, false );
#endif

#if defined(BCHP_M2MC_SCALER_CTRL_EDGE_CONDITION_MASK)
	BGRC_Source_SetScaleEdgeCondition( hGrc, BGRC_EdgeCondition_eReplicateLast );
#endif

#if defined(BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_RANGE_EXP_MAP_SCALE_FACTOR_Y_MASK)
	BGRC_Source_SetMacroBlock( hGrc, BGRC_MacroBlockRange_None, BGRC_MacroBlockRange_None, 0, 0 );
	BGRC_Source_SetMacroBlock_StripWidth( hGrc, BGRC_P_YCbCr420_STRIP_WIDTH );
#endif

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	BGRC_Source_SetMacroBlock_LinearFormat( hGrc, false );
#endif

	/* set default destination state */
	BGRC_Destination_SetSurface( hGrc, NULL );
	BGRC_Destination_SetAlphaSurface( hGrc, NULL );
	BGRC_Destination_SetChromaExpansion( hGrc, BGRC_ChromaExpansion_eReplicate );
	BGRC_Destination_SetZeroPad( hGrc, false );
	BGRC_Destination_SetColorKey( hGrc, 0, 0, 0xFFFFFFFF, 0, 0xFFFFFFFF, false );
	BGRC_Destination_ToggleColorKey( hGrc, false );
	BGRC_Destination_TogglePaletteBypass( hGrc, false );
#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	BGRC_Destination_SetColorMatrix5x4( hGrc, ai32_Matrix, 0 );
	BGRC_Destination_ToggleColorMatrix( hGrc, false );
	BGRC_Destination_SetColorMatrixRounding( hGrc, BGRC_Rounding_eTruncate );
	BGRC_Destination_SetKeyMatrixOrder( hGrc, BGRC_KeyMatrixOrder_eKeyThenMatrix );
#endif
	BGRC_Destination_SetColor( hGrc, 0xFF000000 );
	BGRC_Destination_SetRectangle( hGrc, 0, 0, 0, 0 );

	/* set default pattern state */
	BGRC_Pattern_Set( hGrc, 0, aucPattern, 0, 0 );

	/* set default blend state */
	BGRC_Blend_SetColor( hGrc, 0xFF000000 );

	err = BGRC_Blend_SetColorBlend( hGrc, 
		BGRC_Blend_Source_eSourceColor, BGRC_Blend_Source_eOne, false,
		BGRC_Blend_Source_eZero, BGRC_Blend_Source_eZero, false, 
		BGRC_Blend_Source_eZero );
	if( err != BERR_SUCCESS )
		goto fail;

	err = BGRC_Blend_SetAlphaBlend( hGrc, 
		BGRC_Blend_Source_eSourceAlpha, BGRC_Blend_Source_eOne, false,
		BGRC_Blend_Source_eZero, BGRC_Blend_Source_eZero, false, 
		BGRC_Blend_Source_eZero );
	if( err != BERR_SUCCESS )
		goto fail;

	/* set default output state */
	BGRC_Output_SetSurface( hGrc, NULL );
	BGRC_Output_SetAlphaSurface( hGrc, NULL );
#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	BGRC_Output_SetColorMatrix5x4( hGrc, ai32_Matrix, 0 );
	BGRC_Output_ToggleColorMatrix( hGrc, false );
	BGRC_Output_SetColorMatrixRounding( hGrc, BGRC_Rounding_eTruncate );
#endif
	BGRC_Output_SetRectangle( hGrc, 0, 0, 0, 0 );
	BGRC_Output_SetColorKeySelection( hGrc, 
		BGRC_Output_ColorKeySelection_eTakeSource, 
		BGRC_Output_ColorKeySelection_eTakeSource, 
		BGRC_Output_ColorKeySelection_eTakeDestination, 
		BGRC_Output_ColorKeySelection_eTakeDestination );
	BGRC_Output_SetDither( hGrc, false );
	BGRC_Source_SetFixedScaleFactor( hGrc, 0, 0, 0, 0 );

	hGrc->bUninitialized = false;

	/* copy current state to default state */
	BGRC_P_Source_CopyState( &hGrc->DefaultState, &hGrc->CurrentState, 
		hGrc->aulDefaultRegs, hGrc->aulCurrentRegs );
	BGRC_P_Destination_CopyState( &hGrc->DefaultState, &hGrc->CurrentState, 
		hGrc->aulDefaultRegs, hGrc->aulCurrentRegs );
	BGRC_P_Pattern_CopyState( &hGrc->DefaultState, &hGrc->CurrentState, 
		hGrc->aulDefaultRegs, hGrc->aulCurrentRegs );
	BGRC_P_Blend_CopyState( &hGrc->DefaultState, &hGrc->CurrentState, 
		hGrc->aulDefaultRegs, hGrc->aulCurrentRegs );
	BGRC_P_Output_CopyState( &hGrc->DefaultState, &hGrc->CurrentState, 
		hGrc->aulDefaultRegs, hGrc->aulCurrentRegs );

	/* enable loading of all registers groups */
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_FEEDER_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, OUTPUT_FEEDER_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_COLOR_KEY_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_COLOR_MATRIX_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_COLOR_KEY_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_COLOR_MATRIX_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, OUTPUT_COLOR_MATRIX_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
#endif
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLEND_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
	BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, ROP_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

#if ((BCHP_CHIP==7038) && (BCHP_VER <  BCHP_VER_C0)) || \
	((BCHP_CHIP==3560) && (BCHP_VER >= BCHP_VER_B0)) || \
	((BCHP_CHIP==3563) && (BCHP_VER >= BCHP_VER_A0))
	/* disable 3-2 pulldown cadence detect */
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_BLEND_PULLDOWN_COUNTER_ENABLE, 
		BCHP_M2MC_BLEND_PULLDOWN_COUNTER_ENABLE_ENABLE_DISABLE );
#endif
    
	/* set return handle */
	*phGrc = hGrc;

#if ((BCHP_CHIP==7405) && (BCHP_VER >= BCHP_VER_A0))
{
	uint32_t reg;
	reg = BREG_Read32(hRegister, BCHP_MEMC_1_1_CLIENT_INFO_2);
	reg |= BCHP_MEMC_1_1_CLIENT_INFO_2_RR_EN_MASK;
	BREG_Write32(hRegister, BCHP_MEMC_1_1_CLIENT_INFO_2, reg);
	reg = BREG_Read32(hRegister, BCHP_MEMC_1_1_CLIENT_INFO_2);
}
#endif

	BDBG_LEAVE(BGRC_Open);
	return BERR_SUCCESS;

fail:
	if( hGrc )
	{
		if( hGrc->hWaitSurface )
			BSUR_Surface_Destroy( hGrc->hWaitSurface );

		if( hGrc->hPeriodicEvent )
			BKNI_DestroyEvent( hGrc->hPeriodicEvent );

		if( hGrc->hInterruptEvent )
			BKNI_DestroyEvent( hGrc->hInterruptEvent );

		if( hGrc->hInterruptCallback )
			BINT_DestroyCallback( hGrc->hInterruptCallback );

		BKNI_Free( (void *) hGrc );
	}

	BDBG_LEAVE(BGRC_Open);
	return BERR_TRACE(err);
}

/***************************************************************************/
void BGRC_Close(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_Close);
	BDBG_ASSERT( hGrc );

	/* wait for device to finish */
	(void)BGRC_WaitForOperationsComplete( hGrc, NULL, NULL );

	/* free operation memory */
	BGRC_P_Operation_CleanupList( hGrc );
	BGRC_P_Operation_FreeAll( hGrc );

	/* free list packet memory */
	BGRC_P_List_FreePacketMemory( hGrc );

	/* free wait surface */
	BSUR_Surface_Destroy( hGrc->hWaitSurface );

	/* destroy interrupt events */
	BKNI_DestroyEvent( hGrc->hInterruptEvent );
	BKNI_DestroyEvent( hGrc->hPeriodicEvent );

	/* disable and destroy interrupt callback */
	BINT_DisableCallback( hGrc->hInterruptCallback );
	BINT_DestroyCallback( hGrc->hInterruptCallback );

	/* free memory for private data */
	BDBG_OBJECT_DESTROY(hGrc, BGRC);
	BKNI_Free( (void *) hGrc );

	BDBG_LEAVE(BGRC_Close);
}

/***************************************************************************/
BERR_Code BGRC_ResetState(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_ResetState);
	BDBG_ASSERT( hGrc );

	/* reset all states */
	BGRC_Source_ResetState( hGrc );
	BGRC_Destination_ResetState( hGrc );
	BGRC_Blend_ResetState( hGrc );
	BGRC_Pattern_ResetState( hGrc );
	BGRC_Output_ResetState( hGrc );

	BDBG_LEAVE(BGRC_ResetState);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_P_IssueState(
	BGRC_Handle   hGrc,
	BGRC_Callback pCallback_isr,
	void         *pData )
{
	BGRC_P_State *pState;
	BERR_Code err;

	pState = &hGrc->CurrentState;

	/* check if there is an output surface */
	if( pState->OutSurface.hSurface == 0 )
		return BERR_TRACE(BGRC_ERR_NO_OUTPUT_SURFACE);

	/* validate surface dimensions */
	if( !BGRC_P_VALIDATE_SURFACE_RECTANGLE( &pState->SrcRect ) )
		return BERR_TRACE(BGRC_ERR_SOURCE_DIMENSIONS_INVALID);

	if( !BGRC_P_VALIDATE_SURFACE_RECTANGLE( &pState->DstRect ) )
		return BERR_TRACE(BGRC_ERR_DESTINATION_DIMENSIONS_INVALID);

	if( !BGRC_P_VALIDATE_SURFACE_RECTANGLE( &pState->OutRect ) )
		return BERR_TRACE(BGRC_ERR_OUTPUT_DIMENSIONS_INVALID);

	/* set surface dimensions */
	pState->SrcSurface.ulX = pState->SrcRect.ulX;
	pState->SrcSurface.ulY = pState->SrcRect.ulY;
	pState->SrcSurface.ulWidth = pState->SrcRect.ulWidth ? pState->SrcRect.ulWidth : pState->SrcSurface.ulSurfaceWidth;
	pState->SrcSurface.ulHeight = pState->SrcRect.ulHeight ? pState->SrcRect.ulHeight : pState->SrcSurface.ulSurfaceHeight;
	pState->DstSurface.ulX = pState->DstRect.ulX;
	pState->DstSurface.ulY = pState->DstRect.ulY;
	pState->DstSurface.ulWidth = pState->DstRect.ulWidth ? pState->DstRect.ulWidth : pState->DstSurface.ulSurfaceWidth;
	pState->DstSurface.ulHeight = pState->DstRect.ulHeight ? pState->DstRect.ulHeight : pState->DstSurface.ulSurfaceHeight;
	pState->OutSurface.ulX = pState->OutRect.ulX;
	pState->OutSurface.ulY = pState->OutRect.ulY;
	pState->OutSurface.ulWidth = pState->OutRect.ulWidth ? pState->OutRect.ulWidth : pState->OutSurface.ulSurfaceWidth;
	pState->OutSurface.ulHeight = pState->OutRect.ulHeight ? pState->OutRect.ulHeight : pState->OutSurface.ulSurfaceHeight;

	/* validate surface rectangles */
	if( !BGRC_P_VALIDATE_SURFACE_BOUNDS( pState, Src ) )
		return BERR_TRACE(BGRC_ERR_SOURCE_RECT_OUT_OF_BOUNDS);

	if( !BGRC_P_VALIDATE_SURFACE_BOUNDS( pState, Dst ) )
		return BERR_TRACE(BGRC_ERR_DESTINATION_RECT_OUT_OF_BOUNDS);

	if( !BGRC_P_VALIDATE_SURFACE_BOUNDS( pState, Out ) )
		return BERR_TRACE(BGRC_ERR_OUTPUT_RECT_OUT_OF_BOUNDS);

	/* check if destination and output rectangle sizes are the same */
	if( pState->DstSurface.hSurface && pState->OutSurface.hSurface && (
		(pState->DstSurface.ulWidth != pState->OutSurface.ulWidth) || 
		(pState->DstSurface.ulHeight != pState->OutSurface.ulHeight)) )
		return BERR_TRACE(BGRC_ERR_DESTINATION_DIMENSIONS_INVALID);

	/* check if rectangle has odd egdes when the surface is YCbCr422 */
	if( pState->SrcSurface.hSurface && BPXL_IS_YCbCr422_FORMAT(pState->SrcSurface.eFormat) && 
		((pState->SrcSurface.ulX & 1) || (pState->SrcSurface.ulWidth & 1)) )
		return BERR_TRACE(BGRC_ERR_YCBCR422_SURFACE_HAS_ODD_EDGE);

	if( pState->DstSurface.hSurface && BPXL_IS_YCbCr422_FORMAT(pState->DstSurface.eFormat) && 
		((pState->DstSurface.ulX & 1) || (pState->DstSurface.ulWidth & 1)) )
		return BERR_TRACE(BGRC_ERR_YCBCR422_SURFACE_HAS_ODD_EDGE);

	if( pState->OutSurface.hSurface && BPXL_IS_YCbCr422_FORMAT(pState->OutSurface.eFormat) && 
		((pState->OutSurface.ulX & 1) || (pState->OutSurface.ulWidth & 1)) )
		return BERR_TRACE(BGRC_ERR_YCBCR422_SURFACE_HAS_ODD_EDGE);

	/* Fire interrupt periodically so that operations can be marked as completed and memory can be reallocated. */
	if( hGrc->bPreAllocMemory )
	{
		if( hGrc->ulPacketMemorySinceInterrupt > BGRC_P_LIST_BLOCK_SIZE * 4 )
		{
			hGrc->bPeriodicInterrupt = true;
			hGrc->ulPeriodicInterrupts++;
			hGrc->ulPacketMemorySinceInterrupt = 0;
		}
	}

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	/* check for YCbCr420 source */
	hGrc->bYCbCr420Source = false;
	if( ((pState->SrcSurface.eFormat == BPXL_eY8) || (pState->SrcSurface.eFormat == BPXL_eA8_Y8)) &&
		((pState->SrcAlphaSurface.eFormat == BPXL_eCb8_Cr8) || (pState->SrcAlphaSurface.eFormat == BPXL_eCr8_Cb8)) )
	{
		hGrc->bYCbCr420Source = true;
	}
#endif

	/* check for no scale filtering */
	hGrc->bNoScaleFilter = false;
	if( pState->SrcSurface.hSurface && 
		(pState->SrcSurface.ulWidth == pState->OutSurface.ulWidth) && 
		(pState->SrcSurface.ulHeight == pState->OutSurface.ulHeight)  )
	{
		if( pState->bHorzFilter || pState->bVertFilter )
			hGrc->bNoScaleFilter = true;

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
		if( hGrc->bYCbCr420Source )
			hGrc->bNoScaleFilter = true;
#endif
	}

	/* check if scaling or filtering */
	if( hGrc->bNoScaleFilter || (pState->SrcSurface.hSurface && (
		(pState->SrcSurface.ulWidth != pState->OutSurface.ulWidth) ||
		(pState->SrcSurface.ulHeight != pState->OutSurface.ulHeight))) )
	{
		/* start filter blit operation */
		err = BGRC_P_FilterBlit( hGrc, pCallback_isr, pData, hGrc->bSetEvent );
		if( err != BERR_SUCCESS )
			return BERR_TRACE(err);
	}
	else
	{
		pState->ulHorzScalerStep = (1 << BGRC_P_SCALER_STEP_FRAC_BITS);
		pState->ulVertScalerStep = (1 << BGRC_P_SCALER_STEP_FRAC_BITS);
		pState->ulHorzAveragerCount = 1;
		pState->ulVertAveragerCount = 1;

		/* start blit operation */
		err = BGRC_P_Blit( hGrc, pCallback_isr, pData, hGrc->bSetEvent );
		if( err != BERR_SUCCESS )
			return err;
	}

	/* dump debug info */
	if( pState->SrcSurface.hSurface && pState->DstSurface.hSurface )
	{
		BDBG_MSG(("GRC Blit: Src(o=%08x,f=%08x,r=%dx%d-%dx%d,s=%dx%d) + Dst(o=%08x,f=%08x,r=%dx%d-%dx%d,s=%dx%d) -> Out(o=%08x,f=%08x,r=%dx%d-%dx%d,s=%dx%d) %s", 
			pState->SrcSurface.ulOffset, pState->SrcSurface.eFormat, pState->SrcSurface.ulX, pState->SrcSurface.ulY, pState->SrcSurface.ulWidth, pState->SrcSurface.ulHeight, pState->SrcSurface.ulSurfaceWidth, pState->SrcSurface.ulSurfaceHeight, 
			pState->DstSurface.ulOffset, pState->DstSurface.eFormat, pState->DstSurface.ulX, pState->DstSurface.ulY, pState->DstSurface.ulWidth, pState->DstSurface.ulHeight, pState->DstSurface.ulSurfaceWidth, pState->DstSurface.ulSurfaceHeight, 
			pState->OutSurface.ulOffset, pState->OutSurface.eFormat, pState->OutSurface.ulX, pState->OutSurface.ulY, pState->OutSurface.ulWidth, pState->OutSurface.ulHeight, pState->OutSurface.ulSurfaceWidth, pState->OutSurface.ulSurfaceHeight,
			hGrc->bSetEvent ? "wait" : "async"));
	}
	else if( pState->SrcSurface.hSurface )
	{
		BDBG_MSG(("GRC Blit: Src(o=%08x,f=%08x,r=%dx%d-%dx%d,s=%dx%d) -> Out(o=%08x,f=%08x,r=%dx%d-%dx%d,s=%dx%d) %s", 
			pState->SrcSurface.ulOffset, pState->SrcSurface.eFormat, pState->SrcSurface.ulX, pState->SrcSurface.ulY, pState->SrcSurface.ulWidth, pState->SrcSurface.ulHeight, pState->SrcSurface.ulSurfaceWidth, pState->SrcSurface.ulSurfaceHeight, 
			pState->OutSurface.ulOffset, pState->OutSurface.eFormat, pState->OutSurface.ulX, pState->OutSurface.ulY, pState->OutSurface.ulWidth, pState->OutSurface.ulHeight, pState->OutSurface.ulSurfaceWidth, pState->OutSurface.ulSurfaceHeight,
			hGrc->bSetEvent ? "wait" : "async"));
	}
	else if( pState->DstSurface.hSurface )
	{
		BDBG_MSG(("GRC Blit: Dst(o=%08x,f=%08x,r=%dx%d-%dx%d,s=%dx%d) -> Out(o=%08x,f=%08x,r=%dx%d-%dx%d,s=%dx%d) %s", 
			pState->DstSurface.ulOffset, pState->DstSurface.eFormat, pState->DstSurface.ulX, pState->DstSurface.ulY, pState->DstSurface.ulWidth, pState->DstSurface.ulHeight, pState->DstSurface.ulSurfaceWidth, pState->DstSurface.ulSurfaceHeight, 
			pState->OutSurface.ulOffset, pState->OutSurface.eFormat, pState->OutSurface.ulX, pState->OutSurface.ulY, pState->OutSurface.ulWidth, pState->OutSurface.ulHeight, pState->OutSurface.ulSurfaceWidth, pState->OutSurface.ulSurfaceHeight,
			hGrc->bSetEvent ? "wait" : "async"));
	}
	else
	{
		BDBG_MSG(("GRC Blit: Out(o=%08x,f=%08x,r=%dx%d-%dx%d,s=%dx%d) %s", 
			pState->OutSurface.ulOffset, pState->OutSurface.eFormat, pState->OutSurface.ulX, pState->OutSurface.ulY, pState->OutSurface.ulWidth, pState->OutSurface.ulHeight, pState->OutSurface.ulSurfaceWidth, pState->OutSurface.ulSurfaceHeight,
			hGrc->bSetEvent ? "wait" : "async"));
	}

	/* disable set event flag */
	hGrc->bSetEvent = false;
	hGrc->bPeriodicInterrupt = false;

	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_P_IssueStateAndWait(
	BGRC_Handle hGrc )
{
	uint32_t ulCurrentAddress = 0;
	uint32_t ulPreviousAddress = 0;
	BERR_Code err;

	/* enable set event flag */
	hGrc->bSetEvent = true;

	/* initiate device operation */
	err = BGRC_P_IssueState( hGrc, NULL, NULL );
	if( err != BERR_SUCCESS )
		return BERR_TRACE(err);

	/* wait for the device operation to finish */
	while( BKNI_WaitForEvent( hGrc->hInterruptEvent, hGrc->ulWaitTimeout * 1000 ) == BERR_TIMEOUT )
	{
		ulCurrentAddress = BGRC_P_READ_REG( BLIT_OUTPUT_ADDRESS );
		if( ulCurrentAddress == ulPreviousAddress )
		{
			BGRC_P_PrintRegisters( hGrc );
			return BERR_TRACE(BGRC_ERR_M2MC_DEVICE_IS_HUNG);
		}
		ulPreviousAddress = ulCurrentAddress;
	}

	return BERR_SUCCESS;
}

#ifdef BCHP_M2MC_DCE_PRED_CFG
/***************************************************************************/
void BGRC_P_EnableTestFeature1( 
	BGRC_Handle hGrc,
	BSUR_TestFeature1_Settings *pSettings )
{
	uint32_t ulReg = BREG_Read32( hGrc->hRegister, BCHP_M2MC_DCE_PRED_CFG ) & (~(
		BCHP_MASK(M2MC_DCE_PRED_CFG, ENABLE) | 
		BCHP_MASK(M2MC_DCE_PRED_CFG, CONVERT_RGB) | 
		BCHP_MASK(M2MC_DCE_PRED_CFG, PREDICTION_MODE) | 
		BCHP_MASK(M2MC_DCE_PRED_CFG, EDGE_PRED_ENA) | 
		BCHP_MASK(M2MC_DCE_PRED_CFG, LEFT_PRED_ENA) | 
		BCHP_MASK(M2MC_DCE_PRED_CFG, ABCD_PRED_ENA) | 
		BCHP_MASK(M2MC_DCE_PRED_CFG, LS_PRED_ENA)));

	BREG_Write32( hGrc->hRegister, BCHP_M2MC_DCE_PRED_CFG, ulReg |
		BCHP_FIELD_ENUM(M2MC_DCE_PRED_CFG, ENABLE, Enable) | 
		BCHP_FIELD_DATA(M2MC_DCE_PRED_CFG, PREDICTION_MODE, pSettings->ulPredictionMode) |
		BCHP_FIELD_ENUM(M2MC_DCE_PRED_CFG, CONVERT_RGB, Disable) | 
		BCHP_FIELD_ENUM(M2MC_DCE_PRED_CFG, EDGE_PRED_ENA, Disable) | 
		BCHP_FIELD_ENUM(M2MC_DCE_PRED_CFG, LEFT_PRED_ENA, Disable) | 
		BCHP_FIELD_ENUM(M2MC_DCE_PRED_CFG, ABCD_PRED_ENA, Disable) | 
		BCHP_FIELD_ENUM(M2MC_DCE_PRED_CFG, LS_PRED_ENA, Enable) );

	ulReg = BREG_Read32( hGrc->hRegister, BCHP_M2MC_DCE_COMPR_CFG1 ) & (~(
		BCHP_MASK(M2MC_DCE_COMPR_CFG1, PIXELS_PER_GROUP) | 
		BCHP_MASK(M2MC_DCE_COMPR_CFG1, TGT_BPG)));
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_DCE_COMPR_CFG1, ulReg |
		BCHP_FIELD_ENUM(M2MC_DCE_COMPR_CFG1, PIXELS_PER_GROUP, Four) | 
		BCHP_FIELD_DATA(M2MC_DCE_COMPR_CFG1, TGT_BPG, pSettings->ulBitsPerPixel * 2) );

	ulReg = BREG_Read32( hGrc->hRegister, BCHP_M2MC_DCE_VIDEO_CFG ) & (~(
		BCHP_MASK(M2MC_DCE_VIDEO_CFG, VIDEO_FORMAT) | 
		BCHP_MASK(M2MC_DCE_VIDEO_CFG, COMP10BIT)));
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_DCE_VIDEO_CFG, ulReg |
		BCHP_FIELD_ENUM(M2MC_DCE_VIDEO_CFG, VIDEO_FORMAT, Fmt_4444) | 
		BCHP_FIELD_ENUM(M2MC_DCE_VIDEO_CFG, COMP10BIT, Eight_bit) );

	BGRC_P_SET_FIELD_DATA( OUTPUT_SURFACE_FORMAT_DEF_1, FORMAT_TYPE, 9 );
}

/***************************************************************************/
void BGRC_P_DisableTestFeature1( 
	BGRC_Handle hGrc )
{
	uint32_t ulReg = BREG_Read32( hGrc->hRegister, BCHP_M2MC_DCE_PRED_CFG ) & (~BCHP_MASK(M2MC_DCE_PRED_CFG, ENABLE));
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_DCE_PRED_CFG, ulReg | BCHP_FIELD_ENUM(M2MC_DCE_PRED_CFG, ENABLE, Disable) );
}

/***************************************************************************/
BERR_Code BGRC_P_IssueStateTestFeature1(
	BGRC_Handle   hGrc,
	BGRC_Callback pCallback_isr,
	void         *pData )
{
	BERR_Code err = BERR_SUCCESS;

	BSUR_TestFeature1_Settings stTestFeature1Settings;
	BSUR_Surface_GetTestFeature1( hGrc->CurrentState.OutSurface.hSurface, &stTestFeature1Settings );

	BGRC_WaitForOperationsComplete( hGrc, NULL, NULL );
	BGRC_P_EnableTestFeature1( hGrc, &stTestFeature1Settings );

	err = BGRC_P_IssueStateAndWait( hGrc );

	BGRC_P_DisableTestFeature1( hGrc );
	BGRC_WaitForOperationsComplete( hGrc, pCallback_isr, pData );

	return BERR_TRACE(err);
}
#endif

/***************************************************************************/
BERR_Code BGRC_IssueState(
	BGRC_Handle   hGrc,
	BGRC_Callback pCallback_isr,
	void         *pData )
{
#ifdef BCHP_M2MC_DCE_PRED_CFG
	BSUR_TestFeature1_Settings stTestFeature1Settings;
#endif

	BDBG_ENTER(BGRC_IssueState);
	BDBG_ASSERT( hGrc );

	if( hGrc->CurrentState.OutSurface.hSurface == 0 )
		return BERR_TRACE(BGRC_ERR_NO_OUTPUT_SURFACE);

#ifdef BCHP_M2MC_DCE_PRED_CFG
	BSUR_Surface_GetTestFeature1( hGrc->CurrentState.OutSurface.hSurface, &stTestFeature1Settings );
	if( stTestFeature1Settings.bEnable )
	{
		BGRC_P_IssueStateTestFeature1( hGrc, pCallback_isr, pData );
	}
	else
#endif
	{
		BERR_Code err = BGRC_P_IssueState( hGrc, pCallback_isr, pData );
		if( err != BERR_SUCCESS )
		{
			BDBG_LEAVE(BGRC_IssueState);
			return BERR_TRACE(err);
		}
	}

	BDBG_LEAVE(BGRC_IssueState);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_IssueStateAndWait(
	BGRC_Handle hGrc )
{
#ifdef BCHP_M2MC_DCE_PRED_CFG
	BSUR_TestFeature1_Settings stTestFeature1Settings;
#endif

	BDBG_ENTER(BGRC_IssueStateAndWait);
	BDBG_ASSERT( hGrc );

	if( hGrc->CurrentState.OutSurface.hSurface == 0 )
		return BERR_TRACE(BGRC_ERR_NO_OUTPUT_SURFACE);

#ifdef BCHP_M2MC_DCE_PRED_CFG
	BSUR_Surface_GetTestFeature1( hGrc->CurrentState.OutSurface.hSurface, &stTestFeature1Settings );
	if( stTestFeature1Settings.bEnable )
	{
		BGRC_P_IssueStateTestFeature1( hGrc, NULL, NULL );
	}
	else
#endif
	{
		BERR_Code err = BGRC_P_IssueStateAndWait( hGrc );
		if( err != BERR_SUCCESS )
		{
			BDBG_LEAVE(BGRC_IssueStateAndWait);
			return BERR_TRACE(err);
		}
	}

	BDBG_LEAVE(BGRC_IssueStateAndWait);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_OperationsComplete(
	BGRC_Handle hGrc, 
	bool *pbComplete )
{
	uint32_t ulBlitStatus;
	uint32_t ulListStatus;

	BDBG_ENTER(BGRC_OperationsComplete);
	BDBG_ASSERT( hGrc );
	BDBG_ASSERT( pbComplete );

	/* check if device is idle */
	ulBlitStatus = BGRC_P_READ_REG( BLIT_STATUS );
	ulListStatus = BGRC_P_READ_REG( LIST_STATUS );

	if( (BCHP_GET_FIELD_DATA(ulListStatus, M2MC_LIST_STATUS, BUSY) == BCHP_M2MC_LIST_STATUS_BUSY_Busy) ||
		(BCHP_GET_FIELD_DATA(ulListStatus, M2MC_LIST_STATUS, FINISHED) == BCHP_M2MC_LIST_STATUS_FINISHED_NotFinished) ||
		(BCHP_GET_FIELD_DATA(ulBlitStatus, M2MC_BLIT_STATUS, STATUS) == BCHP_M2MC_BLIT_STATUS_STATUS_RUNNING) )
		*pbComplete = false;
	else
		*pbComplete = true;

	/* wait for dummy blit if device is idle to flush operation processing */
	if( *pbComplete )
	{
		BERR_Code err = BGRC_WaitForOperationsComplete( hGrc, 0, 0 );
		if( err != BERR_SUCCESS )
		{
			BDBG_ERR(( "BGRC_WaitForOperationsComplete FAILED IN BGRC_OperationsComplete" ));
			return BERR_TRACE(err);
		}
	}

	BDBG_LEAVE(BGRC_OperationsComplete);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_WaitForOperationReady(
	BGRC_Handle hGrc,
	BGRC_Callback pCallback_isr,
	void         *pData )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BGRC_WaitForOperationReady);
	BDBG_ASSERT( hGrc );

	if( pCallback_isr )
	{
		hGrc->pPeriodicData = pData;
		hGrc->pPeriodicCallback = pCallback_isr;

		if( hGrc->ulPeriodicInterrupts == 0 )
		{
			err = BGRC_WaitForOperationsComplete( hGrc, pCallback_isr, pData );
			if( err != BERR_SUCCESS )
			{
				BDBG_ERR(( "BGRC_WaitForOperationsComplete FAILED IN BGRC_WaitForOperationReady" ));
				return BERR_TRACE(err);
			}
			hGrc->pPeriodicData = NULL;
			hGrc->pPeriodicCallback = NULL;
		}
	}
	else
	{
		while( hGrc->pCurrListBlock->pNextBlock->ulRefCount )
		{
			BERR_Code err = BKNI_WaitForEvent( hGrc->hPeriodicEvent, hGrc->ulWaitTimeout * 1000 );
			if( err != BERR_SUCCESS )
			{
				BDBG_ERR(( "BKNI_WaitForEvent TIMED OUT IN BGRC_WaitForOperationReady." ));
				return BERR_TRACE(err);
			}
		
			BGRC_P_Operation_CleanupList( hGrc );
		}
	}

	BDBG_LEAVE(BGRC_WaitForOperationReady);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_WaitForOperationsComplete(
	BGRC_Handle hGrc,
	BGRC_Callback pCallback_isr,
	void *pData )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BGRC_WaitForOperationsComplete);
	BDBG_ASSERT( hGrc );

	/* Save current state */
	BGRC_P_Source_CopyState( &hGrc->StoredState, &hGrc->CurrentState, hGrc->aulStoredRegs, hGrc->aulCurrentRegs );
	BGRC_P_Destination_CopyState( &hGrc->StoredState, &hGrc->CurrentState, hGrc->aulStoredRegs, hGrc->aulCurrentRegs );
	BGRC_P_Output_CopyState( &hGrc->StoredState, &hGrc->CurrentState, hGrc->aulStoredRegs, hGrc->aulCurrentRegs );
	BGRC_P_Pattern_CopyState( &hGrc->StoredState, &hGrc->CurrentState, hGrc->aulStoredRegs, hGrc->aulCurrentRegs );
	BGRC_P_Blend_CopyState( &hGrc->StoredState, &hGrc->CurrentState, hGrc->aulStoredRegs, hGrc->aulCurrentRegs );

	/* Do fill and wait */
	err = BGRC_ResetState( hGrc );
	if( err != BERR_SUCCESS )
	{
		BDBG_LEAVE(BGRC_WaitForOperationsComplete);
		return BERR_TRACE(err);
	}

	BGRC_Output_SetSurface( hGrc, hGrc->hWaitSurface );

	if( pCallback_isr )
		err = BGRC_P_IssueState( hGrc, pCallback_isr, pData );
	else
		err = BGRC_P_IssueStateAndWait( hGrc );

	/* Restore current state */
	BGRC_P_Source_CopyState( &hGrc->CurrentState, &hGrc->StoredState, hGrc->aulCurrentRegs, hGrc->aulStoredRegs );
	BGRC_P_Destination_CopyState( &hGrc->CurrentState, &hGrc->StoredState, hGrc->aulCurrentRegs, hGrc->aulStoredRegs );
	BGRC_P_Output_CopyState( &hGrc->CurrentState, &hGrc->StoredState, hGrc->aulCurrentRegs, hGrc->aulStoredRegs );
	BGRC_P_Pattern_CopyState( &hGrc->CurrentState, &hGrc->StoredState, hGrc->aulCurrentRegs, hGrc->aulStoredRegs );
	BGRC_P_Blend_CopyState( &hGrc->CurrentState, &hGrc->StoredState, hGrc->aulCurrentRegs, hGrc->aulStoredRegs );

	BDBG_LEAVE(BGRC_WaitForOperationsComplete);
	if( err != BERR_SUCCESS )
		return BERR_TRACE(err);
	else
		return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetSurface(
	BGRC_Handle hGrc,
	BSUR_Surface_Handle hSurface )
{
	BGRC_P_State *pState;
	BGRC_P_Surface Surface;
	bool bSurfaceChanged = false;

	BDBG_ENTER(BGRC_Source_SetSurface);
	BDBG_ASSERT( hGrc );

	pState = &hGrc->CurrentState;

	/* get surface data */
	BGRC_P_GET_SURFACE_DATA( hSurface, Surface, pState->SrcSurface );

	/* check if surface is changing */
	if( hGrc->bUninitialized || bSurfaceChanged )
	{
		/* validate surface format */
		if( hSurface && BPXL_IS_DEPTH_FORMAT(Surface.eFormat) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

#if (BCHP_CHIP!=3548) && (BCHP_CHIP!=3556) && (BCHP_CHIP!=3563) && (BCHP_CHIP!=35130) && (BCHP_CHIP!=35230)
		if( hSurface && (BPXL_IS_YCbCr444_10BIT_FORMAT(Surface.eFormat) || BPXL_IS_YCbCr422_10BIT_FORMAT(Surface.eFormat)) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);
#endif

		/* set register load field */
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );

		/* set surface fields */
		BGRC_P_SET_FIELD_COMP( SRC_FEEDER_ENABLE, ENABLE, ENABLE, DISABLE, hSurface );
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_ADDR_0, ADDR, Surface.ulOffset );
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_STRIDE_0, STRIDE, Surface.ulPitch );
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_ADDR_1, ADDR, 0);
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_STRIDE_1, STRIDE, 0);
		BGRC_P_SET_FIELD_FORMAT( SRC, FORMAT_TYPE, Surface.eFormat, pState->SrcAlphaSurface.hSurface, pState->bSrcPaletteBypass );
		BGRC_P_SET_FIELD_CHANNELS( SRC, Surface.eFormat, hSurface );

		BGRC_P_SET_FIELD_COMP_DATA( SRC_SURFACE_FORMAT_DEF_3, CH0_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 0) );
		BGRC_P_SET_FIELD_COMP_DATA( SRC_SURFACE_FORMAT_DEF_3, CH1_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 1) );
		BGRC_P_SET_FIELD_COMP_DATA( SRC_SURFACE_FORMAT_DEF_3, CH2_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 2) );
		BGRC_P_SET_FIELD_COMP_DATA( SRC_SURFACE_FORMAT_DEF_3, CH3_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 3) );

#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_0)
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_STRIPE_HEIGHT_WIDTH_0, STRIPE_HEIGHT, 0 );
#endif
#if defined(BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_STRIPED_IMAGE_FORMAT_MASK)
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_0_FORMAT_DEF_1, STRIPED_IMAGE_FORMAT, 0 );
#endif
#if defined(BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_RANGE_EXP_MAP_SCALE_FACTOR_Y_MASK)
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_0_FORMAT_DEF_3, RANGE_EXP_MAP_SCALE_FACTOR_Y, BGRC_P_MACROBLOCK_RANGE_NONE );
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_0_FORMAT_DEF_3, RANGE_EXP_MAP_SCALE_FACTOR_C, BGRC_P_MACROBLOCK_RANGE_NONE );
#endif

		/* set registers for YCbCr 420 format */
		if( (Surface.eFormat == BPXL_eY8) || (Surface.eFormat == BPXL_eA8_Y8) )
		{
#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_0)
			BGRC_P_SET_FIELD_DATA( BLIT_SRC_STRIPE_HEIGHT_WIDTH_0, STRIPE_HEIGHT, Surface.ulSurfaceHeight );
#endif
#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_0_STRIPE_WIDTH_MASK)
			BGRC_P_SET_FIELD_DATA( BLIT_SRC_STRIPE_HEIGHT_WIDTH_0, STRIPE_WIDTH, 
				(pState->ulMacroBlockStripWidth == BGRC_P_YCbCr420_STRIP_WIDTH) ? 0 : 1 );
#endif

#if defined(BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_STRIPED_IMAGE_FORMAT_MASK)
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_0_FORMAT_DEF_1, STRIPED_IMAGE_FORMAT, pState->bMacroBlockLinear ? 0 : 1 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_0_FORMAT_DEF_1, FORMAT_TYPE, pState->bMacroBlockBigEndian ? 0 : 7 );
#else
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_FORMAT_DEF_1, FORMAT_TYPE, pState->bMacroBlockBigEndian ? 0 : (pState->bMacroBlockLinear ? 7 : 15) );
#endif
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_FORMAT_DEF_3, CH0_DISABLE, 1 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_FORMAT_DEF_3, CH1_DISABLE, 1 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_FORMAT_DEF_3, CH2_DISABLE, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_FORMAT_DEF_3, CH3_DISABLE, (Surface.eFormat == BPXL_eA8_Y8) ? 0 : 1 );

#if defined(BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_RANGE_EXP_MAP_SCALE_FACTOR_Y_MASK)
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_0_FORMAT_DEF_3, RANGE_EXP_MAP_SCALE_FACTOR_Y, pState->ulMacroBlockRangeY );
#endif
		}

#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_0)
		if( BGRC_P_REGISTER_CHANGED( BLIT_SRC_STRIPE_HEIGHT_WIDTH_0 ) )
			BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
#endif

		/* store surface data */
		BKNI_Memcpy( &pState->SrcSurface, &Surface, sizeof (BGRC_P_Surface) );
	}

	BDBG_LEAVE(BGRC_Source_SetSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetAlphaSurface(
	BGRC_Handle hGrc,
	BSUR_Surface_Handle hSurface )
{
	BGRC_P_State *pState;
	BGRC_P_Surface Surface;
	bool bSurfaceChanged = false;

	BDBG_ENTER(BGRC_Source_SetAlphaSurface);
	BDBG_ASSERT( hGrc );

	pState = &hGrc->CurrentState;

	/* get surface data */
	BGRC_P_GET_SURFACE_DATA( hSurface, Surface, pState->SrcAlphaSurface );

	/* check if surface is changing */
	if( hGrc->bUninitialized || bSurfaceChanged )
	{
		/* validate surface format */
		if( hSurface && BPXL_IS_DEPTH_FORMAT(Surface.eFormat) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		/* set register load field */
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );

		/* set surface fields */
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_ADDR_1, ADDR, Surface.ulOffset );
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_STRIDE_1, STRIDE, Surface.ulPitch );

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
		BGRC_P_SET_FIELD_COMP_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH0_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 0) );
		BGRC_P_SET_FIELD_COMP_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH1_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 1) );
		BGRC_P_SET_FIELD_COMP_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH2_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 2) );
		BGRC_P_SET_FIELD_COMP_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH3_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 3) );
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, FORMAT_TYPE, 0 );

#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_1)
		BGRC_P_SET_FIELD_DATA( BLIT_SRC_STRIPE_HEIGHT_WIDTH_1, STRIPE_HEIGHT, 0 );
#endif
#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1_STRIPED_IMAGE_FORMAT_MASK)
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, STRIPED_IMAGE_FORMAT, 0 );
#endif
#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_3_RANGE_EXP_MAP_SCALE_FACTOR_Y_MASK)
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, RANGE_EXP_MAP_SCALE_FACTOR_Y, BGRC_P_MACROBLOCK_RANGE_NONE );
		BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, RANGE_EXP_MAP_SCALE_FACTOR_C, BGRC_P_MACROBLOCK_RANGE_NONE );
#endif

		/* set registers for YCbCr 420 format */
		if( (Surface.eFormat == BPXL_eCb8_Cr8) || (Surface.eFormat == BPXL_eCr8_Cb8) )
		{
#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_1)
			BGRC_P_SET_FIELD_DATA( BLIT_SRC_STRIPE_HEIGHT_WIDTH_1, STRIPE_HEIGHT, Surface.ulSurfaceHeight );
#endif
#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_1_STRIPE_WIDTH_MASK)
			BGRC_P_SET_FIELD_DATA( BLIT_SRC_STRIPE_HEIGHT_WIDTH_1, STRIPE_WIDTH, 
				(pState->ulMacroBlockStripWidth == BGRC_P_YCbCr420_STRIP_WIDTH) ? 0 : 1 );
#endif

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1_STRIPED_IMAGE_FORMAT_MASK)
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, STRIPED_IMAGE_FORMAT, pState->bMacroBlockLinear ? 0 : 1 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, FORMAT_TYPE, pState->bMacroBlockBigEndian ? 0 : 7 );
#else
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, FORMAT_TYPE, pState->bMacroBlockBigEndian ? 0 : (pState->bMacroBlockLinear ? 7 : 15) );
#endif
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, CH0_NUM_BITS, BPXL_COMPONENT_SIZE(Surface.eFormat, 0) );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, CH1_NUM_BITS, BPXL_COMPONENT_SIZE(Surface.eFormat, 1) );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, CH2_NUM_BITS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, CH3_NUM_BITS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_2, CH0_LSB_POS, BPXL_COMPONENT_POS(Surface.eFormat, 0) );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_2, CH1_LSB_POS, BPXL_COMPONENT_POS(Surface.eFormat, 1) );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_2, CH2_LSB_POS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_2, CH3_LSB_POS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH0_DISABLE, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH1_DISABLE, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH2_DISABLE, 1 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH3_DISABLE, 1 );

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_3_RANGE_EXP_MAP_SCALE_FACTOR_C_MASK)
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, RANGE_EXP_MAP_SCALE_FACTOR_C, pState->ulMacroBlockRangeC );
#endif
		}
		else
		{
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, CH0_NUM_BITS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, CH1_NUM_BITS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, CH2_NUM_BITS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, CH3_NUM_BITS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_2, CH0_LSB_POS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_2, CH1_LSB_POS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_2, CH2_LSB_POS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_2, CH3_LSB_POS, 0 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH0_DISABLE, 1 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH1_DISABLE, 1 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH2_DISABLE, 1 );
			BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, CH3_DISABLE, 1 );
		}

		if( BGRC_P_REGISTER_CHANGED( BLIT_SRC_STRIPE_HEIGHT_WIDTH_1 ) )
			BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );
#endif

#if 0
		/* set surface format field */
		if( pState->SrcSurface.hSurface )
			BGRC_P_SET_FIELD_FORMAT( SRC, FORMAT_TYPE, pState->SrcSurface.eFormat, hSurface, pState->bSrcPaletteBypass );
#endif

		/* store surface data */
		BKNI_Memcpy( &pState->SrcAlphaSurface, &Surface, sizeof (BGRC_P_Surface) );
	}

	BDBG_LEAVE(BGRC_Source_SetAlphaSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_TogglePaletteBypass(
	BGRC_Handle hGrc,
	bool bEnableBypass )
{
	BGRC_P_State *pState;

	BDBG_ENTER(BGRC_Source_TogglePaletteBypass);
	BDBG_ASSERT( hGrc );

	pState = &hGrc->CurrentState;

	/* set palette bypass field */
	BGRC_P_SET_FIELD_COMP( SRC_SURFACE_FORMAT_DEF_3, PALETTE_BYPASS, DONT_LOOKUP, LOOKUP, bEnableBypass );

	/* set format type due to bypass */
	if( pState->SrcSurface.hSurface )
		BGRC_P_SET_FIELD_FORMAT( SRC, FORMAT_TYPE, pState->SrcSurface.eFormat, pState->SrcAlphaSurface.hSurface, bEnableBypass );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( SRC_SURFACE_FORMAT_DEF_1 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_SURFACE_FORMAT_DEF_3 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );

	pState->bSrcPaletteBypass = bEnableBypass;

	BDBG_LEAVE(BGRC_Source_TogglePaletteBypass);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetChromaExpansion(
	BGRC_Handle hGrc,
	BGRC_ChromaExpansion eChromaExpansion )
{
	BDBG_ENTER(BGRC_Source_SetChromaExpansion);
	BDBG_ASSERT( hGrc );

	/* set chroma expansion field */
	BGRC_P_SET_FIELD_COMP( SRC_SURFACE_FORMAT_DEF_3, CHROMA_FILTER, 
		REPLICATE, FILTER, eChromaExpansion == BGRC_ChromaExpansion_eReplicate );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( SRC_SURFACE_FORMAT_DEF_3 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_FEEDER_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetChromaExpansion);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetZeroPad(
	BGRC_Handle hGrc,
	bool bEnableZeroPad )
{
	BDBG_ENTER(BGRC_Source_SetZeroPad);
	BDBG_ASSERT( hGrc );

	/* set zero pad field */
	BGRC_P_SET_FIELD_COMP( SRC_SURFACE_FORMAT_DEF_3, ZERO_PAD, 
		ZERO_PAD, REPLICATE, bEnableZeroPad );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( SRC_SURFACE_FORMAT_DEF_3 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_FEEDER_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetZeroPad);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetColorKey(
	BGRC_Handle hGrc,
	uint32_t ulMin,
	uint32_t ulMax,
	uint32_t ulMask,
	uint32_t ulReplacement,
	uint32_t ulReplacementMask,
	bool bExclusive )
{
	BDBG_ENTER(BGRC_Source_SetColorKey);
	BDBG_ASSERT( hGrc );

	/* set color key fields */
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, SRC_COLOR_KEY_COMPARE, EXCUSIVE, INCLUSIVE, bExclusive );
	BGRC_P_SET_FIELD_FULL( SRC_COLOR_KEY_LOW, ulMin );
	BGRC_P_SET_FIELD_FULL( SRC_COLOR_KEY_HIGH, ulMax );
	BGRC_P_SET_FIELD_FULL( SRC_COLOR_KEY_MASK, ulMask );
	BGRC_P_SET_FIELD_FULL( SRC_COLOR_KEY_REPLACEMENT, ulReplacement );
	BGRC_P_SET_FIELD_FULL( SRC_COLOR_KEY_REPLACEMENT_MASK, ulReplacementMask );

	/* set register load fields */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	if( BGRC_P_REGISTER_CHANGED( SRC_COLOR_KEY_LOW ) ||
		BGRC_P_REGISTER_CHANGED( SRC_COLOR_KEY_HIGH ) ||
		BGRC_P_REGISTER_CHANGED( SRC_COLOR_KEY_MASK ) ||
		BGRC_P_REGISTER_CHANGED( SRC_COLOR_KEY_REPLACEMENT ) ||
		BGRC_P_REGISTER_CHANGED( SRC_COLOR_KEY_REPLACEMENT_MASK ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_COLOR_KEY_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetColorKey);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_ToggleColorKey(
	BGRC_Handle hGrc,
	bool bEnable )
{
	BDBG_ENTER(BGRC_Source_ToggleColorKey);
	BDBG_ASSERT( hGrc );

	/* set color key enable field */
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, SRC_COLOR_KEY_ENABLE, 
		ENABLE, DISABLE, bEnable );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_ToggleColorKey);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetColorMatrix5x4(
	BGRC_Handle hGrc,
	const int32_t ai32_Matrix[],
	uint32_t ulShift )
{
	BDBG_ENTER(BGRC_Source_SetColorMatrix5x4);
	BDBG_ASSERT( hGrc );

	/* set color matrix fields */
	BGRC_P_SET_FIELD_MATRIX_ROW( SRC, 0, ai32_Matrix,  0, ulShift );
	BGRC_P_SET_FIELD_MATRIX_ROW( SRC, 1, ai32_Matrix,  5, ulShift );
	BGRC_P_SET_FIELD_MATRIX_ROW( SRC, 2, ai32_Matrix, 10, ulShift );
	BGRC_P_SET_FIELD_MATRIX_ROW( SRC, 3, ai32_Matrix, 15, ulShift );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( SRC_CM_C00_C01 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_CM_C02_C03 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_CM_C04 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_CM_C10_C11 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_CM_C12_C13 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_CM_C14 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_CM_C20_C21 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_CM_C22_C23 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_CM_C24 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_CM_C30_C31 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_CM_C32_C33 ) ||
		BGRC_P_REGISTER_CHANGED( SRC_CM_C34 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_COLOR_MATRIX_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetColorMatrix5x4);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_ToggleColorMatrix(
	BGRC_Handle hGrc,
	bool bEnable )
{
	BDBG_ENTER(BGRC_Source_ToggleColorMatrix);
	BDBG_ASSERT( hGrc );

	/* set color matrix enable field */
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, SRC_COLOR_MATRIX_ENABLE, 
		ENABLE, DISABLE, bEnable );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_ToggleColorMatrix);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetColorMatrixRounding(
	BGRC_Handle hGrc,
	BGRC_Rounding eMatrixRounding )
{
	BDBG_ENTER(BGRC_Source_SetColorMatrixRounding);
	BDBG_ASSERT( hGrc );

	/* set matrix rounding field */
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, SRC_COLOR_MATRIX_ROUNDING, 
		TRUNCATE, NEAREST, eMatrixRounding == BGRC_Rounding_eTruncate );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetColorMatrixRounding);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetKeyMatrixOrder(
	BGRC_Handle hGrc,
	BGRC_KeyMatrixOrder eKeyMatrixOrder )
{
	BDBG_ENTER(BGRC_Source_SetKeyMatrixOrder);
	BDBG_ASSERT( hGrc );

	/* set colorkey/matrix order field */
#if defined(BCHP_M2MC_BLIT_HEADER_CBAR_SRC_COLOR_KEY_THEN_MATRIX)
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, CBAR_SRC_COLOR, KEY_THEN_MATRIX, 
		MATRIX_THEN_KEY, eKeyMatrixOrder == BGRC_KeyMatrixOrder_eKeyThenMatrix );
#else
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, CBAR_SRC_COLOR, SCALE_THEN_KEY_THEN_MATRIX, 
		SCALE_THEN_MATRIX_THEN_KEY, eKeyMatrixOrder == BGRC_KeyMatrixOrder_eKeyThenMatrix );
#endif

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetKeyMatrixOrder);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetKeyMatrixScaleOrder(
	BGRC_Handle hGrc,
	BGRC_KeyMatrixScaleOrder eKeyMatrixScaleOrder )
{
	BDBG_ENTER(BGRC_Source_SetKeyMatrixScaleOrder);
	BDBG_ASSERT( hGrc );

	/* set colorkey/matrix order field */
#if defined(BCHP_M2MC_BLIT_HEADER_CBAR_SRC_COLOR_KEY_THEN_MATRIX)
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, CBAR_SRC_COLOR, KEY_THEN_MATRIX, MATRIX_THEN_KEY, 
		(eKeyMatrixScaleOrder == BGRC_KeyMatrixScaleOrder_eScaleThenKeyThenMatrix) ||
		(eKeyMatrixScaleOrder == BGRC_KeyMatrixScaleOrder_eKeyThenMatrixThenScale) ||
		(eKeyMatrixScaleOrder == BGRC_KeyMatrixScaleOrder_eKeyThenScaleThenMatrix) );
#else
	BGRC_P_SET_FIELD_DATA( BLIT_HEADER, CBAR_SRC_COLOR, eKeyMatrixScaleOrder );
#endif

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetKeyMatrixScaleOrder);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetColor(
	BGRC_Handle hGrc,
	uint32_t ulColor )
{
	BDBG_ENTER(BGRC_Source_SetColor);
	BDBG_ASSERT( hGrc );

	/* set single color field */
	BGRC_P_SET_FIELD_FULL( SRC_CONSTANT_COLOR, ulColor );
	BGRC_P_SET_FIELD_DATA( SRC_W_ALPHA, W0_ALPHA, BPXL_GET_COMPONENT( BPXL_eA8_R8_G8_B8, ulColor, 3 ) );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( SRC_CONSTANT_COLOR ) ||
		BGRC_P_REGISTER_CHANGED( SRC_W_ALPHA ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SRC_FEEDER_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetColor);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetRectangle(
	BGRC_Handle hGrc,
	uint32_t ulLeft,
	uint32_t ulTop,
	uint32_t ulWidth,
	uint32_t ulHeight )
{
	BDBG_ENTER(BGRC_Source_SetRectangle);
	BDBG_ASSERT( hGrc );

	/* validate dimensions */
	if( !BGRC_P_VALIDATE_SURFACE_DIMENSIONS( ulLeft, ulTop, ulWidth, ulHeight ) )
		return BERR_TRACE(BGRC_ERR_SOURCE_DIMENSIONS_INVALID);

	if( (ulLeft > BGRC_P_SURFACE_RECT_SIZE_MAX) || (ulTop > BGRC_P_SURFACE_RECT_SIZE_MAX) ||
		(ulWidth > BGRC_P_SURFACE_RECT_SIZE_MAX) || (ulHeight > BGRC_P_SURFACE_RECT_SIZE_MAX) )
		return BERR_TRACE(BGRC_ERR_SOURCE_DIMENSIONS_INVALID);

	/* store width and height */
	hGrc->CurrentState.SrcRect.ulX = ulLeft;
	hGrc->CurrentState.SrcRect.ulY = ulTop;
	hGrc->CurrentState.SrcRect.ulWidth = ulWidth;
	hGrc->CurrentState.SrcRect.ulHeight = ulHeight;

	BDBG_LEAVE(BGRC_Source_SetRectangle);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetFilterCoeffs(
	BGRC_Handle hGrc,
	BGRC_FilterCoeffs eHorizontalCoeffs,
	BGRC_FilterCoeffs eVerticalCoeffs )
{
	BDBG_ENTER(BGRC_Source_SetFilterCoeffs);
	BDBG_ASSERT( hGrc );

	/* set filter coefficients */
	hGrc->CurrentState.eHorzCoeffs = eHorizontalCoeffs;
	hGrc->CurrentState.eVertCoeffs = eVerticalCoeffs;

	BDBG_LEAVE(BGRC_Source_SetFilterCoeffs);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_ToggleFilter(
	BGRC_Handle hGrc,
	bool bEnableHorizontalFilter,
	bool bEnableVerticalFilter )
{
	BDBG_ENTER(BGRC_Source_ToggleFilter);
	BDBG_ASSERT( hGrc );

	/* set filter enables */
	hGrc->CurrentState.bHorzFilter = bEnableHorizontalFilter;
	hGrc->CurrentState.bVertFilter = bEnableVerticalFilter;

	BDBG_LEAVE(BGRC_Source_ToggleFilter);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetFilterPhaseAdjustment(
	BGRC_Handle hGrc,
	int32_t iHorizontalPhase,
	int32_t iVerticalPhase,
	uint32_t ulShift )
{
	BDBG_ENTER(BGRC_Source_SetFilterPhaseAdjustment);
	BDBG_ASSERT( hGrc );

	/* set filter phase */
	hGrc->CurrentState.iHorzPhaseAdj = iHorizontalPhase;
	hGrc->CurrentState.iVertPhaseAdj = iVerticalPhase;
	hGrc->CurrentState.ulPhaseShift = ulShift;

	BDBG_LEAVE(BGRC_Source_SetFilterPhaseAdjustment);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetFixedScaleFactor(
	BGRC_Handle hGrc,                  
	uint32_t ulHorizontalNumerator,     
	uint32_t ulHorizontalDenominator,   
	uint32_t ulVerticalNumerator,       
	uint32_t ulVerticalDenominator )
{
	BDBG_ENTER(BGRC_Source_SetFixedScaleFactor);
	BDBG_ASSERT( hGrc );

	/* set scale factor */
	hGrc->CurrentState.ulHorzScalerNum = ulHorizontalNumerator;
	hGrc->CurrentState.ulHorzScalerDen = ulHorizontalDenominator;
	hGrc->CurrentState.ulVertScalerNum = ulVerticalNumerator;
	hGrc->CurrentState.ulVertScalerDen = ulVerticalDenominator;

	BDBG_LEAVE(BGRC_Source_SetFixedScaleFactor);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetScaleAlphaPreMultiply(
	BGRC_Handle hGrc,
	bool bAlphaPreMultiply )
{
	BDBG_ENTER(BGRC_Source_SetScaleAlphaPreMultiply);
	BDBG_ASSERT( hGrc );

#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_PRE_MULTIPLY_MASK)
	BGRC_P_SET_FIELD_COMP( SCALER_CTRL, ALPHA_PRE_MULTIPLY, ENABLE, DISABLE, bAlphaPreMultiply );

	if( BGRC_P_REGISTER_CHANGED( SCALER_CTRL ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetScaleAlphaPreMultiply);
	return BERR_SUCCESS;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( bAlphaPreMultiply );
	BDBG_LEAVE(BGRC_Source_SetScaleAlphaPreMultiply);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/***************************************************************************/
BERR_Code BGRC_Source_SetScaleAlphaAdjust(
	BGRC_Handle hGrc,
	bool bAdjustAlpha )
{
	BDBG_ENTER(BGRC_Source_SetScaleAlphaAdjust);
	BDBG_ASSERT( hGrc );

#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_ADJUST_ENABLE)
	BGRC_P_SET_FIELD_COMP( SCALER_CTRL, ALPHA_ADJUST, ENABLE, DISABLE, bAdjustAlpha );
#else
	BGRC_P_SET_FIELD_COMP( SCALER_CTRL, OFFSET_ADJUST, ENABLE, DISABLE, bAdjustAlpha );
#endif

	if( BGRC_P_REGISTER_CHANGED( SCALER_CTRL ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetScaleAlphaAdjust);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetScaleRounding(
	BGRC_Handle hGrc,
	BGRC_Rounding eScaleRounding )
{
	BDBG_ENTER(BGRC_Source_SetScaleRounding);
	BDBG_ASSERT( hGrc );

	/* set scale rounding field */
	BGRC_P_SET_FIELD_COMP( SCALER_CTRL, ROUNDING_MODE, 
		TRUNCATE, NEAREST, eScaleRounding == BGRC_Rounding_eTruncate );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( SCALER_CTRL ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetScaleRounding);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetScaleEdgeCondition(
	BGRC_Handle hGrc,
	BGRC_EdgeCondition eEdgeCondition )
{
	BDBG_ENTER(BGRC_Source_SetScaleEdgeCondition);
	BDBG_ASSERT( hGrc );

#if defined(BCHP_M2MC_SCALER_CTRL_EDGE_CONDITION_MASK)
	BGRC_P_SET_FIELD_COMP( SCALER_CTRL, EDGE_CONDITION, REPLICATE, MIRROR, eEdgeCondition == BGRC_EdgeCondition_eReplicateLast );

	if( BGRC_P_REGISTER_CHANGED( SCALER_CTRL ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetScaleEdgeCondition);
	return BERR_SUCCESS;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( eEdgeCondition );
	BDBG_LEAVE(BGRC_Source_SetScaleEdgeCondition);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/***************************************************************************/
BERR_Code BGRC_Source_SetAlphaPreMultiply(
	BGRC_Handle hGrc,
	bool bAlphaPreMultiply )
{
	BDBG_ENTER(BGRC_Source_SetAlphaPreMultiply);
	BDBG_ASSERT( hGrc );

#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_PRE_MULTIPLY_ENABLE_MASK)
	BGRC_P_SET_FIELD_COMP( SCALER_CTRL, ALPHA_PRE_MULTIPLY_ENABLE, ENABLE, DISABLE, bAlphaPreMultiply );

	if( BGRC_P_REGISTER_CHANGED( SCALER_CTRL ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetAlphaPreMultiply);
	return BERR_SUCCESS;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( bAlphaPreMultiply );

	BDBG_LEAVE(BGRC_Source_SetAlphaPreMultiply);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/***************************************************************************/
BERR_Code BGRC_Source_SetAlphaPreMultiplyOffset(
	BGRC_Handle hGrc,
	bool bOffset )
{
	BDBG_ENTER(BGRC_Source_SetAlphaPreMultiplyOffset);
	BDBG_ASSERT( hGrc );

#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_PRE_MULTIPLY_OFFSET_EN_MASK)
	BGRC_P_SET_FIELD_COMP( SCALER_CTRL, ALPHA_PRE_MULTIPLY_OFFSET_EN, ENABLE, DISABLE, bOffset );

	if( BGRC_P_REGISTER_CHANGED( SCALER_CTRL ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetAlphaPreMultiplyOffset);
	return BERR_SUCCESS;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( bOffset );
	BDBG_LEAVE(BGRC_Source_SetAlphaPreMultiplyOffset);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/***************************************************************************/
BERR_Code BGRC_Source_SetMacroBlock(
	BGRC_Handle hGrc,
	BGRC_MacroBlockRange eRangeY,
	BGRC_MacroBlockRange eRangeC,
	uint8_t ucScaleFactorY,
	uint8_t ucScaleFactorC )
{
	BDBG_ENTER(BGRC_Source_SetMacroBlock);
	BDBG_ASSERT( hGrc );

#if defined(BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_RANGE_EXP_MAP_SCALE_FACTOR_Y_MASK)
	if( eRangeY == BGRC_MacroBlockRange_None )
		hGrc->CurrentState.ulMacroBlockRangeY = BGRC_P_MACROBLOCK_RANGE_NONE;
	else if( eRangeY == BGRC_MacroBlockRange_Expansion )
		hGrc->CurrentState.ulMacroBlockRangeY = BGRC_P_MACROBLOCK_RANGE_EXPANSION;
	else if( eRangeY == BGRC_MacroBlockRange_Remapping )
		hGrc->CurrentState.ulMacroBlockRangeY = BGRC_P_MACROBLOCK_RANGE_REMAPPING + ucScaleFactorY;

	BGRC_P_SET_FIELD_DATA( SRC_SURFACE_0_FORMAT_DEF_3, RANGE_EXP_MAP_SCALE_FACTOR_Y, hGrc->CurrentState.ulMacroBlockRangeY );
	BGRC_P_SET_FIELD_DATA( SRC_SURFACE_0_FORMAT_DEF_3, RANGE_EXP_MAP_SCALE_FACTOR_C, BGRC_P_MACROBLOCK_RANGE_NONE );

	if( eRangeC == BGRC_MacroBlockRange_None )
		hGrc->CurrentState.ulMacroBlockRangeC = BGRC_P_MACROBLOCK_RANGE_NONE;
	else if( eRangeC == BGRC_MacroBlockRange_Expansion )
		hGrc->CurrentState.ulMacroBlockRangeC = BGRC_P_MACROBLOCK_RANGE_EXPANSION;
	else if( eRangeC == BGRC_MacroBlockRange_Remapping )
		hGrc->CurrentState.ulMacroBlockRangeC = BGRC_P_MACROBLOCK_RANGE_REMAPPING + ucScaleFactorC;

	BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, RANGE_EXP_MAP_SCALE_FACTOR_Y, BGRC_P_MACROBLOCK_RANGE_NONE );
	BGRC_P_SET_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_3, RANGE_EXP_MAP_SCALE_FACTOR_C, hGrc->CurrentState.ulMacroBlockRangeC );
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( eRangeY );
	BSTD_UNUSED( eRangeC );
	BSTD_UNUSED( ucScaleFactorY );
	BSTD_UNUSED( ucScaleFactorC );
	BDBG_LEAVE(BGRC_Source_SetMacroBlock);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif

	BDBG_LEAVE(BGRC_Source_SetMacroBlock);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetMacroBlock_StripWidth(
	BGRC_Handle hGrc,
	uint32_t ulStripWidth )
{
	BDBG_ENTER(BGRC_Source_SetMacroBlock_StripWidth);
	BDBG_ASSERT( hGrc );

#if defined(BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_RANGE_EXP_MAP_SCALE_FACTOR_Y_MASK)

#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_0_STRIPE_WIDTH_MASK)
	if( (ulStripWidth != BGRC_P_YCbCr420_STRIP_WIDTH) && (ulStripWidth != BGRC_P_YCbCr420_STRIP_WIDTH * 2) )
		return BERR_TRACE(BERR_INVALID_PARAMETER);
#else
	if( ulStripWidth != BGRC_P_YCbCr420_STRIP_WIDTH )
		return BERR_TRACE(BERR_INVALID_PARAMETER);
#endif

	hGrc->CurrentState.ulMacroBlockStripWidth = ulStripWidth;

#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_0_STRIPE_WIDTH_MASK)
	BGRC_P_SET_FIELD_DATA( BLIT_SRC_STRIPE_HEIGHT_WIDTH_0, STRIPE_WIDTH, 
		(hGrc->CurrentState.ulMacroBlockStripWidth == BGRC_P_YCbCr420_STRIP_WIDTH) ? 0 : 1 );
	BGRC_P_SET_FIELD_DATA( BLIT_SRC_STRIPE_HEIGHT_WIDTH_1, STRIPE_WIDTH, 
		(hGrc->CurrentState.ulMacroBlockStripWidth == BGRC_P_YCbCr420_STRIP_WIDTH) ? 0 : 1 );

	if( BGRC_P_REGISTER_CHANGED( BLIT_SRC_STRIPE_HEIGHT_WIDTH_0 ) ||
		BGRC_P_REGISTER_CHANGED( BLIT_SRC_STRIPE_HEIGHT_WIDTH_1 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, SCALE_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );
#endif

#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( ulStripWidth );
	BDBG_LEAVE(BGRC_Source_SetMacroBlock_StripWidth);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif

	BDBG_LEAVE(BGRC_Source_SetMacroBlock_StripWidth);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetMacroBlock_LinearFormat(
	BGRC_Handle hGrc,
	bool bLinearFormat )
{
	BDBG_ENTER(BGRC_Source_SetMacroBlock_LinearFormat);
	BDBG_ASSERT( hGrc );

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	hGrc->CurrentState.bMacroBlockLinear = bLinearFormat;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( bLinearFormat );
	BDBG_LEAVE(BGRC_Source_SetMacroBlock_LinearFormat);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif

	BDBG_LEAVE(BGRC_Source_SetMacroBlock_LinearFormat);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetMacroBlock_Endian(
	BGRC_Handle hGrc,
	bool bBigEndian )
{
	BDBG_ENTER(BGRC_Source_SetMacroBlock_Endian);
	BDBG_ASSERT( hGrc );

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	hGrc->CurrentState.bMacroBlockBigEndian = bBigEndian;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( bBigEndian );
	BDBG_LEAVE(BGRC_Source_SetMacroBlock_Endian);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif

	BDBG_LEAVE(BGRC_Source_SetMacroBlock_Endian);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetDirection(
	BGRC_Handle hGrc,
	bool bRightToLeft,
	bool bBottomToTop )
{
	BDBG_ENTER(BGRC_Source_SetDirection);
	BDBG_ASSERT( hGrc );

	hGrc->CurrentState.bSrcRightToLeft = bRightToLeft;
	hGrc->CurrentState.bSrcBottomToTop = bBottomToTop;

	BDBG_LEAVE(BGRC_Source_SetDirection);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_ResetState(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_Source_ResetState);
	BDBG_ASSERT( hGrc );

	/* copy default state to current state */
	BGRC_P_Source_CopyState( &hGrc->CurrentState, &hGrc->DefaultState, 
		hGrc->aulCurrentRegs, hGrc->aulDefaultRegs );

	BDBG_LEAVE(BGRC_Source_ResetState);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Source_SetDefault(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_Source_SetDefault);
	BDBG_ASSERT( hGrc );

	/* copy current state to default state */
	BGRC_P_Source_CopyState( &hGrc->DefaultState, &hGrc->CurrentState, 
		hGrc->aulDefaultRegs, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Source_SetDefault);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetSurface(
	BGRC_Handle hGrc,
	BSUR_Surface_Handle hSurface )
{
	BGRC_P_State *pState;
	BGRC_P_Surface Surface;
	bool bSurfaceChanged = false;

	BDBG_ENTER(BGRC_Destination_SetSurface);
	BDBG_ASSERT( hGrc );

	pState = &hGrc->CurrentState;

	/* get surface data */
	BGRC_P_GET_SURFACE_DATA( hSurface, Surface, pState->DstSurface );

	/* check if surface is changing */
	if( hGrc->bUninitialized || bSurfaceChanged )
	{
		/* validate surface format */
		if( hSurface && BPXL_IS_DEPTH_FORMAT(Surface.eFormat) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		if( hSurface && (BPXL_IS_YCbCr444_10BIT_FORMAT(Surface.eFormat) || BPXL_IS_YCbCr422_10BIT_FORMAT(Surface.eFormat)) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		/* set register load field */
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );

		/* set surface fields */
		BGRC_P_SET_FIELD_COMP( DEST_FEEDER_ENABLE, ENABLE, ENABLE, DISABLE, hSurface );
		BGRC_P_SET_FIELD_DATA( DEST_SURFACE_ADDR_0, ADDR, Surface.ulOffset );
		BGRC_P_SET_FIELD_DATA( DEST_SURFACE_STRIDE_0, STRIDE, Surface.ulPitch );
		BGRC_P_SET_FIELD_FORMAT( DEST, FORMAT_TYPE, Surface.eFormat, pState->DstAlphaSurface.hSurface, pState->bDstPaletteBypass );
		BGRC_P_SET_FIELD_CHANNELS( DEST, Surface.eFormat, hSurface );

		BGRC_P_SET_FIELD_COMP_DATA( DEST_SURFACE_FORMAT_DEF_3, CH0_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 0) );
		BGRC_P_SET_FIELD_COMP_DATA( DEST_SURFACE_FORMAT_DEF_3, CH1_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 1) );
		BGRC_P_SET_FIELD_COMP_DATA( DEST_SURFACE_FORMAT_DEF_3, CH2_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 2) );
		BGRC_P_SET_FIELD_COMP_DATA( DEST_SURFACE_FORMAT_DEF_3, CH3_DISABLE, 0, 1, hSurface && BPXL_COMPONENT_SIZE(Surface.eFormat, 3) );

		/* store surface data */
		BKNI_Memcpy( &pState->DstSurface, &Surface, sizeof (BGRC_P_Surface) );
	}

	BDBG_LEAVE(BGRC_Destination_SetSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetAlphaSurface(
	BGRC_Handle hGrc,
	BSUR_Surface_Handle hSurface )
{
	BGRC_P_State *pState;
	BGRC_P_Surface Surface;
	bool bSurfaceChanged = false;

	BDBG_ENTER(BGRC_Destination_SetAlphaSurface);
	BDBG_ASSERT( hGrc );

	pState = &hGrc->CurrentState;

	/* get surface data */
	BGRC_P_GET_SURFACE_DATA( hSurface, Surface, pState->DstAlphaSurface );

	/* check if surface is changing */
	if( hGrc->bUninitialized || bSurfaceChanged )
	{
		/* validate surface format */
		if( hSurface && (Surface.eFormat != BPXL_eA1) && (Surface.eFormat != BPXL_eW1) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		/* set register load field */
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );

		/* set surface fields */
		BGRC_P_SET_FIELD_DATA( DEST_SURFACE_ADDR_1, ADDR, Surface.ulOffset );
		BGRC_P_SET_FIELD_DATA( DEST_SURFACE_STRIDE_1, STRIDE, Surface.ulPitch );

		/* set surface format field */
		if( pState->DstSurface.hSurface )
			BGRC_P_SET_FIELD_FORMAT( DEST, FORMAT_TYPE, pState->DstSurface.eFormat, hSurface, pState->bDstPaletteBypass );

		/* store surface data */
		BKNI_Memcpy( &pState->DstAlphaSurface, &Surface, sizeof (BGRC_P_Surface) );
	}

	BDBG_LEAVE(BGRC_Destination_SetAlphaSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_TogglePaletteBypass(
	BGRC_Handle hGrc,
	bool bEnableBypass )
{
	BGRC_P_State *pState;

	BDBG_ENTER(BGRC_Destination_TogglePaletteBypass);
	BDBG_ASSERT( hGrc );

	pState = &hGrc->CurrentState;

	/* set palette bypass field */
	BGRC_P_SET_FIELD_COMP( DEST_SURFACE_FORMAT_DEF_3, PALETTE_BYPASS, DONT_LOOKUP, LOOKUP, bEnableBypass );

	/* set format type due to bypass */
	if( pState->DstSurface.hSurface )
		BGRC_P_SET_FIELD_FORMAT( DEST, FORMAT_TYPE, pState->DstSurface.eFormat, pState->DstAlphaSurface.hSurface, bEnableBypass );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( DEST_SURFACE_FORMAT_DEF_1 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_SURFACE_FORMAT_DEF_3 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );

	pState->bDstPaletteBypass = bEnableBypass;

	BDBG_LEAVE(BGRC_Destination_TogglePaletteBypass);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetChromaExpansion(
	BGRC_Handle hGrc,
	BGRC_ChromaExpansion eChromaExpansion )
{
	BDBG_ENTER(BGRC_Destination_SetChromaExpansion);
	BDBG_ASSERT( hGrc );

	/* set chroma expansion field */
	BGRC_P_SET_FIELD_COMP( DEST_SURFACE_FORMAT_DEF_3, CHROMA_FILTER, 
		REPLICATE, FILTER, eChromaExpansion == BGRC_ChromaExpansion_eReplicate );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( DEST_SURFACE_FORMAT_DEF_3 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Destination_SetChromaExpansion);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetZeroPad(
	BGRC_Handle hGrc,
	bool bEnableZeroPad )
{
	BDBG_ENTER(BGRC_Destination_SetZeroPad);
	BDBG_ASSERT( hGrc );

	/* set zero pad field */
	BGRC_P_SET_FIELD_COMP( DEST_SURFACE_FORMAT_DEF_3, ZERO_PAD, 
		ZERO_PAD, REPLICATE, bEnableZeroPad );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( DEST_SURFACE_FORMAT_DEF_3 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Destination_SetZeroPad);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetColorKey(
	BGRC_Handle hGrc,
	uint32_t ulMin,
	uint32_t ulMax,
	uint32_t ulMask,
	uint32_t ulReplacement,
	uint32_t ulReplacementMask,
	bool bExclusive )
{
	BDBG_ENTER(BGRC_Destination_SetColorKey);
	BDBG_ASSERT( hGrc );

	/* set color key fields */
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, DEST_COLOR_KEY_COMPARE, EXCUSIVE, INCLUSIVE, bExclusive );
	BGRC_P_SET_FIELD_FULL( DEST_COLOR_KEY_LOW, ulMin );
	BGRC_P_SET_FIELD_FULL( DEST_COLOR_KEY_HIGH, ulMax );
	BGRC_P_SET_FIELD_FULL( DEST_COLOR_KEY_MASK, ulMask);
	BGRC_P_SET_FIELD_FULL( DEST_COLOR_KEY_REPLACEMENT, ulReplacement );
	BGRC_P_SET_FIELD_FULL( DEST_COLOR_KEY_REPLACEMENT_MASK, ulReplacementMask );

	/* set register load fields */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	if( BGRC_P_REGISTER_CHANGED( DEST_COLOR_KEY_LOW ) ||
		BGRC_P_REGISTER_CHANGED( DEST_COLOR_KEY_HIGH ) ||
		BGRC_P_REGISTER_CHANGED( DEST_COLOR_KEY_MASK ) ||
		BGRC_P_REGISTER_CHANGED( DEST_COLOR_KEY_REPLACEMENT ) ||
		BGRC_P_REGISTER_CHANGED( DEST_COLOR_KEY_REPLACEMENT_MASK ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_COLOR_KEY_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Destination_SetColorKey);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_ToggleColorKey(
	BGRC_Handle hGrc,
	bool bEnable )
{
	BDBG_ENTER(BGRC_Destination_ToggleColorKey);
	BDBG_ASSERT( hGrc );

	/* set color key enable field */
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, DEST_COLOR_KEY_ENABLE, 
		ENABLE, DISABLE, bEnable );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Destination_ToggleColorKey);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetColorMatrix5x4(
	BGRC_Handle hGrc,
	const int32_t ai32_Matrix[],
	uint32_t ulShift )
{
	BDBG_ENTER(BGRC_Destination_SetColorMatrix5x4);
	BDBG_ASSERT( hGrc );
	BDBG_ASSERT( ai32_Matrix );

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	/* set color matrix fields */
	BGRC_P_SET_FIELD_MATRIX_ROW( DEST, 0, ai32_Matrix,  0, ulShift );
	BGRC_P_SET_FIELD_MATRIX_ROW( DEST, 1, ai32_Matrix,  5, ulShift );
	BGRC_P_SET_FIELD_MATRIX_ROW( DEST, 2, ai32_Matrix, 10, ulShift );
	BGRC_P_SET_FIELD_MATRIX_ROW( DEST, 3, ai32_Matrix, 15, ulShift );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( DEST_CM_C00_C01 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_CM_C02_C03 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_CM_C04 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_CM_C10_C11 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_CM_C12_C13 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_CM_C14 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_CM_C20_C21 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_CM_C22_C23 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_CM_C24 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_CM_C30_C31 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_CM_C32_C33 ) ||
		BGRC_P_REGISTER_CHANGED( DEST_CM_C34 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_COLOR_MATRIX_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Destination_SetColorMatrix5x4);
	return BERR_SUCCESS;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( ai32_Matrix );
	BSTD_UNUSED( ulShift );
	BDBG_LEAVE(BGRC_Destination_SetColorMatrix5x4);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/***************************************************************************/
BERR_Code BGRC_Destination_ToggleColorMatrix(
	BGRC_Handle hGrc,
	bool bEnable )
{
	BDBG_ENTER(BGRC_Destination_ToggleColorMatrix);
	BDBG_ASSERT( hGrc );

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	/* set color matrix enable field */
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, DEST_COLOR_MATRIX_ENABLE, 
		ENABLE, DISABLE, bEnable );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Destination_ToggleColorMatrix);
	return BERR_SUCCESS;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( bEnable );
	BDBG_LEAVE(BGRC_Destination_ToggleColorMatrix);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetColorMatrixRounding(
	BGRC_Handle hGrc,
	BGRC_Rounding eMatrixRounding )
{
	BDBG_ENTER(BGRC_Destination_SetColorMatrixRounding);
	BDBG_ASSERT( hGrc );

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	/* set matrix rounding field */
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, DEST_COLOR_MATRIX_ROUNDING, 
		TRUNCATE, NEAREST, eMatrixRounding == BGRC_Rounding_eTruncate );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Destination_SetColorMatrixRounding);
	return BERR_SUCCESS;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( eMatrixRounding );
	BDBG_LEAVE(BGRC_Destination_SetColorMatrixRounding);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetKeyMatrixOrder(
	BGRC_Handle hGrc,
	BGRC_KeyMatrixOrder eKeyMatrixOrder )
{
	BDBG_ENTER(BGRC_Destination_SetKeyMatrixOrder);
	BDBG_ASSERT( hGrc );

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	/* set colorkey/matrix order field */
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, CBAR_DEST_COLOR, KEY_THEN_MATRIX, 
		MATRIX_THEN_KEY, eKeyMatrixOrder == BGRC_KeyMatrixOrder_eKeyThenMatrix );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Destination_SetKeyMatrixOrder);
	return BERR_SUCCESS;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( eKeyMatrixOrder );
	BDBG_LEAVE(BGRC_Destination_SetKeyMatrixOrder);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetColor(
	BGRC_Handle hGrc,
	uint32_t ulColor )
{
	BDBG_ENTER(BGRC_Destination_SetColor);
	BDBG_ASSERT( hGrc );

	/* set single color field */
	BGRC_P_SET_FIELD_FULL( DEST_CONSTANT_COLOR, ulColor );
	BGRC_P_SET_FIELD_DATA( DEST_W_ALPHA, W0_ALPHA, BPXL_GET_COMPONENT( BPXL_eA8_R8_G8_B8, ulColor, 3 ) );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( DEST_CONSTANT_COLOR ) ||
		BGRC_P_REGISTER_CHANGED( DEST_W_ALPHA ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, DST_FEEDER_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Destination_SetColor);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetRectangle(
	BGRC_Handle hGrc,
	uint32_t ulLeft,
	uint32_t ulTop,
	uint32_t ulWidth,
	uint32_t ulHeight )
{
	BDBG_ENTER(BGRC_Destination_SetRectangle);
	BDBG_ASSERT( hGrc );

	/* validate dimensions */
	if( !BGRC_P_VALIDATE_SURFACE_DIMENSIONS( ulLeft, ulTop, ulWidth, ulHeight ) )
		return BERR_TRACE(BGRC_ERR_DESTINATION_DIMENSIONS_INVALID);

	if( (ulLeft > BGRC_P_SURFACE_RECT_SIZE_MAX) || (ulTop > BGRC_P_SURFACE_RECT_SIZE_MAX) ||
		(ulWidth > BGRC_P_SURFACE_RECT_SIZE_MAX) || (ulHeight > BGRC_P_SURFACE_RECT_SIZE_MAX) )
		return BERR_TRACE(BGRC_ERR_DESTINATION_DIMENSIONS_INVALID);

	/* store width and height */
	hGrc->CurrentState.DstRect.ulX = ulLeft;
	hGrc->CurrentState.DstRect.ulY = ulTop;
	hGrc->CurrentState.DstRect.ulWidth = ulWidth;
	hGrc->CurrentState.DstRect.ulHeight = ulHeight;

	BDBG_LEAVE(BGRC_Destination_SetRectangle);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetDirection(
	BGRC_Handle hGrc,
	bool bRightToLeft,
	bool bBottomToTop )
{
	BDBG_ENTER(BGRC_Destination_SetDirection);
	BDBG_ASSERT( hGrc );

	hGrc->CurrentState.bDstRightToLeft = bRightToLeft;
	hGrc->CurrentState.bDstBottomToTop = bBottomToTop;

	BDBG_LEAVE(BGRC_Destination_SetDirection);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_ResetState(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_Destination_ResetState);
	BDBG_ASSERT( hGrc );

	/* copy default state to current state */
	BGRC_P_Destination_CopyState( &hGrc->CurrentState, &hGrc->DefaultState, 
		hGrc->aulCurrentRegs, hGrc->aulDefaultRegs );

	BDBG_LEAVE(BGRC_Destination_ResetState);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Destination_SetDefault(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_Destination_SetDefault);
	BDBG_ASSERT( hGrc );

	/* copy current state to default state */
	BGRC_P_Destination_CopyState( &hGrc->DefaultState, &hGrc->CurrentState, 
		hGrc->aulDefaultRegs, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Destination_SetDefault);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Pattern_Set(
	BGRC_Handle hGrc,
	uint8_t ucVector,
	const uint8_t aucPattern[8],
	uint32_t ulColor0,
	uint32_t ulColor1 )
{
	uint32_t aulPattern[2] = { 0, 0 };

	BDBG_ENTER(BGRC_Pattern_Set);
	BDBG_ASSERT( hGrc );

	if( aucPattern )
	{
		/* setup pattern */
		hGrc->CurrentState.aucPattern[0] = aucPattern[4];
		hGrc->CurrentState.aucPattern[1] = aucPattern[5];
		hGrc->CurrentState.aucPattern[2] = aucPattern[6];
		hGrc->CurrentState.aucPattern[3] = aucPattern[7];
		hGrc->CurrentState.aucPattern[4] = aucPattern[0];
		hGrc->CurrentState.aucPattern[5] = aucPattern[1];
		hGrc->CurrentState.aucPattern[6] = aucPattern[2];
		hGrc->CurrentState.aucPattern[7] = aucPattern[3];

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
		BGRC_P_ALIGN_PATTERN( aulPattern, hGrc->CurrentState.aucPattern, 
			hGrc->CurrentState.OutRect.ulX, hGrc->CurrentState.OutRect.ulY );
#else
		BGRC_P_ALIGN_PATTERN( aulPattern, hGrc->CurrentState.aucPattern, 0, 0 );
#endif
	}

	/* set pattern/rop fields */
	BGRC_P_SET_FIELD_DATA( ROP_OPERATION, VECTOR, ucVector );
	BGRC_P_SET_FIELD_DATA( ROP_PATTERN_TOP, PATTERN, aulPattern[1] );
	BGRC_P_SET_FIELD_DATA( ROP_PATTERN_BOTTOM, PATTERN, aulPattern[0] );
	BGRC_P_SET_FIELD_FULL( ROP_PATTERN_COLOR_0, ulColor0 );
	BGRC_P_SET_FIELD_FULL( ROP_PATTERN_COLOR_1, ulColor1 );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( ROP_OPERATION ) ||
		BGRC_P_REGISTER_CHANGED( ROP_PATTERN_TOP ) ||
		BGRC_P_REGISTER_CHANGED( ROP_PATTERN_BOTTOM ) ||
		BGRC_P_REGISTER_CHANGED( ROP_PATTERN_COLOR_0 ) ||
		BGRC_P_REGISTER_CHANGED( ROP_PATTERN_COLOR_1 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, ROP_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Pattern_Set);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Pattern_ResetState(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_Pattern_ResetState);
	BDBG_ASSERT( hGrc );

	/* copy default state to current state */
	BGRC_P_Pattern_CopyState( &hGrc->CurrentState, &hGrc->DefaultState, 
		hGrc->aulCurrentRegs, hGrc->aulDefaultRegs );

	BDBG_LEAVE(BGRC_Pattern_ResetState);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Pattern_SetDefault(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_Pattern_SetDefault);
	BDBG_ASSERT( hGrc );

	/* copy current state to default state */
	BGRC_P_Pattern_CopyState( &hGrc->DefaultState, &hGrc->CurrentState, 
		hGrc->aulDefaultRegs, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Pattern_SetDefault);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Blend_SetColor(
	BGRC_Handle hGrc,
	uint32_t ulColor )
{
	BDBG_ENTER(BGRC_Blend_SetColor);
	BDBG_ASSERT( hGrc );

	/* set blend color field */
	BGRC_P_SET_FIELD_FULL( BLEND_CONSTANT_COLOR, ulColor );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLEND_CONSTANT_COLOR ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLEND_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Blend_SetColor);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Blend_SetColorBlend(
	BGRC_Handle hGrc,
	BGRC_Blend_Source eSourceA,
	BGRC_Blend_Source eSourceB,
	bool bSubtractCD,
	BGRC_Blend_Source eSourceC,
	BGRC_Blend_Source eSourceD,
	bool bSubtractE,
	BGRC_Blend_Source eSourceE )
{
	BDBG_ENTER(BGRC_Blend_SetColorBlend);
	BDBG_ASSERT( hGrc );

	/* set color blend fields */
	BGRC_P_SET_FIELD_BLEND( COLOR, eSourceA, eSourceB, eSourceC, eSourceD, eSourceE, bSubtractCD, bSubtractE );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLEND_COLOR_OP ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLEND_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Blend_SetColorBlend);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Blend_SetAlphaBlend(
	BGRC_Handle hGrc,
	BGRC_Blend_Source eSourceA,
	BGRC_Blend_Source eSourceB,
	bool bSubtractCD,
	BGRC_Blend_Source eSourceC,
	BGRC_Blend_Source eSourceD,
	bool bSubtractE,
	BGRC_Blend_Source eSourceE )
{
	BDBG_ENTER(BGRC_Blend_SetAlphaBlend);
	BDBG_ASSERT( hGrc );

	/* set alpha blend fields */
	BGRC_P_SET_FIELD_BLEND( ALPHA, eSourceA, eSourceB, eSourceC, eSourceD, eSourceE, bSubtractCD, bSubtractE );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLEND_ALPHA_OP ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLEND_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Blend_SetAlphaBlend);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Blend_ResetState(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_Blend_ResetState);
	BDBG_ASSERT( hGrc );

	/* copy default state to current state */
	BGRC_P_Blend_CopyState( &hGrc->CurrentState, &hGrc->DefaultState, 
		hGrc->aulCurrentRegs, hGrc->aulDefaultRegs );

	BDBG_LEAVE(BGRC_Blend_ResetState);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Blend_SetDefault(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_Blend_SetDefault);
	BDBG_ASSERT( hGrc );

	/* copy current state to default state */
	BGRC_P_Blend_CopyState( &hGrc->DefaultState, &hGrc->CurrentState, 
		hGrc->aulDefaultRegs, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Blend_SetDefault);
	return BERR_SUCCESS;
}

#include "bchp_sun_top_ctrl.h"

/***************************************************************************/
BERR_Code BGRC_Output_SetSurface(
	BGRC_Handle hGrc,
	BSUR_Surface_Handle hSurface )
{
	BGRC_P_State *pState;
	BGRC_P_Surface Surface;
	bool bSurfaceChanged = false;

	BDBG_ENTER(BGRC_Output_SetSurface);
	BDBG_ASSERT( hGrc );

	pState = &hGrc->CurrentState;

	/* get surface data */
	BGRC_P_GET_SURFACE_DATA( hSurface, Surface, pState->OutSurface );

	/* check if surface is changing */
	if( hGrc->bUninitialized || bSurfaceChanged )
	{
		bool bBypass = true;

		/* validate surface format */
		if( hSurface && BPXL_IS_DEPTH_FORMAT(Surface.eFormat) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		if( hSurface && (BPXL_IS_YCbCr444_10BIT_FORMAT(Surface.eFormat) || BPXL_IS_YCbCr422_10BIT_FORMAT(Surface.eFormat)) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		/* set register load field */
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, OUTPUT_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );

		/* set surface fields */
		BGRC_P_SET_FIELD_COMP( OUTPUT_FEEDER_ENABLE, ENABLE, ENABLE, DISABLE, hSurface );
		BGRC_P_SET_FIELD_DATA( OUTPUT_SURFACE_ADDR_0, ADDR, Surface.ulOffset );
		BGRC_P_SET_FIELD_DATA( OUTPUT_SURFACE_STRIDE_0, STRIDE, Surface.ulPitch );
		BGRC_P_SET_FIELD_FORMAT( OUTPUT, FORMAT_TYPE, Surface.eFormat, pState->OutAlphaSurface.hSurface, bBypass );
		BGRC_P_SET_FIELD_CHANNELS( OUTPUT, Surface.eFormat, hSurface );

		/* store surface data */
		BKNI_Memcpy( &pState->OutSurface, &Surface, sizeof (BGRC_P_Surface) );
	}

	BDBG_LEAVE(BGRC_Output_SetSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Output_SetAlphaSurface(
	BGRC_Handle hGrc,
	BSUR_Surface_Handle hSurface )
{
	BGRC_P_State *pState;
	BGRC_P_Surface Surface;
	bool bSurfaceChanged = false;

	BDBG_ENTER(BGRC_Output_SetAlphaSurface);
	BDBG_ASSERT( hGrc );

	pState = &hGrc->CurrentState;

	/* get surface data */
	BGRC_P_GET_SURFACE_DATA( hSurface, Surface, pState->OutAlphaSurface );

	/* check if surface is changing */
	if( hGrc->bUninitialized || bSurfaceChanged )
	{
		bool bBypass = true;

		/* validate surface format */
		if( hSurface && (Surface.eFormat != BPXL_eA1) && (Surface.eFormat != BPXL_eW1) )
			return BERR_TRACE(BERR_INVALID_PARAMETER);

		/* store surface data */
		BKNI_Memcpy( &pState->OutAlphaSurface, &Surface, sizeof (BGRC_P_Surface) );

		/* set register load field */
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, OUTPUT_FEEDER_GRP_CNTRL, GRP_ENABLE, pState, hGrc->aulCurrentRegs );

		/* set surface fields */
		BGRC_P_SET_FIELD_DATA( OUTPUT_SURFACE_ADDR_1, ADDR, pState->OutAlphaSurface.ulOffset );
		BGRC_P_SET_FIELD_DATA( OUTPUT_SURFACE_STRIDE_1, STRIDE, pState->OutAlphaSurface.ulPitch );

		/* set surface format field */
		if( pState->OutSurface.hSurface )
			BGRC_P_SET_FIELD_FORMAT( OUTPUT, FORMAT_TYPE, pState->OutSurface.eFormat, hSurface, bBypass );
	}

	BDBG_LEAVE(BGRC_Output_SetAlphaSurface);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Output_SetColorMatrix5x4(
	BGRC_Handle hGrc,
	const int32_t ai32_Matrix[],
	uint32_t ulShift )
{
	BDBG_ENTER(BGRC_Output_SetColorMatrix5x4);
	BDBG_ASSERT( hGrc );
	BDBG_ASSERT( ai32_Matrix );

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	/* set color matrix fields */
	BGRC_P_SET_FIELD_MATRIX_ROW( OUTPUT, 0, ai32_Matrix,  0, ulShift );
	BGRC_P_SET_FIELD_MATRIX_ROW( OUTPUT, 1, ai32_Matrix,  5, ulShift );
	BGRC_P_SET_FIELD_MATRIX_ROW( OUTPUT, 2, ai32_Matrix, 10, ulShift );
	BGRC_P_SET_FIELD_MATRIX_ROW( OUTPUT, 3, ai32_Matrix, 15, ulShift );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C00_C01 ) ||
		BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C02_C03 ) ||
		BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C04 ) ||
		BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C10_C11 ) ||
		BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C12_C13 ) ||
		BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C14 ) ||
		BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C20_C21 ) ||
		BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C22_C23 ) ||
		BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C24 ) ||
		BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C30_C31 ) ||
		BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C32_C33 ) ||
		BGRC_P_REGISTER_CHANGED( OUTPUT_CM_C34 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, OUTPUT_COLOR_MATRIX_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Output_SetColorMatrix5x4);
	return BERR_SUCCESS;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( ai32_Matrix );
	BSTD_UNUSED( ulShift );
	BDBG_LEAVE(BGRC_Output_SetColorMatrix5x4);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/***************************************************************************/
BERR_Code BGRC_Output_ToggleColorMatrix(
	BGRC_Handle hGrc,
	bool bEnable )
{
	BDBG_ENTER(BGRC_Output_ToggleColorMatrix);
	BDBG_ASSERT( hGrc );

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	/* set color matrix enable field */
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, OUTPUT_COLOR_MATRIX_ENABLE, 
		ENABLE, DISABLE, bEnable );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Output_ToggleColorMatrix);
	return BERR_SUCCESS;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( bEnable );
	BDBG_LEAVE(BGRC_Output_ToggleColorMatrix);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/***************************************************************************/
BERR_Code BGRC_Output_SetColorMatrixRounding(
	BGRC_Handle hGrc,
	BGRC_Rounding eMatrixRounding )
{
	BDBG_ENTER(BGRC_Output_SetColorMatrixRounding);
	BDBG_ASSERT( hGrc );

#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	/* set matrix rounding field */
	BGRC_P_SET_FIELD_COMP( BLIT_HEADER, OUTPUT_COLOR_MATRIX_ROUNDING, 
		TRUNCATE, NEAREST, eMatrixRounding == BGRC_Rounding_eTruncate );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLIT_HEADER ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Output_SetColorMatrixRounding);
	return BERR_SUCCESS;
#else
	BSTD_UNUSED( hGrc );
	BSTD_UNUSED( eMatrixRounding );
	BDBG_LEAVE(BGRC_Output_SetColorMatrixRounding);
	return BERR_TRACE(BERR_NOT_SUPPORTED);
#endif
}

/***************************************************************************/
BERR_Code BGRC_Output_SetRectangle(
	BGRC_Handle hGrc,
	uint32_t ulLeft,
	uint32_t ulTop,
	uint32_t ulWidth,
	uint32_t ulHeight )
{
	uint32_t aulPattern[2] = { 0, 0 };

	BDBG_ENTER(BGRC_Output_SetRectangle);
	BDBG_ASSERT( hGrc );

	/* validate dimensions */
	if( !BGRC_P_VALIDATE_SURFACE_DIMENSIONS( ulLeft, ulTop, ulWidth, ulHeight ) )
		return BERR_TRACE(BGRC_ERR_OUTPUT_DIMENSIONS_INVALID);

	if( (ulLeft > BGRC_P_SURFACE_RECT_SIZE_MAX) || (ulTop > BGRC_P_SURFACE_RECT_SIZE_MAX) ||
		(ulWidth > BGRC_P_SURFACE_RECT_SIZE_MAX) || (ulHeight > BGRC_P_SURFACE_RECT_SIZE_MAX) )
		return BERR_TRACE(BGRC_ERR_OUTPUT_DIMENSIONS_INVALID);

	/* store width and height */
	hGrc->CurrentState.OutRect.ulX = ulLeft;
	hGrc->CurrentState.OutRect.ulY = ulTop;
	hGrc->CurrentState.OutRect.ulWidth = ulWidth;
	hGrc->CurrentState.OutRect.ulHeight = ulHeight;

	/* set pattern fields */
#if ((BCHP_CHIP==7038) && (BCHP_VER < BCHP_VER_C0))
	BGRC_P_ALIGN_PATTERN( aulPattern, hGrc->CurrentState.aucPattern, ulLeft, ulTop );
#else
	BGRC_P_ALIGN_PATTERN( aulPattern, hGrc->CurrentState.aucPattern, 0, 0 );
#endif
	BGRC_P_SET_FIELD_DATA( ROP_PATTERN_TOP, PATTERN, aulPattern[1] );
	BGRC_P_SET_FIELD_DATA( ROP_PATTERN_BOTTOM, PATTERN, aulPattern[0] );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( ROP_PATTERN_TOP ) ||
		BGRC_P_REGISTER_CHANGED( ROP_PATTERN_BOTTOM ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, ROP_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Output_SetRectangle);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Output_SetColorKeySelection(
	BGRC_Handle hGrc,
	BGRC_Output_ColorKeySelection eSrcNotKeyedDstNotKeyed,
	BGRC_Output_ColorKeySelection eSrcNotKeyedDstKeyed,
	BGRC_Output_ColorKeySelection eSrcKeyedDstNotKeyed,
	BGRC_Output_ColorKeySelection eSrcKeyedDstKeyed )
{
	BDBG_ENTER(BGRC_Output_SetColorKeySelection);
	BDBG_ASSERT( hGrc );

	/* set output selction fields */
	BGRC_P_SET_FIELD_COLORKEY( eSrcNotKeyedDstNotKeyed, eSrcNotKeyedDstKeyed, eSrcKeyedDstNotKeyed, eSrcKeyedDstKeyed );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( BLEND_COLOR_KEY_ACTION ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, BLEND_PARAM_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Output_SetColorKeySelection);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Output_SetDither(
	BGRC_Handle hGrc,
	bool bEnableDither )
{
	BDBG_ENTER(BGRC_Output_SetDither);
	BDBG_ASSERT( hGrc );

	/* set dither field */
	BGRC_P_SET_FIELD_COMP( OUTPUT_SURFACE_FORMAT_DEF_3, DITHER_ENABLE, 
		ENABLE, DISABLE, bEnableDither );

	/* set register load field */
	if( BGRC_P_REGISTER_CHANGED( OUTPUT_SURFACE_FORMAT_DEF_3 ) )
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, OUTPUT_FEEDER_GRP_CNTRL, GRP_ENABLE, &hGrc->CurrentState, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Output_SetDither);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Output_SetDirection(
	BGRC_Handle hGrc,
	bool bRightToLeft,
	bool bBottomToTop )
{
	BDBG_ENTER(BGRC_Output_SetDirection);
	BDBG_ASSERT( hGrc );

	hGrc->CurrentState.bOutRightToLeft = bRightToLeft;
	hGrc->CurrentState.bOutBottomToTop = bBottomToTop;

	BDBG_LEAVE(BGRC_Output_SetDirection);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Output_ResetState(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_Output_ResetState);
	BDBG_ASSERT( hGrc );

	/* copy default state to current state */
	BGRC_P_Output_CopyState( &hGrc->CurrentState, &hGrc->DefaultState, 
		hGrc->aulCurrentRegs, hGrc->aulDefaultRegs );

	BDBG_LEAVE(BGRC_Output_ResetState);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Output_SetDefault(
	BGRC_Handle hGrc )
{
	BDBG_ENTER(BGRC_Output_SetDefault);
	BDBG_ASSERT( hGrc );

	/* copy current state to default state */
	BGRC_P_Output_CopyState( &hGrc->DefaultState, &hGrc->CurrentState, 
		hGrc->aulDefaultRegs, hGrc->aulCurrentRegs );

	BDBG_LEAVE(BGRC_Output_SetDefault);
	return BERR_SUCCESS;
}

/* End of File */
