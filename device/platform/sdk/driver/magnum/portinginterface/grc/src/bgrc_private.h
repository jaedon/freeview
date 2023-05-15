/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgrc_private.h $
 * $brcm_Revision: Hydra_Software_Devel/147 $
 * $brcm_Date: 2/16/12 3:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/grc/7405/bgrc_private.h $
 * 
 * Hydra_Software_Devel/147   2/16/12 3:25p nissen
 * SW7405-5427: Adding flags for output mirroring when blitting
 * 
 * Hydra_Software_Devel/146   2/8/12 11:25a nissen
 * SW7405-3671: Added hang field to GRC structure.
 * 
 * Hydra_Software_Devel/145   1/6/12 1:19p nissen
 * SW7435-13 : Fixed support for 7435 A0.
 * 
 * Hydra_Software_Devel/144   12/15/11 6:29p nissen
 * SW7435-13 : Added support for 7435 A0.
 * 
 * Hydra_Software_Devel/143   12/14/11 5:01p nissen
 * SW7435-13 : Added support for 7435 A0.
 * 
 * Hydra_Software_Devel/142   10/5/11 5:45p nissen
 * SW7425-248: Updated use of BDBG_OBJECTs.
 * 
 * Hydra_Software_Devel/141   10/5/11 1:04p nissen
 * SW7425-248: Removed BDBG_OBJECT support for the 7038.
 * 
 * Hydra_Software_Devel/140   9/17/11 7:11p nissen
 * SW7425-248: Added BDBG_OBJECT_ID_DECLARE.
 * 
 * Hydra_Software_Devel/139   9/17/11 6:38p nissen
 * SW7425-248: Added BDBG_OBJECT_ASSERT.
 * 
 * Hydra_Software_Devel/138   7/14/11 10:16a pntruong
 * SWDTV-7838: Fixed build errors.  Gotta have both mask/shift.
 *
 * Hydra_Software_Devel/137   4/20/11 1:20p nissen
 * SW7630-104: Added support for A8_Y8 format and big endian 420
 * conversion.
 *
 * Hydra_Software_Devel/136   9/17/10 5:58p nissen
 * SWBLURAY-21937: Reducing M2MC stripe size during scale blits on 7630
 * due to hang.
 *
 * Hydra_Software_Devel/135   7/15/10 9:18p nissen
 * SW7405-3671: Added more interrupt support.
 *
 * Hydra_Software_Devel/134   7/12/10 2:34p nissen
 * SW7405-3671: Changed packet fields in grc handle.
 *
 * Hydra_Software_Devel/133   6/21/10 2:16p nissen
 * SW7405-3671: Fixed non-packet build.
 *
 * Hydra_Software_Devel/132   6/21/10 12:31p nissen
 * SW7405-3671: Added support for packet context and settings.
 *
 * Hydra_Software_Devel/131   2/11/10 5:24p nissen
 * SW7405-3671: Added support for cached packet memory.
 *
 * Hydra_Software_Devel/130   2/3/10 2:05p nissen
 * SW7405-3671: Added batch packets.
 *
 * Hydra_Software_Devel/129   1/29/10 7:25p nissen
 * SW7405-3671: Added support for using packets in any order.
 *
 * Hydra_Software_Devel/128   1/19/10 12:49p nissen
 * SW7405-3671: Added packet sync offset field to GRC handle structure.
 *
 * Hydra_Software_Devel/127   1/9/10 10:58a nissen
 * SW7405-3671: Added support for packet blits.
 *
 * Hydra_Software_Devel/126   8/20/09 12:42p nissen
 * PR 52470: Changed some variables and functions to const and/or static.
 *
 * Hydra_Software_Devel/125   8/18/09 5:33p nissen
 * PR 52470: Optimized setting registers.
 *
 * Hydra_Software_Devel/124   8/13/09 6:26p nissen
 * PR 57469: Increased operation min.
 *
 * Hydra_Software_Devel/123   8/12/09 1:13p nissen
 * PR 57469: Set min packet and operation sizes when opening.
 *
 * Hydra_Software_Devel/122   8/10/09 6:09p nissen
 * PR 57469: Fixed problem with WaitForOperationReady function.
 *
 * Hydra_Software_Devel/121   7/7/09 5:47p nissen
 * PR 56610: Moved local declaration to GRC handle structure.
 *
 * Hydra_Software_Devel/120   6/22/09 5:13p nissen
 * PR 56219: Added new macro for setting register field data.
 *
 * Hydra_Software_Devel/119   6/7/09 5:15p nissen
 * PR 51077: Removed ulOperationsSinceInterrupt field from BGRC_P_Handle
 * structure.
 *
 * Hydra_Software_Devel/118   5/13/09 10:46a nissen
 * PR 54982: Added code to make sure that surface rectangle positions are
 * not negative.
 *
 * Hydra_Software_Devel/117   4/17/09 4:30p nissen
 * PR 51077: Added callbacks to wait functions.
 *
 * Hydra_Software_Devel/116   4/7/09 2:31p nissen
 * PR 52976: Added support for 7635/7630 and future chips using RDB
 * defines.
 *
 * Hydra_Software_Devel/115   3/20/09 4:20p nissen
 * PR 51077: Added functions to wait on operations, and better handling
 * for running out of memory.
 *
 * Hydra_Software_Devel/114   1/31/09 1:15a jrubio
 * PR51629: add 7336 support
 *
 * Hydra_Software_Devel/113   12/3/08 6:53p nissen
 * PR 47763: Added support for 7420.
 *
 * Hydra_Software_Devel/112   7/30/08 8:16p nissen
 * PR 33687: Added support for the 7601.
 *
 * Hydra_Software_Devel/111   6/23/08 12:54p nissen
 * PR 43693: Removed scaler coefficient register overwrite fix for 3548
 * and 3556.
 *
 * Hydra_Software_Devel/110   6/4/08 8:02p nissen
 * PR 43321: Updated for 3548 and 3556.
 *
 * Hydra_Software_Devel/109   6/4/08 7:51p nissen
 * PR 42985: Changed scaler coefficient define from 16 to 64 for 3563 Cx
 * and greater.
 *
 * Hydra_Software_Devel/108   4/4/08 11:54a nissen
 * PR 39460: Added support for the 3556.
 *
 * Hydra_Software_Devel/107   2/8/08 2:21p nissen
 * PR 38945: Added support for the 3548.
 *
 * Hydra_Software_Devel/106   2/8/08 10:50a nissen
 * PR 39221: Added pulldown register overwrite fix for 3563 C0.
 *
 * Hydra_Software_Devel/105   2/7/08 12:46p nissen
 * PR 24618: Added sharper anti-flutter filter coefficient table.
 *
 * Hydra_Software_Devel/104   12/17/07 9:51a nissen
 * PR 38269: Added support for inverting sources during a blit.
 *
 * Hydra_Software_Devel/103   12/10/07 3:03p nissen
 * PR 29724: Fixed problem with reseting macroblock state.
 *
 * Hydra_Software_Devel/102   12/4/07 5:42p nissen
 * PR 28248: Removed register overwrite fix for 3563 C0 and above.
 *
 * Hydra_Software_Devel/101   12/4/07 5:17p nissen
 * PR 36878: Added support for the 7335.
 *
 * Hydra_Software_Devel/100   11/16/07 3:03p nissen
 * PR 36759: Added support for setting the scale factor independently of
 * the provided rectangle sizes.
 *
 * Hydra_Software_Devel/99   10/12/07 7:42a nissen
 * PR 35920: Added support for preallocating system memory for operation
 * structures when opening module.
 *
 * Hydra_Software_Devel/98   10/11/07 6:05p nissen
 * PR 33390: Increased scale down max, and added define vertical scale
 * down max.
 *
 * Hydra_Software_Devel/97   9/19/07 11:19a nissen
 * PR 34538: Added support for building for the 7325.
 *
 * Hydra_Software_Devel/96   9/18/07 1:27p nissen
 * PR 34852: Added support for building for the 3573.
 *
 * Hydra_Software_Devel/95   8/20/07 2:06p nissen
 * PR 34132: Added support for setting a linear 420 source.
 *
 * Hydra_Software_Devel/94   8/20/07 12:46p nissen
 * PR 33390: Reduce scale down max define from 15 to 7 for new chips, due
 * to hang when scaling down past 7 times.
 *
 * Hydra_Software_Devel/93   5/2/07 11:59a nissen
 * PR 29724: Fixed support for MacroBlock strip width.
 *
 * Hydra_Software_Devel/92   5/2/07 11:34a nissen
 * PR 29724: Added support for 7405 A0 including new MacroBlock strip
 * width setting.
 *
 * Hydra_Software_Devel/91   3/29/07 11:28a nissen
 * PR 28248: Fixed problem with 3563 M2MC overwritting source color matrix
 * and pulldown registers when writing scaler coefficent registers.
 *
 * Hydra_Software_Devel/90   3/28/07 9:49p nissen
 * PR 29156: Fixed bug with palette surface identification.
 *
 * Hydra_Software_Devel/89   3/28/07 5:21p nissen
 * PR 29156: Fixed bug with palette surface identification.
 *
 * Hydra_Software_Devel/88   3/28/07 2:26p nissen
 * PR 25683: Fixed problem with identifying a surface when changing its
 * palette.
 *
 * Hydra_Software_Devel/87   3/2/07 11:36a nissen
 * PR 28010: Added Macroblock feature support for 7440 B0.
 *
 * Hydra_Software_Devel/86   3/1/07 12:33p nissen
 * PR 25991: Fixed MacroBlock feature for 7400 B0.
 *
 * Hydra_Software_Devel/85   1/29/07 10:44a nissen
 * PR 19665: Fixed problem with hardware striping while scaling.
 *
 * Hydra_Software_Devel/84   1/22/07 1:43p nissen
 * PR 20763: Fixed support for 10-bit YCbCr 444 format on 3563.
 *
 * Hydra_Software_Devel/83   12/1/06 10:53a nissen
 * PR 25991: Added support for 7400 B0.
 *
 * Hydra_Software_Devel/82   11/15/06 2:33p nissen
 * PR 25683: Fixed problem with using new surface id.
 *
 * Hydra_Software_Devel/81   11/15/06 11:39a nissen
 * PR 25668: Added support for 7403.
 *
 * Hydra_Software_Devel/80   11/15/06 11:00a nissen
 * PR 25683: Added code to use new surface id.
 *
 * Hydra_Software_Devel/79   11/8/06 11:11a nissen
 * PR 25683: Added support for using a surface ID to identify new
 * surfaces.
 *
 * Hydra_Software_Devel/78   10/24/06 7:32p nissen
 * PR 24648: Fixed problem with VC-1 macroblock
 *
 * Hydra_Software_Devel/77   10/19/06 11:02a nissen
 * PR 24647: Added support for preallocting all device memory when module
 * is opened.
 *
 * Hydra_Software_Devel/76   10/6/06 10:56a nissen
 * PR 24719: Fixed macro used to calculate input stripe width on 7118.
 *
 * Hydra_Software_Devel/75   10/5/06 1:55p nissen
 * PR 24648: Added support for VC-1 macro block range remapping and
 * expansion for 7401 C0 and 7118.
 *
 * Hydra_Software_Devel/74   9/29/06 10:03a nissen
 * PR 24618: Added support for second blurryer anti-flutter filter
 * coefficient table.
 *
 * Hydra_Software_Devel/73   9/8/06 3:51p nissen
 * PR 23517: Added support for 7440.
 *
 * Hydra_Software_Devel/72   8/25/06 11:22a nissen
 * PR 20763: Added support for 10-bit YCbCr 444 and 422 formats.
 *
 * Hydra_Software_Devel/71   8/17/06 11:45a nissen
 * PR 21940: Fixed filter table support for 7118.
 *
 * Hydra_Software_Devel/70   8/17/06 11:23a nissen
 * PR 23179: Added support for building and running on the 3563.
 *
 * Hydra_Software_Devel/69   6/22/06 10:38a nissen
 * PR 21940: Added support for 7118.
 *
 * Hydra_Software_Devel/68   6/15/06 9:36a nissen
 * PR 21171: Adjusted register group counts for 7401 B0.
 *
 * Hydra_Software_Devel/67   5/30/06 10:45a nissen
 * PR 21878: Added support for setting the operation time out interval
 * with settings structure when opening module.
 *
 * Hydra_Software_Devel/66   5/3/06 12:38p nissen
 * PR 21171: Added support for building and running 7401 B0.
 *
 * Hydra_Software_Devel/65   4/18/06 1:20p nissen
 * PR 18369: Added support for hardware striping when scaling on the 7400
 * and 7401.
 *
 * Hydra_Software_Devel/64   3/22/06 1:05p nissen
 * PR 19551: Added register value compare macro.
 *
 * Hydra_Software_Devel/63   3/22/06 12:02p nissen
 * PR 19551: Eliminted color and format BCHP_M2MC_ defines.
 *
 * Hydra_Software_Devel/62   3/21/06 12:58p nissen
 * PR 20331: Calling new function to get surface and palette offsets.
 *
 * Hydra_Software_Devel/61   3/17/06 10:39a nissen
 * PR 19084: Added brackets to parameter for macros that reads and writes
 * registers
 *
 * Hydra_Software_Devel/60   3/15/06 1:20p nissen
 * PR 15840: Fixed problem with YCbCr420 format without filtering enabled.
 *
 * Hydra_Software_Devel/59   3/15/06 12:10p nissen
 * PR 15840: Added support for using YCbCr420 surfaces as a source.
 *
 * Hydra_Software_Devel/58   3/8/06 2:49p nissen
 * PR 19575: Added support for building and running second m2mc on 7438.
 *
 * Hydra_Software_Devel/57   2/21/06 4:33p nissen
 * PR 19553: Fixed problem with setting filter coefficient registers for
 * 7400.
 *
 * Hydra_Software_Devel/56   2/21/06 3:27p nissen
 * PR 19084: Fixed macros that reads and writes registers.
 *
 * Hydra_Software_Devel/55   2/21/06 11:58a nissen
 * PR 19084: Added support for second m2mc device.
 *
 * Hydra_Software_Devel/54   2/9/06 5:27p nissen
 * PR 19553: Adjusted register groups.
 *
 * Hydra_Software_Devel/53   2/1/06 5:27p nissen
 * PR 19084: Added support for building for the 7400 A0.
 *
 * Hydra_Software_Devel/52   1/18/06 11:34a nissen
 * PR 19116: Added support for inactivating operations when no interrupts
 * are expected.
 *
 * Hydra_Software_Devel/51   1/12/06 5:48p nissen
 * PR 18761: Added support for BCHP_VER macro.
 *
 * Hydra_Software_Devel/50   12/3/05 6:00p nissen
 * PR 18007: Increased block size and max allocation defines.
 *
 * Hydra_Software_Devel/49   12/1/05 6:03p nissen
 * PR 18007: Added code to return error when down scaling more than 15x
 * for M2MC Cx and above.
 *
 * Hydra_Software_Devel/48   10/25/05 1:44p nissen
 * PR 17518: Modified scaler and filter related defines for 7401.
 *
 * Hydra_Software_Devel/47   10/5/05 10:55a nissen
 * PR 17273: Added memory cache support.
 *
 * Hydra_Software_Devel/46   9/13/05 4:27p nissen
 * PR 16499: Added code to free memory past a default or specified
 * threshold.
 *
 * Hydra_Software_Devel/45   8/2/05 4:12p nissen
 * PR 16489: Fixed problem with processing interrupts for operations that
 * have not completed.
 *
 * Hydra_Software_Devel/44   7/7/05 5:47p nissen
 * PR 15108: Fixed problem with getting palette offset.
 *
 * Hydra_Software_Devel/43   7/5/05 5:25p nissen
 * PR 16056: Added support for building for the 7401 A0.
 *
 * Hydra_Software_Devel/43   7/5/05 5:19p nissen
 * PR 16056: Added support for building for the 7401 A0.
 *
 * Hydra_Software_Devel/42   5/18/05 5:31p agin
 * PR14720: B2, C1, C2 compilation support.
 *
 * Hydra_Software_Devel/41   5/12/05 4:34p nissen
 * PR 15264: Modified support for user defined phase adjustment when
 * filtering.
 *
 * Hydra_Software_Devel/40   5/12/05 2:44p nissen
 * PR 15264: Added support for user defined phase adjustment when
 * filtering.
 *
 * Hydra_Software_Devel/39   5/12/05 10:25a nissen
 * PR 15226: Modified code to store surface information when a surface is
 * set instead of looking it up later.
 *
 * Hydra_Software_Devel/38   5/6/05 1:38p nissen
 * PR 13488: Added support for M2MC C1.
 *
 * Hydra_Software_Devel/37   3/16/05 12:26p nissen
 * PR 14329: Fixed setting format field for W formats.
 *
 * Hydra_Software_Devel/36   2/17/05 1:11p nissen
 * PR 14086: Added support for new color matrix register precision on the
 * 3560.
 *
 * Hydra_Software_Devel/35   12/17/04 1:40p nissen
 * PR 13325: Added packet offset field to operation data structure.
 *
 * Hydra_Software_Devel/34   12/13/04 2:12p nissen
 * PR 13488: Added support for M2MC C0.
 *
 * Hydra_Software_Devel/33   9/24/04 2:32p nissen
 * PR 12728: Removed extra semi-colon for pedantic errors.
 *
 * Hydra_Software_Devel/32   9/23/04 2:07p nissen
 * PR 12729: Added support for anti-flutter filtering.
 *
 * Hydra_Software_Devel/31   6/28/04 1:05p nissen
 * PR 11700: Eliminated compiler warnings.
 *
 * Hydra_Software_Devel/30   6/22/04 9:10p nissen
 * PR 11638: Added fix for expanding alpha using replication for ARGBB1555
 * formats.
 *
 * Hydra_Software_Devel/29   6/11/04 10:25p nissen
 * PR 11486: Fixed problemss with scaling.
 *
 * Hydra_Software_Devel/28   6/3/04 8:32p nissen
 * PR 11349: Added functionality for tracking interrupts.
 *
 * Hydra_Software_Devel/27   5/24/04 5:08p jasonh
 * PR 11189: Merge down from B0 to main-line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/27/04 7:22p nissen
 * PR 9635: Added support for M2MC B0.
 *
 * Hydra_Software_Devel/26   4/12/04 9:53a pntruong
 * PR 10548: Changed private shift function to non-private.
 *
 * Hydra_Software_Devel/25   4/9/04 2:14p nissen
 * PR 10380: Modified field setting macros to do mask and set operation
 * for more than one field at a time.
 *
 * Hydra_Software_Devel/24   3/31/04 12:01p nissen
 * PR 10165: Added macros to validate surface rectangles.
 *
 * Hydra_Software_Devel/23   3/12/04 2:32p nissen
 * PR 10024: Added support for more error checking.
 * PR 10072: Added support for no-scale filtering.
 *
 * Hydra_Software_Devel/22   3/5/04 2:39p nissen
 * PR 9856: Fixed problem with rectangle width's and height's being set to
 * zero.
 *
 * Hydra_Software_Devel/21   2/17/04 2:39p nissen
 * PR 9719: Added function defintion for getting sharper filter
 * coefficient table.
 *
 * Hydra_Software_Devel/20   2/10/04 11:19a nissen
 * PR 9700: Added function definition for getting blurry filter
 * coefficients.
 *
 * Hydra_Software_Devel/19   2/2/04 12:49p nissen
 * PR 8725: Eliminated warnings from set alpha blend macro.
 *
 * Hydra_Software_Devel/18   1/30/04 1:49p nissen
 * PR 9537: Fixed macro that checks if a register has changed.
 *
 * Hydra_Software_Devel/17   1/21/04 2:01p nissen
 * PR 9432: Added more error checking.
 *
 * Hydra_Software_Devel/16   12/18/03 2:13p nissen
 * PR 8725: Eliminated warnings.
 *
 * Hydra_Software_Devel/15   11/21/03 6:45p nissen
 * Fixed problem with callback function pointer.
 *
 * Hydra_Software_Devel/14   11/20/03 7:01p nissen
 * Added operation queuing mechanism and support for interrupts.
 *
 * Hydra_Software_Devel/13   10/20/03 1:37p nissen
 * Removed macro.
 *
 * Hydra_Software_Devel/12   10/9/03 1:38p nissen
 * Removed IKOS hacks.
 *
 * Hydra_Software_Devel/11   9/29/03 11:19a nissen
 * Fixed problem with macro that sets filter coeff fields.
 *
 * Hydra_Software_Devel/10   9/19/03 4:00p nissen
 * Added support for aligning ROP pattern.
 *
 * Hydra_Software_Devel/9   8/26/03 4:13p nissen
 * Added macro to get surface data.
 *
 * Hydra_Software_Devel/8   8/21/03 1:29p nissen
 * Fixed copy field macro.
 *
 * Hydra_Software_Devel/7   8/20/03 4:47p nissen
 * Added macros to copy registers and fields. Added state structure.
 *
 * Hydra_Software_Devel/6   8/19/03 10:14a nissen
 * Fixed macro that sets surface format type field. Fixed macro that reads
 * register.
 *
 * Hydra_Software_Devel/5   8/12/03 11:37a nissen
 * Changed field and register setting macros.
 *
 * Hydra_Software_Devel/4   8/6/03 2:36p nissen
 * Update macros for setting registers and their fields.
 *
 * Hydra_Software_Devel/3   6/25/03 11:59a nissen
 * Added macros for setting and getting register values.
 *
 * Hydra_Software_Devel/2   6/5/03 11:50a nissen
 * Added macro to set surface format type field.
 *
 * Hydra_Software_Devel/1   5/28/03 5:33p nissen
 * Added macros of format definition type.
 *
 ***************************************************************************/

#ifndef BGRC_PRIVATE_H__
#define BGRC_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bgrc_packet.h"
#include "bgrc_packet_priv.h"

/***************************************************************************/
BDBG_OBJECT_ID_DECLARE(BGRC);

/***************************************************************************/
#ifdef BCHP_M2MC_OUTPUT_CM_C34
#define BGRC_P_REG_COUNT                                    (((BCHP_M2MC_OUTPUT_CM_C34 - BCHP_M2MC_SCRATCH_LIST) >> 2) + 1)
#else
#define BGRC_P_REG_COUNT                                    (((BCHP_M2MC_SRC_CM_C34 - BCHP_M2MC_SCRATCH_LIST) >> 2) + 1)
#endif

#define BGRC_P_GROUP_COUNT                                  15
#define BGRC_P_HEADER_COUNT                                 2
#define BGRC_P_USERDATA_COUNT                               3

#define BGRC_P_OPERATION_MIN                                128
#define BGRC_P_OPERATION_MAX                                256

#define BGRC_P_LIST_BLOCK_ALIGN                             5
#define BGRC_P_LIST_BLOCK_SIZE                              2048
#define BGRC_P_LIST_BLOCK_MIN_SIZE                          16384

/***************************************************************************/
#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
#define BGRC_P_LIST_SRC_FEEDER_GRP_CNTRL_COUNT              13
#define BGRC_P_LIST_BLIT_GRP_CNTRL_COUNT                    17
#define BGRC_P_LIST_SCALE_PARAM_GRP_CNTRL_COUNT             13
#else
#define BGRC_P_LIST_SRC_FEEDER_GRP_CNTRL_COUNT              10
#define BGRC_P_LIST_BLIT_GRP_CNTRL_COUNT                    11
#define BGRC_P_LIST_SCALE_PARAM_GRP_CNTRL_COUNT             9
#endif

#define BGRC_P_LIST_DST_FEEDER_GRP_CNTRL_COUNT              10
#define BGRC_P_LIST_OUTPUT_FEEDER_GRP_CNTRL_COUNT           8
#define BGRC_P_LIST_BLEND_PARAM_GRP_CNTRL_COUNT             4
#define BGRC_P_LIST_ROP_GRP_CNTRL_COUNT                     5
#define BGRC_P_LIST_SRC_COLOR_KEY_GRP_CNTRL_COUNT           5
#define BGRC_P_LIST_DST_COLOR_KEY_GRP_CNTRL_COUNT           5

#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_67)
#define BGRC_P_LIST_SCALE_COEF_GRP_CNTRL_COUNT              64
#else
#define BGRC_P_LIST_SCALE_COEF_GRP_CNTRL_COUNT              16
#endif

#define BGRC_P_LIST_SRC_COLOR_MATRIX_GRP_CNTRL_COUNT        12
#define BGRC_P_LIST_DST_COLOR_MATRIX_GRP_CNTRL_COUNT        12
#define BGRC_P_LIST_OUTPUT_COLOR_MATRIX_GRP_CNTRL_COUNT     12
#define BGRC_P_LIST_SRC_CLUT_GRP_CNTRL_COUNT                1
#define BGRC_P_LIST_DST_CLUT_GRP_CNTRL_COUNT                1

#define BGRC_P_MACROBLOCK_RANGE_NONE                        8
#define BGRC_P_MACROBLOCK_RANGE_REMAPPING                   9
#define BGRC_P_MACROBLOCK_RANGE_EXPANSION                   16

/***************************************************************************/
typedef struct
{
	uint32_t ulX;
	uint32_t ulY;
	uint32_t ulWidth;
	uint32_t ulHeight;
}
BGRC_P_Rect;

/***************************************************************************/
typedef struct BGRC_P_Block BGRC_P_Block;
struct BGRC_P_Block
{
	BGRC_P_Block *pNextBlock;           /* pointer to next block */
	void *pvMemory;                     /* pointer to device memory */
	void *pvCached;                     /* pointer to cached device memory */
	uint32_t ulOffset;                  /* device memory offset */
	uint32_t ulRefCount;
	bool bBusy;                         /* indicates if block is busy */
};

/***************************************************************************/
typedef struct BGRC_P_Operation BGRC_P_Operation;
struct BGRC_P_Operation
{
	BGRC_P_Operation *pPrevOp;
	BGRC_P_Operation *pNextOp;
	BGRC_P_Block *pBlock;
	BGRC_Callback pUserCallback;
	void *pUserData;
	uint32_t ulPacketOffset;
	uint32_t ulPacketSize;
	bool bSetEvent;
	bool bSetPeriodicEvent;
	bool bActive;
};

/***************************************************************************/
typedef struct
{
	BSUR_Surface_Handle hSurface;
	BPXL_Format eFormat;
	void *pMemory;
	uint32_t ulOffset;
	uint32_t ulPitch;
	uint32_t ulPaletteOffset;
	uint32_t ulPaletteEntries;
	uint32_t ulSurfaceWidth;
	uint32_t ulSurfaceHeight;
	uint32_t ulX;
	uint32_t ulY;
	uint32_t ulWidth;
	uint32_t ulHeight;
	uint32_t ulID;
}
BGRC_P_Surface;

/***************************************************************************/
typedef struct
{
	BGRC_P_Surface SrcSurface;               /* source surface structure */
	BGRC_P_Surface DstSurface;               /* destination surface structure */
	BGRC_P_Surface OutSurface;               /* output surface structure */
	BGRC_P_Surface SrcAlphaSurface;          /* source alpha surface structure */
	BGRC_P_Surface DstAlphaSurface;          /* destination alpha surface structure */
	BGRC_P_Surface OutAlphaSurface;          /* output alpha surface structure */
	BGRC_P_Rect SrcRect;                     /* source rectangle */
	BGRC_P_Rect DstRect;                     /* destination rectangle */
	BGRC_P_Rect OutRect;                     /* output rectangle */
	BGRC_FilterCoeffs eHorzCoeffs;           /* horizontal scaler coefficient */
	BGRC_FilterCoeffs eVertCoeffs;           /* vertical scaler coefficient */
	const uint32_t *pulHorzFirCoeffs;        /* pointer to horz fir coefficients */
	const uint32_t *pulVertFirCoeffs;        /* pointer to vert fir coefficients */
	uint32_t ulHorzScalerStep;               /* horizontal scaler step */
	uint32_t ulVertScalerStep;               /* vertical scaler step */
	uint32_t ulHorzInitPhase;                /* horizontal initial phase */
	uint32_t ulVertInitPhase;                /* vertical initial phase */
	uint32_t ulHorzScalerNum;                /* horizontal scale factor numerator */
	uint32_t ulHorzScalerDen;                /* horizontal scale factor denominator */
	uint32_t ulVertScalerNum;                /* vertical scale factor numerator */
	uint32_t ulVertScalerDen;                /* vertical scale factor denominator */
	uint32_t ulHorzAveragerCount;            /* horizontal averager count */
	uint32_t ulVertAveragerCount;            /* vertical averager count */
	uint32_t ulHorzAveragerCoeff;            /* horizontal averager coefficient */
	uint32_t ulVertAveragerCoeff;            /* vertical averager coefficient */
	uint32_t ulSrcStripWidth;                /* src strip width for scaling*/
	uint32_t ulOutStripWidth;                /* out strip width for scaling */
	uint32_t ulOverlapStrip;                 /* overlap strip for scaling */
	uint32_t ulPhaseShift;                   /* Phase shift value */
	int32_t iHorzPhaseAdj;                   /* horizontal initial phase ajustment */
	int32_t iVertPhaseAdj;                   /* vertical initial phase ajustment */
	uint8_t aucPattern[8];                   /* 8x8 bit ROP pattern */
	bool bHorzFilter;                        /* enables horizontal filter */
	bool bVertFilter;                        /* enables vertical filter */
	bool bSrcPaletteBypass;                  /* enables bypassing src palette */
	bool bDstPaletteBypass;                  /* enables bypassing dst palette */
	bool bSrcRightToLeft;
	bool bSrcBottomToTop;
	bool bDstRightToLeft;
	bool bDstBottomToTop;
	bool bOutRightToLeft;
	bool bOutBottomToTop;
	uint32_t ulMacroBlockRangeY;
	uint32_t ulMacroBlockRangeC;
	uint32_t ulMacroBlockStripWidth;
	bool bMacroBlockLinear;
	bool bMacroBlockBigEndian;
}
BGRC_P_State;

/***************************************************************************/
BDBG_OBJECT_ID_DECLARE(BGRC);
typedef struct BGRC_P_Handle
{
	BDBG_OBJECT(BGRC)
	BCHP_Handle hChip;                          /* handle to chip module */
	BREG_Handle hRegister;                      /* handle to register module */
	BMEM_Handle hMemory;                        /* handle to memory module */
	BINT_Handle hInterrupt;                     /* handle to interrupt module */
	
	uint32_t ulDeviceNum;						/* number of M2MC device being used */
	uint32_t ulPacketMemoryMax; 				/* max packet memory */
	uint32_t ulOperationMax;					/* max operations */
	uint32_t ulWaitTimeout; 					/* seconds to wait before assuming device is hung */
	
#ifdef BGRC_NON_PACKET_MODE
	BINT_CallbackHandle hInterruptCallback;     /* handle to interrupt callback */
	BKNI_EventHandle hInterruptEvent;           /* handle to interrupt event */
	BKNI_EventHandle hPeriodicEvent;            /* handle to periodic event */
	uint32_t aulCurrentRegs[BGRC_P_REG_COUNT];  /* array of current m2mc registers */
	uint32_t aulDefaultRegs[BGRC_P_REG_COUNT];  /* array of default m2mc registers */
	uint32_t aulActualRegs[BGRC_P_REG_COUNT];   /* array of actual m2mc registers */
	uint32_t aulStoredRegs[BGRC_P_REG_COUNT];   /* array of stored m2mc registers */
	BGRC_P_State CurrentState;                  /* current state information */
	BGRC_P_State DefaultState;                  /* default state information */
	BGRC_P_State StoredState;                   /* stored state information */
	uint32_t ulSurfaceID;                       /* current surface id */
	uint32_t ulPeriodicInterrupts;              /* count of pending periodic interrupts */
	bool bNoScaleFilter;                        /* indicates filtering without scaling */
	bool bYCbCr420Source;                       /* source is YCbCr420 */
	bool bUninitialized;                        /* indicates if module is inited */
	bool bSetEvent;                             /* indicates if isr should set event */
	bool bPeriodicInterrupt;                    /* indicates if interrupt should be fired */

	BGRC_P_Block *pCurrListBlock;               /* pointer to current list memory block */
	BGRC_P_Block *pPrevListBlock;               /* pointer to current list memory block */
	uint32_t ulListBlockPos;                    /* position within current list memory block */
	BGRC_Callback pPrevUserCallback;            /* pointer to previous user interrupt callback */
	uint32_t *pulPrevPacket;                    /* pointer to previous list packet */
	uint32_t ulPacketMemorySize;                /* amount of memory allocated for packets */
	uint32_t ulOperationCount;                  /* count allocated operations */
	uint32_t ulIntExpected;                     /* number of interrupts expected */
	uint32_t ulIntReceived;                     /* number of interrupts received */
	uint32_t ulPacketMemorySinceInterrupt;      /* packet memory allocated since periodic interrupt */
	bool bPreAllocMemory;                       /* allocate memory when opening module */

	BGRC_P_Operation *pCurrOp;
	BGRC_P_Operation *pLastOp;
	BGRC_P_Operation *pFreeOp;

	BSUR_Surface_Handle hWaitSurface;
	BGRC_Callback pPeriodicCallback;
	void *pPeriodicData;

#else
	/* packets */
	BGRC_PacketContext_Handle hContext;
	BINT_CallbackHandle hCallback;
	BGRC_Callback callback_isr;
	void *callback_data;

	BLST_D_HEAD(context_list, BGRC_P_PacketContext) context_list;

	uint32_t *fifo_memory_addr;
	uint32_t *fifo_cached_addr;
	uint32_t *fifo_base_addr;
	uint32_t  fifo_base_offset;
	uint32_t  fifo_total_size;

	uint32_t *prev_fifo_addr;
	uint32_t *curr_fifo_addr;
	uint32_t  curr_fifo_offset;
	uint32_t *last_fifo_addr;
	uint32_t  last_fifo_offset;

	uint32_t *start_flush_addr;
	uint32_t *end_flush_addr;

	bool advance_interrupt;
#endif
}
BGRC_P_Handle;

/***************************************************************************/
#define BGRC_P_WATCHDOG_CNTR_OFF    0
#define BGRC_P_WATCHDOG_CNTR_FIRE   1
#define BGRC_P_WATCHDOG_CNTR_MAX    6

/***************************************************************************/
#define BCHP_M2MC_LIST_PACKET_HEADER_1         BCHP_M2MC_SCRATCH_LIST

/***************************************************************************/
#ifdef BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_1                            BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_MASK           BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_FORMAT_TYPE_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_SHIFT          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_FORMAT_TYPE_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_1_CH0_NUM_BITS_MASK          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_CH0_NUM_BITS_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_1_CH1_NUM_BITS_MASK          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_CH1_NUM_BITS_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_1_CH2_NUM_BITS_MASK          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_CH2_NUM_BITS_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_1_CH3_NUM_BITS_MASK          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_CH3_NUM_BITS_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT         BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT         BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT         BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT         BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_2                            BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_2
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_2_CH0_LSB_POS_MASK           BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_2_CH0_LSB_POS_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_2_CH1_LSB_POS_MASK           BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_2_CH1_LSB_POS_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_2_CH2_LSB_POS_MASK           BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_2_CH2_LSB_POS_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_2_CH3_LSB_POS_MASK           BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_2_CH3_LSB_POS_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_2_CH0_LSB_POS_SHIFT          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_2_CH0_LSB_POS_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_2_CH1_LSB_POS_SHIFT          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_2_CH1_LSB_POS_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_2_CH2_LSB_POS_SHIFT          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_2_CH2_LSB_POS_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_2_CH3_LSB_POS_SHIFT          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_2_CH3_LSB_POS_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3                            BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CH0_DISABLE_MASK           BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CH0_DISABLE_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CH1_DISABLE_MASK           BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CH1_DISABLE_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CH2_DISABLE_MASK           BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CH2_DISABLE_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CH3_DISABLE_MASK           BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CH3_DISABLE_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CH0_DISABLE_SHIFT          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CH0_DISABLE_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CH1_DISABLE_SHIFT          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CH1_DISABLE_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CH2_DISABLE_SHIFT          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CH2_DISABLE_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CH3_DISABLE_SHIFT          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CH3_DISABLE_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_PALETTE_BYPASS_MASK        BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_PALETTE_BYPASS_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_PALETTE_BYPASS_SHIFT       BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_PALETTE_BYPASS_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_PALETTE_BYPASS_DONT_LOOKUP BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_PALETTE_BYPASS_DONT_LOOKUP
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_PALETTE_BYPASS_LOOKUP      BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_PALETTE_BYPASS_LOOKUP
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CHROMA_FILTER_MASK         BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CHROMA_FILTER_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CHROMA_FILTER_REPLICATE    BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CHROMA_FILTER_REPLICATE
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CHROMA_FILTER_FILTER       BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CHROMA_FILTER_FILTER
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_CHROMA_FILTER_SHIFT        BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_CHROMA_FILTER_SHIFT
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_ZERO_PAD_MASK              BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_ZERO_PAD_MASK
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_ZERO_PAD_ZERO_PAD          BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_ZERO_PAD_ZERO_PAD
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_ZERO_PAD_REPLICATE         BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_ZERO_PAD_REPLICATE
#define BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_ZERO_PAD_SHIFT             BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_ZERO_PAD_SHIFT
#define BCHP_M2MC_BLIT_SRC_TOP_LEFT                                   BCHP_M2MC_BLIT_SRC_TOP_LEFT_0
#define BCHP_M2MC_BLIT_SRC_TOP_LEFT_LEFT_MASK                         BCHP_M2MC_BLIT_SRC_TOP_LEFT_0_LEFT_MASK
#define BCHP_M2MC_BLIT_SRC_TOP_LEFT_LEFT_SHIFT                        BCHP_M2MC_BLIT_SRC_TOP_LEFT_0_LEFT_SHIFT
#define BCHP_M2MC_BLIT_SRC_TOP_LEFT_TOP_MASK                          BCHP_M2MC_BLIT_SRC_TOP_LEFT_0_TOP_MASK
#define BCHP_M2MC_BLIT_SRC_TOP_LEFT_TOP_SHIFT                         BCHP_M2MC_BLIT_SRC_TOP_LEFT_0_TOP_SHIFT
#define BCHP_M2MC_BLIT_SRC_SIZE                                       BCHP_M2MC_BLIT_SRC_SIZE_0
#define BCHP_M2MC_BLIT_SRC_SIZE_SURFACE_WIDTH_MASK                    BCHP_M2MC_BLIT_SRC_SIZE_0_SURFACE_WIDTH_MASK
#define BCHP_M2MC_BLIT_SRC_SIZE_SURFACE_WIDTH_SHIFT                   BCHP_M2MC_BLIT_SRC_SIZE_0_SURFACE_WIDTH_SHIFT
#define BCHP_M2MC_BLIT_SRC_SIZE_SURFACE_HEIGHT_MASK                   BCHP_M2MC_BLIT_SRC_SIZE_0_SURFACE_HEIGHT_MASK
#define BCHP_M2MC_BLIT_SRC_SIZE_SURFACE_HEIGHT_SHIFT                  BCHP_M2MC_BLIT_SRC_SIZE_0_SURFACE_HEIGHT_SHIFT
#define BCHP_M2MC_BLIT_INPUT_STRIPE_WIDTH                             BCHP_M2MC_BLIT_INPUT_STRIPE_WIDTH_0
#define BCHP_M2MC_BLIT_INPUT_STRIPE_WIDTH_STRIPE_WIDTH_MASK           BCHP_M2MC_BLIT_INPUT_STRIPE_WIDTH_0_STRIPE_WIDTH_MASK
#define BCHP_M2MC_BLIT_INPUT_STRIPE_WIDTH_STRIPE_WIDTH_SHIFT          BCHP_M2MC_BLIT_INPUT_STRIPE_WIDTH_0_STRIPE_WIDTH_SHIFT
#define BCHP_M2MC_BLIT_STRIPE_OVERLAP                                 BCHP_M2MC_BLIT_STRIPE_OVERLAP_0
#define BCHP_M2MC_BLIT_STRIPE_OVERLAP_STRIPE_WIDTH_MASK               BCHP_M2MC_BLIT_STRIPE_OVERLAP_0_STRIPE_WIDTH_MASK
#define BCHP_M2MC_BLIT_STRIPE_OVERLAP_STRIPE_WIDTH_SHIFT              BCHP_M2MC_BLIT_STRIPE_OVERLAP_0_STRIPE_WIDTH_SHIFT
#define BCHP_M2MC_HORIZ_SCALER_INITIAL_PHASE                          BCHP_M2MC_HORIZ_SCALER_0_INITIAL_PHASE
#define BCHP_M2MC_HORIZ_SCALER_INITIAL_PHASE_PHASE_MASK               BCHP_M2MC_HORIZ_SCALER_0_INITIAL_PHASE_PHASE_MASK
#define BCHP_M2MC_HORIZ_SCALER_INITIAL_PHASE_PHASE_SHIFT              BCHP_M2MC_HORIZ_SCALER_0_INITIAL_PHASE_PHASE_SHIFT
#define BCHP_M2MC_HORIZ_SCALER_STEP                                   BCHP_M2MC_HORIZ_SCALER_0_STEP
#define BCHP_M2MC_HORIZ_SCALER_STEP_STEP_MASK                         BCHP_M2MC_HORIZ_SCALER_0_STEP_STEP_MASK
#define BCHP_M2MC_HORIZ_SCALER_STEP_STEP_SHIFT                        BCHP_M2MC_HORIZ_SCALER_0_STEP_STEP_SHIFT
#define BCHP_M2MC_VERT_SCALER_INITIAL_PHASE                           BCHP_M2MC_VERT_SCALER_0_INITIAL_PHASE
#define BCHP_M2MC_VERT_SCALER_INITIAL_PHASE_PHASE_MASK                BCHP_M2MC_VERT_SCALER_0_INITIAL_PHASE_PHASE_MASK
#define BCHP_M2MC_VERT_SCALER_INITIAL_PHASE_PHASE_SHIFT               BCHP_M2MC_VERT_SCALER_0_INITIAL_PHASE_PHASE_SHIFT
#define BCHP_M2MC_VERT_SCALER_STEP                                    BCHP_M2MC_VERT_SCALER_0_STEP
#define BCHP_M2MC_VERT_SCALER_STEP_STEP_MASK                          BCHP_M2MC_VERT_SCALER_0_STEP_STEP_MASK
#define BCHP_M2MC_VERT_SCALER_STEP_STEP_SHIFT                         BCHP_M2MC_VERT_SCALER_0_STEP_STEP_SHIFT
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_01_COEFF_0_MASK              BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE0_01_COEFF_0_MASK
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_01_COEFF_1_MASK              BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE0_01_COEFF_1_MASK
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_MASK               BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE0_2_COEFF_2_MASK
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE1_01_COEFF_0_MASK              BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE1_01_COEFF_0_MASK
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE1_01_COEFF_1_MASK              BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE1_01_COEFF_1_MASK
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE1_2_COEFF_2_MASK               BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE1_2_COEFF_2_MASK
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_01_COEFF_0_SHIFT             BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE0_01_COEFF_0_SHIFT
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_01_COEFF_1_SHIFT             BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE0_01_COEFF_1_SHIFT
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT              BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE0_2_COEFF_2_SHIFT
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE1_01_COEFF_0_SHIFT             BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE1_01_COEFF_0_SHIFT
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE1_01_COEFF_1_SHIFT             BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE1_01_COEFF_1_SHIFT
#define BCHP_M2MC_HORIZ_FIR_COEFF_PHASE1_2_COEFF_2_SHIFT              BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE1_2_COEFF_2_SHIFT

#define BCHP_M2MC_1_SRC_SURFACE_FORMAT_DEF_1                          BCHP_M2MC_1_SRC_SURFACE_0_FORMAT_DEF_1
#define BCHP_M2MC_1_SRC_SURFACE_FORMAT_DEF_2                          BCHP_M2MC_1_SRC_SURFACE_0_FORMAT_DEF_2
#define BCHP_M2MC_1_SRC_SURFACE_FORMAT_DEF_3                          BCHP_M2MC_1_SRC_SURFACE_0_FORMAT_DEF_3
#define BCHP_M2MC_1_BLIT_SRC_TOP_LEFT                                 BCHP_M2MC_1_BLIT_SRC_TOP_LEFT_0
#define BCHP_M2MC_1_BLIT_SRC_SIZE                                     BCHP_M2MC_1_BLIT_SRC_SIZE_0
#define BCHP_M2MC_1_BLIT_INPUT_STRIPE_WIDTH                           BCHP_M2MC_1_BLIT_INPUT_STRIPE_WIDTH_0
#define BCHP_M2MC_1_BLIT_STRIPE_OVERLAP                               BCHP_M2MC_1_BLIT_STRIPE_OVERLAP_0
#define BCHP_M2MC_1_HORIZ_SCALER_INITIAL_PHASE                        BCHP_M2MC_1_HORIZ_SCALER_0_INITIAL_PHASE
#define BCHP_M2MC_1_HORIZ_SCALER_STEP                                 BCHP_M2MC_1_HORIZ_SCALER_0_STEP
#define BCHP_M2MC_1_VERT_SCALER_INITIAL_PHASE                         BCHP_M2MC_1_VERT_SCALER_0_INITIAL_PHASE
#define BCHP_M2MC_1_VERT_SCALER_STEP                                  BCHP_M2MC_1_VERT_SCALER_0_STEP

#define BCHP_M2MC1_SRC_SURFACE_FORMAT_DEF_1                            BCHP_M2MC1_SRC_SURFACE_0_FORMAT_DEF_2
#define BCHP_M2MC1_SRC_SURFACE_FORMAT_DEF_2                            BCHP_M2MC1_SRC_SURFACE_0_FORMAT_DEF_1
#define BCHP_M2MC1_SRC_SURFACE_FORMAT_DEF_3                            BCHP_M2MC1_SRC_SURFACE_0_FORMAT_DEF_3
#define BCHP_M2MC1_BLIT_SRC_TOP_LEFT                                   BCHP_M2MC1_BLIT_SRC_TOP_LEFT_0
#define BCHP_M2MC1_BLIT_SRC_SIZE                                       BCHP_M2MC1_BLIT_SRC_SIZE_0
#define BCHP_M2MC1_BLIT_INPUT_STRIPE_WIDTH                             BCHP_M2MC1_BLIT_INPUT_STRIPE_WIDTH_0
#define BCHP_M2MC1_BLIT_STRIPE_OVERLAP                                 BCHP_M2MC1_BLIT_STRIPE_OVERLAP_0
#define BCHP_M2MC1_HORIZ_SCALER_INITIAL_PHASE                          BCHP_M2MC1_HORIZ_SCALER_0_INITIAL_PHASE
#define BCHP_M2MC1_HORIZ_SCALER_STEP                                   BCHP_M2MC1_HORIZ_SCALER_0_STEP
#define BCHP_M2MC1_VERT_SCALER_INITIAL_PHASE                           BCHP_M2MC1_VERT_SCALER_0_INITIAL_PHASE
#define BCHP_M2MC1_VERT_SCALER_STEP                                    BCHP_M2MC1_VERT_SCALER_0_STEP
#endif

/***************************************************************************/
#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_67)
#define BGRC_P_FIR_PHASE_COUNT           8
#define BGRC_P_FIR_TAP_COUNT             8
#define BGRC_P_FIR_FRAC_BITS             10
#define BGRC_P_FIR_OVERLAP_MIN           4
#else
#define BGRC_P_FIR_PHASE_COUNT           2
#define BGRC_P_FIR_TAP_COUNT             4
#define BGRC_P_FIR_FRAC_BITS             8
#define BGRC_P_FIR_OVERLAP_MIN           3
#endif
#define BGRC_P_FIR_FRAC_SCALE            (1 << BGRC_P_FIR_FRAC_BITS)

#ifdef BCHP_M2MC_HORIZ_SCALER_0_STEP_reserved0_SHIFT
#define BGRC_P_SCALER_STEP_FRAC_BITS     (BCHP_M2MC_HORIZ_SCALER_0_STEP_reserved0_SHIFT - 4)
#else
#define BGRC_P_SCALER_STEP_FRAC_BITS     (BCHP_M2MC_HORIZ_SCALER_STEP_reserved0_SHIFT - 4)
#endif

#define BGRC_P_SCALER_STEP_FRAC_MASK     ((1 << BGRC_P_SCALER_STEP_FRAC_BITS) - 1)

#define BGRC_P_AVERAGER_COEFF_FRAC_BITS  19
#define BGRC_P_AVERAGER_COEFF_FRAC_MASK  ((1 << BGRC_P_AVERAGER_COEFF_FRAC_BITS) - 1)

#if (BCHP_CHIP==7630)
#define BGRC_P_STRIP_WIDTH_MAX           120
#else
#define BGRC_P_STRIP_WIDTH_MAX           128
#endif

#define BGRC_P_MATRIX_FRAC_BITS          10

#define BGRC_P_SCALE_DOWN_MAX            15
#define BGRC_P_SCALE_DOWN_MAX_Y          7

#if (BCHP_M2MC_SRC_CM_C04_CM_C04_MASK & 0xF000)
#define BGRC_P_MATRIX_ADD_FRAC_BITS      4
#else
#define BGRC_P_MATRIX_ADD_FRAC_BITS      0
#endif

#define BGRC_P_YCbCr420_STRIP_WIDTH      64

#define BGRC_P_SCALER_STEP_TO_STRIPE_WIDTH_SHIFT    (BGRC_P_SCALER_STEP_FRAC_BITS - 16)

/***************************************************************************/
#define BGRC_P_MIN( v0, v1 )        (((v0) < (v1)) ? (v0) : (v1))
#define BGRC_P_MAX( v0, v1 )        (((v0) > (v1)) ? (v0) : (v1))
#define BGRC_P_CLAMP( v, mn, mx )   BGRC_P_MIN(BGRC_P_MAX(v, mn), mx)

/***************************************************************************/
#define BGRC_P_REG_INDEX( reg ) ((BCHP_M2MC_##reg - BCHP_M2MC_SCRATCH_LIST) >> 2)

/***************************************************************************/
#define BGRC_P_GET_FIELD_DATA( reg, field ) \
	((hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] & \
		(BCHP_M2MC_##reg##_##field##_MASK)) >> \
		(BCHP_M2MC_##reg##_##field##_SHIFT))

/***************************************************************************/
#define BGRC_P_COMPARE_FIELD( reg, field, flag ) \
	(((hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] & \
		(BCHP_M2MC_##reg##_##field##_MASK)) >> \
		(BCHP_M2MC_##reg##_##field##_SHIFT)) == \
		(BCHP_M2MC_##reg##_##field##_##flag))

/***************************************************************************/
#define BGRC_P_COMPARE_VALUE( reg, field, value ) \
	(((hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] & \
		(BCHP_M2MC_##reg##_##field##_MASK)) >> \
		(BCHP_M2MC_##reg##_##field##_SHIFT)) == value)

/***************************************************************************/
#define BGRC_P_SET_FIELD_FULL( reg, value ) \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] = ((uint32_t) (value))

/***************************************************************************/
#define BGRC_P_SET_FIELD_DATA( reg, field, value ) \
{ \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] &= \
		(~(BCHP_M2MC_##reg##_##field##_MASK)); \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] |= \
		(((uint32_t) (value)) << \
		(BCHP_M2MC_##reg##_##field##_SHIFT)) & \
		(BCHP_M2MC_##reg##_##field##_MASK); \
}

/***************************************************************************/
#define BGRC_P_SET_FIELD_ENUM( reg, field, flag ) \
{ \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] &= \
		(~(BCHP_M2MC_##reg##_##field##_MASK)); \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] |= \
		(BCHP_M2MC_##reg##_##field##_##flag << \
		(BCHP_M2MC_##reg##_##field##_SHIFT)) & \
		(BCHP_M2MC_##reg##_##field##_MASK); \
}

/***************************************************************************/
#define BGRC_P_SET_FIELD_COMP( reg, field, flag_true, flag_false, comp ) \
{ \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] &= \
		(~(BCHP_M2MC_##reg##_##field##_MASK)); \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] |= (((comp) ? \
		(BCHP_M2MC_##reg##_##field##_##flag_true) : \
		(BCHP_M2MC_##reg##_##field##_##flag_false)) << \
		(BCHP_M2MC_##reg##_##field##_SHIFT)) & \
		(BCHP_M2MC_##reg##_##field##_MASK); \
}

/***************************************************************************/
#define BGRC_P_SET_FIELD_COMP_DATA( reg, field, data_true, data_false, comp ) \
{ \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] &= \
		(~(BCHP_M2MC_##reg##_##field##_MASK)); \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] |= (((comp) ? \
		(data_true) : (data_false)) << \
		(BCHP_M2MC_##reg##_##field##_SHIFT)) & \
		(BCHP_M2MC_##reg##_##field##_MASK); \
}

/***************************************************************************/
#define BGRC_P_LOAD_LIST_GRP( reg, field, flag, pState, pulRegs ) \
{ \
	if( ((pulRegs)[BGRC_P_REG_INDEX(reg)] & BCHP_M2MC_##reg##_##field##_MASK) == 0 ) \
	{ \
		(pulRegs)[BGRC_P_REG_INDEX(reg)] &= \
			(~(BCHP_M2MC_##reg##_##field##_MASK)); \
		(pulRegs)[BGRC_P_REG_INDEX(reg)] |= \
			(BCHP_M2MC_##reg##_##field##_##flag << \
			(BCHP_M2MC_##reg##_##field##_SHIFT)) & \
			(BCHP_M2MC_##reg##_##field##_MASK); \
	} \
}

/***************************************************************************/
#define BGRC_P_SET_FIELD_FORMAT( reg, field, format, alpha, bypass ) \
{ \
	if( BPXL_IS_YCbCr444_10BIT_FORMAT(format) ) \
	{ \
		/* YCbCr444 10-bit */ \
		BGRC_P_SET_FIELD_DATA( reg##_SURFACE_FORMAT_DEF_1, field, 8 ); \
	} \
	else if( BPXL_IS_YCbCr420_FORMAT(format) ) \
	{ \
		/* YCbCr420 */ \
		BGRC_P_SET_FIELD_DATA( reg##_SURFACE_FORMAT_DEF_1, field, pState->bMacroBlockBigEndian ? 0 : 7 ); \
	} \
	else if( BPXL_IS_YCbCr422_10BIT_FORMAT(format) ) \
	{ \
		/* YCbCr422 10-bit */ \
		BGRC_P_SET_FIELD_DATA( reg##_SURFACE_FORMAT_DEF_1, field, 6 ); \
	} \
	else if( BPXL_IS_ALPHA_ONLY_FORMAT(format) ) \
	{ \
		/* ALPHA */ \
		BGRC_P_SET_FIELD_DATA( reg##_SURFACE_FORMAT_DEF_1, field, 5 ); \
	} \
	else if( BPXL_IS_YCbCr422_FORMAT(format) ) \
	{ \
		/* YCbCr422 */ \
		BGRC_P_SET_FIELD_DATA( reg##_SURFACE_FORMAT_DEF_1, field, 4 ); \
	} \
	else if( (!(bypass)) && BPXL_IS_PALETTE_FORMAT(format) ) \
	{ \
		/* PALETTE */ \
		BGRC_P_SET_FIELD_DATA( reg##_SURFACE_FORMAT_DEF_1, field, 3 ); \
	} \
	else if( (alpha) && \
		(BPXL_COMPONENT_SIZE(format, 3) == 0) && \
		(BPXL_COMPONENT_SIZE(format, 2) == 5) && \
		(BPXL_COMPONENT_SIZE(format, 1) == 6) && \
		(BPXL_COMPONENT_SIZE(format, 0) == 5) ) \
	{ \
		/* WRGB_1565 */ \
		BGRC_P_SET_FIELD_DATA( reg##_SURFACE_FORMAT_DEF_1, field, 2 ); \
	} \
	else if( BPXL_IS_WINDOW_FORMAT(format) || ( \
		(BPXL_COMPONENT_SIZE(format, 3) == 1) && \
		(BPXL_COMPONENT_SIZE(format, 2) == 5) && \
		(BPXL_COMPONENT_SIZE(format, 1) == 5) && \
		(BPXL_COMPONENT_SIZE(format, 0) == 5)) ) \
	{ \
		/* WRGB_1555 */ \
		BGRC_P_SET_FIELD_DATA( reg##_SURFACE_FORMAT_DEF_1, field, 1 ); \
	} \
	else \
	{ \
		/* OTHER */ \
		BGRC_P_SET_FIELD_DATA( reg##_SURFACE_FORMAT_DEF_1, field, 0 ); \
	} \
}

/***************************************************************************/
#define BGRC_P_SET_FIELD_CHANNELS( reg, format, surface ) \
{ \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_SURFACE_FORMAT_DEF_1)] &= ~( \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_1_CH0_NUM_BITS_MASK | \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_1_CH1_NUM_BITS_MASK | \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_1_CH2_NUM_BITS_MASK | \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_1_CH3_NUM_BITS_MASK); \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_SURFACE_FORMAT_DEF_2)] &= ~( \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_2_CH0_LSB_POS_MASK | \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_2_CH1_LSB_POS_MASK | \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_2_CH2_LSB_POS_MASK | \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_2_CH3_LSB_POS_MASK); \
	if( surface ) \
	{ \
		if( BPXL_IS_YCbCr444_10BIT_FORMAT(format) ) \
		{ \
			hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_SURFACE_FORMAT_DEF_1)] |= \
				(BPXL_COMPONENT_SIZE(format, 0) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT) | \
				(BPXL_COMPONENT_SIZE(format, 1) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT) | \
				(BPXL_COMPONENT_SIZE(format, 2) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT); \
			hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_SURFACE_FORMAT_DEF_2)] |= \
				(BPXL_COMPONENT_POS(format, 0) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_2_CH0_LSB_POS_SHIFT) | \
				(BPXL_COMPONENT_POS(format, 1) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_2_CH1_LSB_POS_SHIFT) | \
				(BPXL_COMPONENT_POS(format, 2) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_2_CH2_LSB_POS_SHIFT); \
		} \
		else \
		{ \
			hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_SURFACE_FORMAT_DEF_1)] |= \
				(BPXL_COMPONENT_SIZE(format, 0) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_1_CH0_NUM_BITS_SHIFT) | \
				(BPXL_COMPONENT_SIZE(format, 1) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_1_CH1_NUM_BITS_SHIFT) | \
				(BPXL_COMPONENT_SIZE(format, 2) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_1_CH2_NUM_BITS_SHIFT) | \
				(BPXL_COMPONENT_SIZE(format, 3) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_1_CH3_NUM_BITS_SHIFT); \
			hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_SURFACE_FORMAT_DEF_2)] |= \
				(BPXL_COMPONENT_POS(format, 0) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_2_CH0_LSB_POS_SHIFT) | \
				(BPXL_COMPONENT_POS(format, 1) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_2_CH1_LSB_POS_SHIFT) | \
				(BPXL_COMPONENT_POS(format, 2) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_2_CH2_LSB_POS_SHIFT) | \
				(BPXL_COMPONENT_POS(format, 3) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_2_CH3_LSB_POS_SHIFT); \
		} \
	} \
}

#define BGRC_P_SET_FIELD_CHANNEL_DISABLE( reg, format, surface ) \
{ \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_SURFACE_FORMAT_DEF_3)] &= ~( \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_3_CH0_DISABLE_MASK | \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_3_CH1_DISABLE_MASK | \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_3_CH2_DISABLE_MASK | \
		BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_3_CH3_DISABLE_MASK); \
	if( surface ) \
	{ \
		hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_SURFACE_FORMAT_DEF_3)] |= \
			((BPXL_HAS_MASKED_ALPHA(format) ? 1 : 0) << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_3_CH3_DISABLE_SHIFT); \
	} \
	else \
	{ \
		hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_SURFACE_FORMAT_DEF_3)] |= \
			(1 << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_3_CH0_DISABLE_SHIFT) | \
			(1 << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_3_CH1_DISABLE_SHIFT) | \
			(1 << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_3_CH2_DISABLE_SHIFT) | \
			(1 << BCHP_M2MC_##reg##_SURFACE_FORMAT_DEF_3_CH3_DISABLE_SHIFT); \
	} \
}

/***************************************************************************/
#define BGRC_P_GET_FIELD_MATRIX_ENTRY( entry, left, right ) \
	(((((entry) * (((entry) < 0) ? -1 : 1)) << (left)) >> (right)) * (((entry) < 0) ? -1 : 1))

/***************************************************************************/
#define BGRC_P_SET_FIELD_MATRIX_ROW( reg, row, matrix, index, shift ) \
{ \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_CM_C##row##0_C##row##1)] = \
		BCHP_FIELD_DATA(M2MC_##reg##_CM_C##row##0_C##row##1, CM_C##row##0, \
			BGRC_P_GET_FIELD_MATRIX_ENTRY( (matrix)[(index) + 0], BGRC_P_MATRIX_FRAC_BITS, shift ) & \
			BCHP_MASK(M2MC_##reg##_CM_C##row##0_C##row##1, CM_C##row##1)) | \
		BCHP_FIELD_DATA(M2MC_##reg##_CM_C##row##0_C##row##1, CM_C##row##1, \
			BGRC_P_GET_FIELD_MATRIX_ENTRY( (matrix)[(index) + 1], BGRC_P_MATRIX_FRAC_BITS, shift ) & \
			BCHP_MASK(M2MC_##reg##_CM_C##row##0_C##row##1, CM_C##row##1)); \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_CM_C##row##2_C##row##3)] = \
		BCHP_FIELD_DATA(M2MC_##reg##_CM_C##row##2_C##row##3, CM_C##row##2, \
			BGRC_P_GET_FIELD_MATRIX_ENTRY( (matrix)[(index) + 2], BGRC_P_MATRIX_FRAC_BITS, shift ) & \
			BCHP_MASK(M2MC_##reg##_CM_C##row##2_C##row##3, CM_C##row##3)) | \
		BCHP_FIELD_DATA(M2MC_##reg##_CM_C##row##2_C##row##3, CM_C##row##3, \
			BGRC_P_GET_FIELD_MATRIX_ENTRY( (matrix)[(index) + 3], BGRC_P_MATRIX_FRAC_BITS, shift ) & \
			BCHP_MASK(M2MC_##reg##_CM_C##row##2_C##row##3, CM_C##row##3)); \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg##_CM_C##row##4)] = \
		BCHP_FIELD_DATA(M2MC_##reg##_CM_C##row##4, CM_C##row##4, \
			BGRC_P_GET_FIELD_MATRIX_ENTRY( (matrix)[(index) + 4], BGRC_P_MATRIX_ADD_FRAC_BITS, shift ) & \
			BCHP_MASK(M2MC_##reg##_CM_C##row##4, CM_C##row##4)); \
}

/***************************************************************************/
#define BGRC_P_SET_FIELD_BLEND( reg, srca, srcb, srcc, srcd, srce, subcd, sube ) \
{ \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(BLEND_##reg##_OP)] = \
		(BGRC_P_GetBlendOp( srca ) << BCHP_M2MC_BLEND_##reg##_OP_OP_A_SHIFT) | \
		(BGRC_P_GetBlendOp( srcb ) << BCHP_M2MC_BLEND_##reg##_OP_OP_B_SHIFT) | \
		(BGRC_P_GetBlendOp( srcc ) << BCHP_M2MC_BLEND_##reg##_OP_OP_C_SHIFT) | \
		(BGRC_P_GetBlendOp( srcd ) << BCHP_M2MC_BLEND_##reg##_OP_OP_D_SHIFT) | \
		(BGRC_P_GetBlendOp( srce ) << BCHP_M2MC_BLEND_##reg##_OP_OP_E_SHIFT) | \
		(BGRC_P_GetBlendOpInv( srca ) << BCHP_M2MC_BLEND_##reg##_OP_OP_A_INV_SHIFT) | \
		(BGRC_P_GetBlendOpInv( srcb ) << BCHP_M2MC_BLEND_##reg##_OP_OP_B_INV_SHIFT) | \
		(BGRC_P_GetBlendOpInv( srcc ) << BCHP_M2MC_BLEND_##reg##_OP_OP_C_INV_SHIFT) | \
		(BGRC_P_GetBlendOpInv( srcd ) << BCHP_M2MC_BLEND_##reg##_OP_OP_D_INV_SHIFT) | \
		(BGRC_P_GetBlendOpInv( srce ) << BCHP_M2MC_BLEND_##reg##_OP_OP_E_INV_SHIFT) | \
		(((subcd) ? 1 : 0) << BCHP_M2MC_BLEND_##reg##_OP_SUBTRACT_CD_SHIFT) | \
		(((sube) ? 1 : 0) << BCHP_M2MC_BLEND_##reg##_OP_SUBTRACT_E_SHIFT); \
}

/***************************************************************************/
#define BGRC_P_SET_FIELD_COLORKEY( nsnd, nsd, snd, sd ) \
{ \
	hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(BLEND_COLOR_KEY_ACTION)] = \
		((nsnd) << BCHP_M2MC_BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_NOT_DEST_SHIFT) | \
		((nsd) << BCHP_M2MC_BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_DEST_SHIFT) | \
		((snd) << BCHP_M2MC_BLEND_COLOR_KEY_ACTION_ACTION_SRC_NOT_DEST_SHIFT) | \
		((sd) << BCHP_M2MC_BLEND_COLOR_KEY_ACTION_ACTION_SRC_DEST_SHIFT); \
}

/***************************************************************************/
#define BGRC_P_SET_FIELD_DIRECTION( surf0, surf1, surf2, dir, coord, fore, back ) \
{ \
	if( BGRC_P_GET_FIELD_DATA( BLIT_##surf0##_TOP_LEFT, coord ) < \
		BGRC_P_GET_FIELD_DATA( BLIT_##surf1##_TOP_LEFT, coord ) ) \
	{ \
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, surf0##_##dir##_DIRECTION, back ); \
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, surf1##_##dir##_DIRECTION, back ); \
		BGRC_P_SET_FIELD_COMP( BLIT_CTRL, surf2##_##dir##_DIRECTION, back, fore, \
			BGRC_P_COMPARE_FIELD( surf2##_FEEDER_ENABLE, ENABLE, ENABLE ) ); \
	} \
	else \
	{ \
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, surf0##_##dir##_DIRECTION, fore ); \
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, surf1##_##dir##_DIRECTION, fore ); \
		BGRC_P_SET_FIELD_ENUM( BLIT_CTRL, surf2##_##dir##_DIRECTION, fore ); \
	} \
}

/***************************************************************************/
#define BGRC_P_SURFACE_INTERSECT( surf0, surf1 ) (( \
	BGRC_P_GET_FIELD_DATA( surf0##_SURFACE_ADDR_0, ADDR ) == \
	BGRC_P_GET_FIELD_DATA( surf1##_SURFACE_ADDR_0, ADDR )) && (!((( \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf0##_TOP_LEFT, LEFT ) + \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf0##_SIZE, SURFACE_WIDTH )) < \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf1##_TOP_LEFT, LEFT )) || ( \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf0##_TOP_LEFT, LEFT ) > ( \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf1##_TOP_LEFT, LEFT ) + \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf1##_SIZE, SURFACE_WIDTH ))) || (( \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf0##_TOP_LEFT, TOP ) + \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf0##_SIZE, SURFACE_HEIGHT )) < \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf1##_TOP_LEFT, TOP )) || ( \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf0##_TOP_LEFT, TOP ) > ( \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf1##_TOP_LEFT, TOP ) + \
	BGRC_P_GET_FIELD_DATA( BLIT_##surf1##_SIZE, SURFACE_HEIGHT ))))))

/***************************************************************************/
#define BGRC_P_COPY_FIELD( pState, dst, src, type, reg, field ) \
{ \
	if( ((dst)[BGRC_P_REG_INDEX(reg)] & (BCHP_M2MC_##reg##_##field##_MASK)) != \
		((src)[BGRC_P_REG_INDEX(reg)] & (BCHP_M2MC_##reg##_##field##_MASK)) ) \
	{ \
		(dst)[BGRC_P_REG_INDEX(reg)] &= \
			(~(BCHP_M2MC_##reg##_##field##_MASK)); \
		(dst)[BGRC_P_REG_INDEX(reg)] |= (src)[BGRC_P_REG_INDEX(reg)] & \
			(BCHP_M2MC_##reg##_##field##_MASK); \
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, type, GRP_ENABLE, pState, dst ); \
	} \
}

/***************************************************************************/
#define BGRC_P_COPY_REGISTER( pState, dst, src, type, reg ) \
{ \
	if( (dst)[BGRC_P_REG_INDEX(reg)] != (src)[BGRC_P_REG_INDEX(reg)]) \
	{ \
		(dst)[BGRC_P_REG_INDEX(reg)] = (src)[BGRC_P_REG_INDEX(reg)]; \
		BGRC_P_LOAD_LIST_GRP( LIST_PACKET_HEADER_1, type, GRP_ENABLE, pState, dst ); \
	} \
}

/***************************************************************************/
#define BGRC_P_REGISTER_CHANGED( reg ) \
	(hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)] != hGrc->aulActualRegs[BGRC_P_REG_INDEX(reg)])

/***************************************************************************/
#if ((BCHP_CHIP==7400) && (BCHP_VER == BCHP_VER_A0)) || \
	((BCHP_CHIP==7438) && (BCHP_VER >= BCHP_VER_A0)) || \
	((BCHP_CHIP==7440) && (BCHP_VER >= BCHP_VER_A0))
#define BGRC_P_READ_REG( reg ) \
	(uint32_t) BREG_Read32( hGrc->hRegister, (hGrc->ulDeviceNum ? BCHP_M2MC_1_##reg : BCHP_M2MC_##reg) )
#define BGRC_P_WRITE_REG( reg, val ) \
	BREG_Write32( hGrc->hRegister, (hGrc->ulDeviceNum ? BCHP_M2MC_1_##reg : BCHP_M2MC_##reg), (val) )
#elif (BCHP_CHIP==7435)
#define BGRC_P_READ_REG( reg ) \
	(uint32_t) BREG_Read32( hGrc->hRegister, (hGrc->ulDeviceNum ? BCHP_M2MC1_##reg : BCHP_M2MC_##reg) )
#define BGRC_P_WRITE_REG( reg, val ) \
	BREG_Write32( hGrc->hRegister, (hGrc->ulDeviceNum ? BCHP_M2MC1_##reg : BCHP_M2MC_##reg), (val) )
#else
#define BGRC_P_READ_REG( reg ) \
	(uint32_t) BREG_Read32( hGrc->hRegister, BCHP_M2MC_##reg )
#define BGRC_P_WRITE_REG( reg, val ) \
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_##reg, (val) )
#endif

/***************************************************************************/
#define BGRC_P_SET_REGISTER( reg ) \
{ \
	hGrc->aulActualRegs[BGRC_P_REG_INDEX(reg)] = hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)]; \
	*pulPacket++ = hGrc->aulCurrentRegs[BGRC_P_REG_INDEX(reg)]; \
}

/***************************************************************************/
#define BGRC_P_SET_REGISTERS( group, reg ) \
{ \
	uint32_t ulIndex = BGRC_P_REG_INDEX(reg); \
	uint32_t *pulActualRegs = &hGrc->aulActualRegs[ulIndex]; \
	uint32_t *pulCurrentRegs = &hGrc->aulCurrentRegs[ulIndex]; \
	uint32_t ii; \
	for( ii = 0; ii < BGRC_P_LIST_##group##_GRP_CNTRL_COUNT; ++ii ) \
	{ \
		*pulActualRegs++ = *pulCurrentRegs; \
		*pulPacket++ = *pulCurrentRegs++; \
	} \
}

/***************************************************************************/
#define BGRC_P_SET_PALETTE_ENTRY( reg, num, value ) \
	pOp->aulPaletteEntries[(num)] = (value)

/***************************************************************************/
#define BGRC_P_SET_PALETTE( reg, surface, count ) \
{ \
	BSUR_Palette_Handle hPalette; \
	uint32_t *pulEntries; \
	uint32_t ii; \
\
	BSUR_Surface_GetPalette( (surface), &hPalette ); \
	if( hPalette ) \
	{ \
		BSUR_Palette_GetAddress( hPalette, (void *) (&pulEntries) ); \
		if( pulEntries ) \
		{ \
			for( ii = 0; ii < (count); ++ii ) \
				BGRC_P_SET_PALETTE_ENTRY( reg, ii, pulEntries[ii] ); \
		} \
	} \
}

/***************************************************************************/
#define BGRC_P_VALIDATE_SURFACE_DIMENSIONS( x, y, width, height ) ( \
	(!(((x) || (y)) && (((width) == 0) || ((height) == 0)))) && \
	(((int32_t) (x) >= 0) && ((int32_t) (y) >= 0)) )

/***************************************************************************/
#define BGRC_P_VALIDATE_SURFACE_RECTANGLE( rect ) \
	BGRC_P_VALIDATE_SURFACE_DIMENSIONS( (rect)->ulX, (rect)->ulY, (rect)->ulWidth, (rect)->ulHeight )

/***************************************************************************/
#define BGRC_P_VALIDATE_SURFACE_BOUNDS( state, sur ) \
	(!((state)->sur##Surface.hSurface && ( \
	(((state)->sur##Surface.ulX + (state)->sur##Surface.ulWidth) > (state)->sur##Surface.ulSurfaceWidth) || \
	(((state)->sur##Surface.ulY + (state)->sur##Surface.ulHeight) > (state)->sur##Surface.ulSurfaceHeight))))

/***************************************************************************/
#define BGRC_P_GET_SURFACE_DATA( newsurface, newdata, olddata ) \
{ \
	BSUR_Palette_Handle hPalette = 0; \
	BPXL_Format eFormat = 0; \
	uint32_t ulPaletteOffset = 0; \
	uint32_t ulSurfaceID = 0; \
	if( (newsurface) ) \
	{ \
		if( BSUR_Surface_GetID( (newsurface), &ulSurfaceID ) != BERR_SUCCESS ) \
			return BERR_TRACE(BERR_INVALID_PARAMETER); \
		if( ulSurfaceID == 0 ) \
		{ \
			ulSurfaceID = ++hGrc->ulSurfaceID; \
			if( BSUR_Surface_SetID( (newsurface), ulSurfaceID ) != BERR_SUCCESS ) \
				return BERR_TRACE(BERR_INVALID_PARAMETER); \
		} \
		if( BSUR_Surface_GetFormat( (newsurface), &eFormat ) != BERR_SUCCESS ) \
			return BERR_TRACE(BERR_INVALID_PARAMETER); \
		if( BPXL_IS_PALETTE_FORMAT( eFormat ) ) \
		{ \
			if( BSUR_Surface_GetPalette( (newsurface), &hPalette ) != BERR_SUCCESS ) \
				return BERR_TRACE(BERR_INVALID_PARAMETER); \
			if( BSUR_Palette_GetOffset( hPalette, &ulPaletteOffset ) != BERR_SUCCESS ) \
				return BERR_TRACE(BERR_INVALID_PARAMETER); \
		} \
	} \
	if( hGrc->bUninitialized || ((newsurface) != (olddata).hSurface) || \
		(ulSurfaceID != (olddata).ulID) || (ulPaletteOffset != (olddata).ulPaletteOffset) ) \
	{ \
		bSurfaceChanged = true; \
		BKNI_Memset( &(newdata), 0, sizeof (BGRC_P_Surface) ); \
		if( (newsurface) ) \
		{ \
			(newdata).ulID = ulSurfaceID; \
			(newdata).hSurface = (newsurface); \
			(newdata).eFormat = eFormat; \
			if( BSUR_Surface_GetAddress( (newsurface), &((newdata).pMemory), &((newdata).ulPitch) ) != BERR_SUCCESS ) \
				return BERR_TRACE(BERR_INVALID_PARAMETER); \
			if( BSUR_Surface_GetDimensions( (newsurface), &((newdata).ulSurfaceWidth), &((newdata).ulSurfaceHeight) ) != BERR_SUCCESS ) \
				return BERR_TRACE(BERR_INVALID_PARAMETER); \
			if( BSUR_Surface_GetOffset( (newsurface), &((newdata).ulOffset) ) != BERR_SUCCESS ) \
				return BERR_TRACE(BERR_INVALID_PARAMETER); \
			if( BPXL_IS_PALETTE_FORMAT((newdata).eFormat) ) \
			{ \
				(newdata).ulPaletteOffset = ulPaletteOffset; \
				(newdata).ulPaletteEntries = (uint32_t) BPXL_NUM_PALETTE_ENTRIES((newdata).eFormat); \
			} \
		} \
	} \
}

/***************************************************************************/
#define BGRC_P_SET_SURFACE_DIMENSIONS( reg, surface, xx, yy, width, height ) \
{ \
	if( (surface) ) \
	{ \
		uint32_t ulWidth = (width); \
		uint32_t ulHeight = (height); \
		uint32_t ulTemp; \
		if( (ulWidth == 0) || (ulHeight == 0) ) \
			BSUR_Surface_GetDimensions( (surface), \
				(ulWidth == 0) ? (&ulWidth) : (&ulTemp), \
				(ulHeight == 0) ? (&ulHeight) : (&ulTemp) ); \
		BGRC_P_SET_FIELD_DATA( BLIT_##reg##_TOP_LEFT, LEFT, (xx) ); \
		BGRC_P_SET_FIELD_DATA( BLIT_##reg##_TOP_LEFT, TOP, (yy) ); \
		BGRC_P_SET_FIELD_DATA( BLIT_##reg##_SIZE, SURFACE_WIDTH, ulWidth ); \
		BGRC_P_SET_FIELD_DATA( BLIT_##reg##_SIZE, SURFACE_HEIGHT, ulHeight ); \
		if( BGRC_P_REGISTER_CHANGED( BLIT_##reg##_SIZE ) || \
			BGRC_P_REGISTER_CHANGED( BLIT_##reg##_TOP_LEFT ) ) \
			BGRC_P_SET_FIELD_ENUM( LIST_PACKET_HEADER_1, BLIT_GRP_CNTRL, GRP_ENABLE ); \
	} \
}

/***************************************************************************/
#define BGRC_P_ALIGN_PATTERN( pout, pin, xx, yy ) \
{ \
	uint32_t ii; \
	for( ii = 0; ii < 8; ++ii ) \
		((uint8_t *) (pout))[ii] = ((uint8_t *) (pin))[(ii + (yy)) & 7]; \
	for( ii = 0; ii < 8; ++ii ) \
		((uint8_t *) (pout))[ii] = (uint8_t) ((((uint8_t *) (pout))[ii] >> (8 - ((xx) & 7))) | \
			(((uint8_t *) (pout))[ii] << ((xx) & 7))); \
}

/***************************************************************************/
#define BGRC_P_WRITE_REGISTER( reg ) \
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_##reg, pOp->aulRegs[BGRC_P_REG_INDEX(reg)] )

#define BGRC_P_WRITE_REGISTER_COEFF( reg, num ) \
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_##reg##_FIR_COEFF_PHASE0_01 + (num) * 4, \
		pOp->aulRegs[BGRC_P_REG_INDEX(reg##_FIR_COEFF_PHASE0_01) + (num)] );

#define BGRC_P_WRITE_REGISTER_ENTRY( reg, num ) \
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_##reg##_CLUT_ENTRY_i_ARRAY_BASE + (num) * 4, \
		pOp->aulPaletteEntries[(num)] );

/****************************************************************************/
uint32_t BGRC_P_GetBlendOp( BGRC_Blend_Source eSource );
uint32_t BGRC_P_GetBlendOpInv( BGRC_Blend_Source eSource );

BERR_Code BGRC_P_Blit( BGRC_Handle hGrc, BGRC_Callback pCallback, void *pData, bool bSetEvent );
BERR_Code BGRC_P_FilterBlit( BGRC_Handle hGrc, BGRC_Callback pCallback, void *pData, bool bSetEvent );

void BGRC_P_Source_CopyState( BGRC_P_State *pDstState, BGRC_P_State *pSrcState, uint32_t aulDstRegs[], uint32_t aulSrcRegs[] );
void BGRC_P_Destination_CopyState( BGRC_P_State *pDstState, BGRC_P_State *pSrcState, uint32_t aulDstRegs[], uint32_t aulSrcRegs[] );
void BGRC_P_Pattern_CopyState( BGRC_P_State *pDstState, BGRC_P_State *pSrcState, uint32_t aulDstRegs[], uint32_t aulSrcRegs[] );
void BGRC_P_Blend_CopyState( BGRC_P_State *pDstState, BGRC_P_State *pSrcState, uint32_t aulDstRegs[], uint32_t aulSrcRegs[] );
void BGRC_P_Output_CopyState( BGRC_P_State *pDstState, BGRC_P_State *pSrcState, uint32_t aulDstRegs[], uint32_t aulSrcRegs[] );
void BGRC_P_PrintRegisters( BGRC_Handle hGrc );

bool BGRC_P_List_InitPacketMemory( BGRC_Handle hGrc, uint32_t ulMemorySize );
void BGRC_P_List_FreePacketMemory( BGRC_Handle hGrc );
void BGRC_P_List_CleanupPacketMemory( BGRC_Handle hGrc );
void BGRC_P_List_PacketIsr( void *pvParam1, int iParam2 );

void BGRC_P_Operation_FreeAll( BGRC_Handle hGrc );
void BGRC_P_Operation_CleanupList( BGRC_Handle hGrc );
bool BGRC_P_Operation_Prealloc( BGRC_Handle hGrc, uint32_t ulCount );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BGRC_PRIVATE_H__ */

/* end of file */
