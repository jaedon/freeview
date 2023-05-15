/***************************************************************************
 *     Copyright (c) 2009-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgrc_packet.c $
 * $brcm_Revision: Hydra_Software_Devel/108 $
 * $brcm_Date: 8/24/12 1:55p $
 *
 * Module Description: GRC Packet API
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/grc/7435/bgrc_packet.c $
 * 
 * Hydra_Software_Devel/108   8/24/12 1:55p nissen
 * SW7445-22: Added support for the dual M2MCs on the 7445.
 * 
 * Hydra_Software_Devel/107   8/22/12 12:31p nissen
 * SW7425-3714: Fixed coverity defects by checking if context is null.
 * 
 * Hydra_Software_Devel/106   8/21/12 12:24p nissen
 * SW7425-3625: Switched to using cached memory access.
 * 
 * Hydra_Software_Devel/105   8/2/12 2:26p nissen
 * SW7563-13: Added support for the 7563.
 * 
 * Hydra_Software_Devel/104   6/27/12 3:34p nissen
 * SW7231-777: Increased length of short delay.
 * 
 * Hydra_Software_Devel/103   6/26/12 5:55p nissen
 * SW7231-777: Added very short delay before checking M2MC status, because
 * it may not have started yet.
 * 
 * Hydra_Software_Devel/102   5/1/12 7:07p nissen
 * SW7584-11: Added more support for 7584.
 * 
 * Hydra_Software_Devel/101   5/1/12 1:03p nissen
 * SW7584-11: Added support or 7584.
 * 
 * Hydra_Software_Devel/100   2/9/12 2:25p nissen
 * SW7405-4810: Fixed BGRC_Packet_GetStatus function.
 * 
 * Hydra_Software_Devel/99   2/8/12 11:24a nissen
 * SW7405-3671: Moved device reset to AdvancePackets when there is a hang.
 * 
 * Hydra_Software_Devel/98   2/7/12 7:41p nissen
 * SW7405-3671: Fixed build error.
 * 
 * Hydra_Software_Devel/97   2/7/12 5:04p nissen
 * SW7405-3671: Added code to reset M2MC and restart packets when M2MC
 * hangs.
 * 
 * Hydra_Software_Devel/95   1/6/12 1:35p nissen
 * SW7435-13 : Fixed support for 7435 A0.
 * 
 * Hydra_Software_Devel/94   12/21/11 5:27p nissen
 * SW7360-5: Added support for the 7360 A0.
 * 
 * Hydra_Software_Devel/93   12/21/11 12:55p nissen
 * SW7405-3671: Added better check for when the M2MC is hung.
 * 
 * Hydra_Software_Devel/92   12/15/11 6:50p nissen
 * SW7435-13 : Added support for 7435 A0.
 * 
 * Hydra_Software_Devel/91   11/29/11 4:15p nissen
 * SW7425-1795: Changed to BMEM_Handle.
 * 
 * Hydra_Software_Devel/90   11/22/11 6:20p nissen
 * SW7425-1795: Added heap field to BGRC_PacketContext_CreateSettings
 * structure for packet memory allocations.
 * 
 * Hydra_Software_Devel/89   11/3/11 4:56p nissen
 * SWBLURAY-27238: Added support for 2nd M2MC for 7640 B0.
 * 
 * Hydra_Software_Devel/88   10/5/11 5:34p nissen
 * SW7425-248: Updated use of BDBG_OBJECTs.
 * 
 * Hydra_Software_Devel/87   10/5/11 12:58p nissen
 * SW7425-248: Removed BDBG_OBJECT support for the 7038.
 * 
 * Hydra_Software_Devel/86   9/23/11 12:31p nissen
 * SW7429-4: Added support for 7429.
 * 
 * Hydra_Software_Devel/85   9/17/11 7:17p nissen
 * SW7425-248: Added BDBG_OBJECT_SET.
 * 
 * Hydra_Software_Devel/84   9/17/11 7:06p nissen
 * SW7425-248: Added BDBG_OBJECT_ID.
 * 
 * Hydra_Software_Devel/83   9/17/11 6:31p nissen
 * SW7425-248: Added BDBG_OBJECT_ASSERT.
 * 
 * Hydra_Software_Devel/82   9/17/11 4:12p nissen
 * SW7335-1080: Added type conversion for parameter.
 * 
 * Hydra_Software_Devel/81   9/12/11 2:15p nissen
 * SWBLURAY-27238: Fixing support for 7640 B0.
 * 
 * Hydra_Software_Devel/80   9/6/11 4:36p nissen
 * SWBLURAY-27238: Added support for 7640 B0.
 * 
 * Hydra_Software_Devel/79   7/27/11 6:04p nissen
 * SW7405-3671: Added support for fixed scaling with 420 destriping.
 * 
 * Hydra_Software_Devel/78   6/28/11 2:23p nissen
 * SW7420-1893: Changed Sync so that it continues to try it's blit instead
 * of reporting out of memory.
 * 
 * Hydra_Software_Devel/77   6/23/11 10:18a nissen
 * SW7420-1893: Prevented sync blit frin over writting packet memory.
 * 
 * Hydra_Software_Devel/76   5/27/11 6:26p nissen
 * SW7346-149: Added packet writing functions, and fixed syncing a context
 * that has not been used yet.
 * 
 * Hydra_Software_Devel/75   4/5/11 5:36p jhaberf
 * SWDTV-5977: Added support for 35233 DTV chip
 * 
 * Hydra_Software_Devel/74   3/28/11 5:31p nissen
 * SW7420-1715: Fixed link list when creating context.
 * 
 * Hydra_Software_Devel/73   3/18/11 7:37p nissen
 * SW7038-3500: Fixing header size.
 * 
 * Hydra_Software_Devel/72   3/18/11 1:22p nissen
 * SW7420-1575: Added new width and height fields for surface planes.
 * 
 * Hydra_Software_Devel/71   3/7/11 2:16p nissen
 * SW7038-3500: Switched header size back.
 * 
 * Hydra_Software_Devel/70   3/5/11 4:51p nissen
 * SW7038-3500: Changed header size to fit filter table.
 * 
 * Hydra_Software_Devel/69   3/4/11 1:42p nissen
 * SW7038-3500: Fixed header size.
 * 
 * Hydra_Software_Devel/68   3/3/11 4:49p nissen
 * SW7038-3500: Fixed build error for 7038.
 * 
 * Hydra_Software_Devel/67   12/6/10 12:43p nissen
 * SW7420-1138: Added support for validating memory bounds used by blits.
 * 
 * Hydra_Software_Devel/66   12/3/10 11:46a nissen
 * SW7344-10: Added support for the 7344 and 7346.
 * 
 * Hydra_Software_Devel/65   11/26/10 4:23p jhaberf
 * SW35125-33: Added support for the 35125 DTV chip
 * 
 * Hydra_Software_Devel/64   11/25/10 12:27p nissen
 * SW7552-13: Fixed support for 7552.
 * 
 * Hydra_Software_Devel/63   11/24/10 9:31p nissen
 * SW7552-13: Added support for 7552.
 * 
 * Hydra_Software_Devel/62   11/19/10 12:51p nissen
 * SW7425-19: Fixed HW reset.
 * 
 * Hydra_Software_Devel/61   11/11/10 5:40p nissen
 * SW7420-1200: Added support for storing packets before submitting,
 * through a field in the context's create settings structure.
 * 
 * Hydra_Software_Devel/60   10/27/10 6:22p nissen
 * SW7405-4810: Fixed GetContextStatus to notify contexts need advancing
 * without an interrupt firing.
 * 
 * Hydra_Software_Devel/59   10/12/10 1:18p nissen
 * SW7231-9: Added support for the 7231.
 * 
 * Hydra_Software_Devel/58   10/12/10 12:54p nissen
 * SW7358-14: Added support for the 7358.
 * 
 * Hydra_Software_Devel/57   10/7/10 2:39p nissen
 * SW7425-19: Added support for the 7422 and 7425.
 * 
 * Hydra_Software_Devel/56   9/21/10 10:44a nissen
 * SW7405-3671: Fixed problem with size when getting packet memory.
 * 
 * Hydra_Software_Devel/55   9/20/10 4:23p nissen
 * SW7405-4869: Fixed Rop usage.
 * 
 * Hydra_Software_Devel/54   9/15/10 5:39p nissen
 * SW7420-1081: Switched to using BMEM_Alloc with cached memory when
 * allocating user packets.
 * 
 * Hydra_Software_Devel/53   9/3/10 4:16p nissen
 * SW7405-4810: Added BGRC_Packet_GetStatus function.
 * 
 * Hydra_Software_Devel/52   8/27/10 10:28a mward
 * SW7405-3671: Initialize m2mc_format to avoid a warning from some
 * compilers.
 * 
 * Hydra_Software_Devel/51   8/18/10 10:19p nissen
 * SW7405-3671: Only advancing and syncing packets when destroying a
 * context, if no callbacks set.
 * 
 * Hydra_Software_Devel/50   8/18/10 3:13p nissen
 * SW7405-3671: Checking if user overwrote submited packet memory.
 * 
 * Hydra_Software_Devel/49   8/9/10 11:29a nissen
 * SW7405-3671: Fixed HW fifo memory allocation.
 * 
 * Hydra_Software_Devel/48   8/4/10 8:35p nissen
 * SW7405-3671: Added alignment for allocation of output sync memory.
 * 
 * Hydra_Software_Devel/47   8/4/10 7:14p nissen
 * SW7405-3671: Increased size of output sync memory to size of maximum
 * palette.
 * 
 * Hydra_Software_Devel/46   8/4/10 6:14p nissen
 * SW7405-3671: Fixed reset state when creating context.
 * 
 * Hydra_Software_Devel/45   7/31/10 8:01p nissen
 * SW7405-3671: Added fix for checking if blits are complete when syncing.
 * 
 * Hydra_Software_Devel/44   7/30/10 11:19a nissen
 * SW7405-3671: Fixed available memory when returning context status.
 * 
 * Hydra_Software_Devel/43   7/29/10 7:53p nissen
 * SW7405-3671: Removed code to submit packets when getting status.
 * 
 * Hydra_Software_Devel/42   7/27/10 2:35p nissen
 * SW7405-3671: Fixed packet memory wrap.
 * 
 * Hydra_Software_Devel/41   7/26/10 2:14p nissen
 * SW7405-3671: Fixed packet memory allocation.
 * 
 * Hydra_Software_Devel/40   7/21/10 10:43p nissen
 * SW7405-3671: Added code to put last context to the top of the list
 * after advancing them all.
 * 
 * Hydra_Software_Devel/39   7/21/10 6:36p nissen
 * SW7405-3671: Added code to free context structure when destroying
 * context.
 * 
 * Hydra_Software_Devel/38   7/21/10 4:34p nissen
 * SW7405-3671: Added support for user supplying private data ptr during
 * context creation which gets passed back when getting status.
 * 
 * Hydra_Software_Devel/37   7/21/10 3:27p nissen
 * SW7405-3671: Added code to validate buffer size when submitting
 * packets.
 * 
 * Hydra_Software_Devel/36   7/21/10 11:47a nissen
 * SW7405-3671: Fixed setting buffer pointer and size out when getting
 * packet memory.
 * 
 * Hydra_Software_Devel/35   7/18/10 4:08p nissen
 * SW7405-3671: Fixed context status.
 * 
 * Hydra_Software_Devel/34   7/17/10 3:32p nissen
 * SW7405-3671: Fixed memory allocation.
 * 
 * Hydra_Software_Devel/33   7/15/10 9:01p nissen
 * SW7405-3671: Added more interrupt support.
 * 
 * Hydra_Software_Devel/32   7/12/10 2:31p nissen
 * SW7405-3671: Rewrote to support sw fifo.
 * 
 * Hydra_Software_Devel/31   6/30/10 9:27p nissen
 * SW7405-3671: Updated color matrix and filter packets.
 * 
 * Hydra_Software_Devel/30   6/30/10 7:50p nissen
 * SW7405-3671: Added support for blending and pixel format enums.
 * 
 * Hydra_Software_Devel/29   6/24/10 2:14p nissen
 * SW7405-3671: Added source feeder destripe packet function.
 * 
 * Hydra_Software_Devel/28   6/23/10 3:49p nissen
 * SW7405-3671: Fixed problem with setting blit ctrl registers.
 * 
 * Hydra_Software_Devel/27   6/23/10 12:33p nissen
 * SW7405-3671: Fixed build error.
 * 
 * Hydra_Software_Devel/26   6/21/10 2:20p nissen
 * SW7405-3671: Fixed packet settings.
 * 
 * Hydra_Software_Devel/25   6/21/10 12:37p nissen
 * SW7405-3671: Added context support. Switched to M2MC packets.
 * 
 * Hydra_Software_Devel/24   5/5/10 12:06p nissen
 * SW7405-3671: Fixed problem with scaler striping size.
 * 
 * Hydra_Software_Devel/23   3/19/10 3:14p nissen
 * SW7405-3671: Fixed alpha premultiply define.
 * 
 * Hydra_Software_Devel/22   3/19/10 2:29p nissen
 * SW7405-3671: Fixed alpha premultiply.
 * 
 * Hydra_Software_Devel/21   3/11/10 1:18p nissen
 * SW7405-3671: Added support for alpha premultiply.
 * 
 * Hydra_Software_Devel/20   3/10/10 2:54p nissen
 * SW7405-3671: Added offset buffer pointers to batch packet blits.
 * 
 * Hydra_Software_Devel/19   2/16/10 4:23p nissen
 * SW7405-3671: Fixed warnings.
 * 
 * Hydra_Software_Devel/18   2/11/10 5:51p nissen
 * SW7405-3671: Fixed setting palette format.
 * 
 * Hydra_Software_Devel/17   2/11/10 5:29p nissen
 * SW7405-3671: Added support for cached packet memory.
 * 
 * Hydra_Software_Devel/16   2/10/10 2:12p nissen
 * SW7405-3671: Fixed palette to palette blits.
 * 
 * Hydra_Software_Devel/15   2/9/10 4:39p nissen
 * SW7405-3671: Updated batch blits.
 * 
 * Hydra_Software_Devel/14   2/8/10 9:02p nissen
 * SW7405-3671: Fixed problem with writing scale packet.
 * 
 * Hydra_Software_Devel/13   2/8/10 5:56p nissen
 * SW7405-3671: Updated batching to submit as many blits as possible.
 * 
 * Hydra_Software_Devel/12   2/5/10 8:16p nissen
 * SW7405-3671: Fixed palette to palette blits.
 * 
 * Hydra_Software_Devel/11   2/5/10 12:12p nissen
 * SW7405-3671: Fixed problem with init packet.
 * 
 * Hydra_Software_Devel/10   2/5/10 10:54a nissen
 * SW7405-3671: Moved packet function.
 * 
 * Hydra_Software_Devel/9   2/3/10 1:57p nissen
 * SW7405-3671: Added batch packets.
 * 
 * Hydra_Software_Devel/8   1/29/10 7:15p nissen
 * SW7405-3671: Added support for using packets in any order.
 * 
 * Hydra_Software_Devel/7   1/19/10 4:49p nissen
 * SW7405-3671: Added support for different M2MC cores.
 * 
 * Hydra_Software_Devel/6   1/19/10 12:27p nissen
 * SW7405-3671: Added support for sync'ing and 420 destriping.
 * 
 * Hydra_Software_Devel/5   1/8/10 2:22p nissen
 * SW7405-3671: Removed buffer size parameters when opening packet module.
 * 
 * Hydra_Software_Devel/4   1/7/10 5:21p nissen
 * SW7405-3671: Fixed functions that write SourceFeeder and FillBlit
 * packets. Added debug printing.
 * 
 * Hydra_Software_Devel/3   12/31/09 10:14p nissen
 * SW7405-3671: Added palette support and software striping while scaling
 * support.
 * 
 * Hydra_Software_Devel/2   12/24/09 5:45p nissen
 * SW7405-3671: Fixed build error.
 * 
 * Hydra_Software_Devel/1   12/24/09 1:04a nissen
 * SW7405-3671: Packet blits
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bstd_defs.h"
#include "berr.h"
#include "bkni.h"
#include "bint.h"
#include "bchp.h"
#include "bmem.h"
#include "bsur.h"
#include "bint.h"
#include "breg_mem.h"
#include "bgrc_errors.h"

#if (BCHP_CHIP==7405)
#include "bchp_int_id_memc16_gfx_l2.h"
#else
#include "bchp_hif_cpu_intr1.h"
#endif

#if ((BCHP_CHIP==7400) && (BCHP_VER >= BCHP_VER_B0))
#include "bchp_memc16_gfx_l2.h"
#include "bchp_memc16_gfx_grb.h"
#elif (BCHP_CHIP==7405)
#include "bchp_memc16_gfx_l2.h"
#include "bchp_memc_gfx_grb.h"
#elif (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || (BCHP_CHIP==7336)
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

#if ((BCHP_CHIP==7400) && (BCHP_VER == BCHP_VER_A0)) || (((BCHP_CHIP==7640) && (BCHP_VER >= BCHP_VER_B0)) || BCHP_CHIP==7440)
#include "bchp_m2mc_1.h"
#elif ((BCHP_CHIP==7438) && (BCHP_VER >= BCHP_VER_A0))
#include "bchp_m2mc_1.h"
#include "bchp_m2mc_1_grb.h"
#include "bchp_int_id_m2mc_1_l2.h"
#endif

#include "bchp_common.h"

#include "bgrc.h"
#include "bgrc_packet.h"
#include "bgrc_packet_priv.h"
#include "bgrc_private.h"

BDBG_MODULE(BGRC);
BDBG_OBJECT_ID(BGRC);
BDBG_OBJECT_ID(BGRC_PacketContext);

/***************************************************************************/
#if ((BCHP_CHIP==7400) && (BCHP_VER >= BCHP_VER_B0))
BINT_Id BGRC_PACKET_P_M2MC_INT_ID = BCHP_INT_ID_CREATE(BCHP_MEMC16_GFX_L2_CPU_STATUS, BCHP_MEMC16_GFX_L2_CPU_STATUS_M2MC_INTR_SHIFT);
#elif (BCHP_CHIP==7438)
BINT_Id BGRC_PACKET_P_M2MC_INT_ID = BCHP_INT_ID_GFX_L2_M2MC_INTR;
#elif ((BCHP_CHIP==7405) && (BCHP_VER >= BCHP_VER_A0))
BINT_Id BGRC_PACKET_P_M2MC_INT_ID = BCHP_INT_ID_M2MC_0_INTR;
#elif (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || (BCHP_CHIP==7336)
BINT_Id BGRC_PACKET_P_M2MC_INT_ID = BCHP_INT_ID_CREATE(BCHP_GRAPHICS_L2_CPU_STATUS, BCHP_GRAPHICS_L2_CPU_STATUS_M2MC_0_INTR_SHIFT);
#elif (BCHP_CHIP==7420) || (BCHP_CHIP==7125)
BINT_Id BGRC_PACKET_P_M2MC_INT_ID = BCHP_INT_ID_CREATE(BCHP_GFX_L2_CPU_STATUS, BCHP_GFX_L2_CPU_STATUS_M2MC_INTR_SHIFT);
#elif (BCHP_CHIP==7468)
BINT_Id BGRC_PACKET_P_M2MC_INT_ID = BCHP_INT_ID_CREATE(BCHP_M2MC_WRAP_L2_CPU_STATUS, BCHP_M2MC_WRAP_L2_CPU_STATUS_M2MC_INTR_SHIFT);
#elif (BCHP_CHIP==7552) || (BCHP_CHIP==7550) || (BCHP_CHIP==7358) || (BCHP_CHIP==7360) || (BCHP_CHIP==7563)
BINT_Id BGRC_PACKET_P_M2MC_INT_ID = BCHP_INT_ID_CREATE(BCHP_M2MC_TOP_L2_CPU_STATUS, BCHP_M2MC_TOP_L2_CPU_STATUS_M2MC_INTR_SHIFT);
#elif ((BCHP_CHIP==7640) && (BCHP_VER >= BCHP_VER_B0))
BINT_Id BGRC_PACKET_P_M2MC_INT_ID   = BCHP_INT_ID_CREATE(BCHP_WRAP_M2MC_L2_CPU_STATUS, BCHP_WRAP_M2MC_L2_CPU_STATUS_M2MC_0_INTR_SHIFT);
BINT_Id BGRC_PACKET_P_M2MC_1_INT_ID = BCHP_INT_ID_CREATE(BCHP_WRAP_M2MC_L2_CPU_STATUS, BCHP_WRAP_M2MC_L2_CPU_STATUS_M2MC_1_INTR_SHIFT);
#elif (BCHP_CHIP==7435) || (BCHP_CHIP==7445)
BINT_Id BGRC_PACKET_P_M2MC0_INT_ID = BCHP_INT_ID_CREATE(BCHP_M2MC_L2_CPU_STATUS, BCHP_M2MC_L2_CPU_STATUS_M2MC_INTR_SHIFT);
BINT_Id BGRC_PACKET_P_M2MC1_INT_ID = BCHP_INT_ID_CREATE(BCHP_M2MC1_L2_CPU_STATUS, BCHP_M2MC_L2_CPU_STATUS_M2MC_INTR_SHIFT);
#else
BINT_Id BGRC_PACKET_P_M2MC_INT_ID = BCHP_INT_ID_M2MC_INTR;
#endif

/***************************************************************************/
#define BGRC_PACKET_MEMORY_MAX    131072
#define BGRC_OPERATION_MAX          2048
#define BGRC_WAIT_TIMEOUT             10

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
void BGRC_P_ResetDevice( 
	BGRC_Handle hGrc )
{
#if ((BCHP_CHIP==7400) && (BCHP_VER >= BCHP_VER_B0))
	BREG_Write32( hGrc->hRegister, BCHP_MEMC16_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(MEMC16_GFX_GRB_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_MEMC16_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(MEMC16_GFX_GRB_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
#elif ((BCHP_CHIP==7405) && (BCHP_VER >= BCHP_VER_A0))
	BREG_Write32( hGrc->hRegister, BCHP_MEMC_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(MEMC_GFX_GRB_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_MEMC_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(MEMC_GFX_GRB_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
#elif (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || (BCHP_CHIP==7336)
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
	while( BREG_Read32(hGrc->hRegister, BCHP_M2MC_BLIT_STATUS) == 0 );
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
#elif (BCHP_CHIP==7640)
	if( hGrc->ulDeviceNum )
	{
		BREG_Write32( hGrc->hRegister, BCHP_WRAP_M2MC_GRB_SW_RESET_0, BCHP_FIELD_ENUM(WRAP_M2MC_GRB_SW_RESET_0, M2MC1_SW_RESET, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_WRAP_M2MC_GRB_SW_RESET_0, BCHP_FIELD_ENUM(WRAP_M2MC_GRB_SW_RESET_0, M2MC1_SW_RESET, DEASSERT) );
	}
	else
	{
		BREG_Write32( hGrc->hRegister, BCHP_WRAP_M2MC_GRB_SW_RESET_0, BCHP_FIELD_ENUM(WRAP_M2MC_GRB_SW_RESET_0, M2MC0_SW_RESET, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_WRAP_M2MC_GRB_SW_RESET_0, BCHP_FIELD_ENUM(WRAP_M2MC_GRB_SW_RESET_0, M2MC0_SW_RESET, DEASSERT) );
	}
#elif (BCHP_CHIP==7435) || (BCHP_CHIP==7445)
	if( hGrc->ulDeviceNum )
	{
		BREG_Write32( hGrc->hRegister, BCHP_M2MC1_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC1_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC1_CLK_GATE_AND_SW_INIT_CONTROL, BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL_START_SW_INIT_MASK );
		while( BREG_Read32(hGrc->hRegister, BCHP_M2MC1_BLIT_STATUS) == 0 );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC1_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC1_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );
	}
	else
	{
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL, BCHP_M2MC_CLK_GATE_AND_SW_INIT_CONTROL_START_SW_INIT_MASK );
		while( BREG_Read32(hGrc->hRegister, BCHP_M2MC_BLIT_STATUS) == 0 );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, ASSERT) );
		BREG_Write32( hGrc->hRegister, BCHP_M2MC_GR_SW_INIT_0, BCHP_FIELD_ENUM(M2MC_GR_SW_INIT_0, M2MC_CLK_108_SW_INIT, DEASSERT) );
	}
#else
	BREG_Write32( hGrc->hRegister, BCHP_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GRB_SW_RESET_0, M2MC_SW_RESET, ASSERT) );
	BREG_Write32( hGrc->hRegister, BCHP_GFX_GRB_SW_RESET_0, BCHP_FIELD_ENUM(GFX_GRB_SW_RESET_0, M2MC_SW_RESET, DEASSERT) );
#endif
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
	BERR_Code err = BERR_SUCCESS;

	BGRC_Handle hGrc = (BGRC_Handle) BKNI_Malloc( sizeof (BGRC_P_Handle) );
	if( hGrc == 0 )
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);

	BKNI_Memset( (void *) hGrc, 0, sizeof (BGRC_P_Handle) );
	BDBG_OBJECT_SET(hGrc, BGRC);

	hGrc->hChip = hChip;
	hGrc->hRegister = hRegister;
	hGrc->hMemory = hMemory;
	hGrc->hInterrupt = hInterrupt;
	hGrc->ulPacketMemoryMax = pDefSettings ? pDefSettings->ulPacketMemoryMax : BGRC_P_DEFAULT_SETTINGS.ulPacketMemoryMax;
	hGrc->ulOperationMax = pDefSettings ? pDefSettings->ulOperationMax : BGRC_P_DEFAULT_SETTINGS.ulOperationMax;
	hGrc->ulDeviceNum = pDefSettings ? pDefSettings->ulDeviceNum : BGRC_P_DEFAULT_SETTINGS.ulDeviceNum;
	hGrc->ulWaitTimeout = pDefSettings ? pDefSettings->ulWaitTimeout : BGRC_P_DEFAULT_SETTINGS.ulWaitTimeout;

	/* allocate device fifo memory */
	hGrc->fifo_memory_addr = BMEM_Heap_AllocAligned( hGrc->hMemory, hGrc->ulPacketMemoryMax + BGRC_PACKET_P_BLIT_GROUP_SIZE_MAX, BGRC_PACKET_P_MEMORY_ALIGN_BITS, 0 );
	if( hGrc->fifo_memory_addr == NULL )
	{
		BGRC_Close( hGrc );
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}

	err = BMEM_ConvertAddressToOffset( hGrc->hMemory, hGrc->fifo_memory_addr, &hGrc->fifo_base_offset );
	if( err != BERR_SUCCESS )
	{
		BGRC_Close( hGrc );
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}

	err = BMEM_ConvertAddressToCached( hGrc->hMemory, hGrc->fifo_memory_addr, (void *) &hGrc->fifo_cached_addr );
	if( err != BERR_SUCCESS )
	{
		BGRC_Close( hGrc );
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}

	hGrc->fifo_total_size = hGrc->ulPacketMemoryMax;
	hGrc->fifo_base_addr = hGrc->fifo_cached_addr;
/*	hGrc->fifo_base_addr = hGrc->fifo_memory_addr;*/
	hGrc->curr_fifo_addr = hGrc->fifo_base_addr;
	hGrc->curr_fifo_offset = hGrc->fifo_base_offset;
	hGrc->start_flush_addr = hGrc->curr_fifo_addr;

	/* init context list */
	BLST_D_INIT(&hGrc->context_list);

	/* reset m2mc */
	BGRC_P_ResetDevice( hGrc );

	*phGrc = hGrc;
	return BERR_SUCCESS;
}

/***************************************************************************/
void BGRC_Close(
	BGRC_Handle hGrc )
{
	BDBG_OBJECT_ASSERT(hGrc, BGRC);
	BDBG_ASSERT(BLST_D_EMPTY(&hGrc->context_list));

	/* destroy callbacks */
	if( hGrc->hCallback )
	{
		BINT_DisableCallback( hGrc->hCallback );
		BINT_DestroyCallback( hGrc->hCallback );
	}

	/* free memory */
	if( hGrc->fifo_memory_addr )
		BMEM_Free( hGrc->hMemory, hGrc->fifo_memory_addr );

	BDBG_OBJECT_DESTROY(hGrc, BGRC);
	BKNI_Free( (void *) hGrc );
}

/***************************************************************************/
static BERR_Code BGRC_PACKET_P_SetCallback_BINT(
	BGRC_Handle hGrc,
	BINT_CallbackHandle *phCallback,
	bool create )
{
	if( create )
	{
		if( *phCallback == 0 )
		{
#if defined(BCHP_M2MC1_REVISION) || defined(BCHP_M2MC_1_REVISION)
			uint32_t int_id = hGrc->ulDeviceNum ? BGRC_PACKET_P_M2MC1_INT_ID : BGRC_PACKET_P_M2MC0_INT_ID;
#else
			uint32_t int_id = BGRC_PACKET_P_M2MC_INT_ID;
#endif

			BERR_Code err = BINT_CreateCallback( phCallback, hGrc->hInterrupt, int_id, BGRC_PACKET_P_Isr, hGrc, 0);
			if( err != BERR_SUCCESS )
				return BERR_TRACE(err);

			BINT_ClearCallback( *phCallback );
			BINT_EnableCallback( *phCallback );
		}
	}
	else if( *phCallback )
	{
		BINT_DisableCallback( *phCallback );
		BINT_DestroyCallback( *phCallback );
		*phCallback = 0;
	}

	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Packet_SetCallback(
	BGRC_Handle hGrc,
	BGRC_Callback callback_isr,
	void *callback_data )
{
	BDBG_OBJECT_ASSERT(hGrc, BGRC);

	BKNI_EnterCriticalSection();
	hGrc->callback_isr = callback_isr;
	hGrc->callback_data = callback_data;
	BKNI_LeaveCriticalSection();

	BGRC_PACKET_P_SetCallback_BINT( hGrc, &hGrc->hCallback, NULL != callback_isr);

	return BERR_SUCCESS;
}

/***************************************************************************/
#define BGRC_PACKET_P_DEFAULT_PACKET_BUFFER_SIZE (1024*64)

static BGRC_PacketContext_CreateSettings BGRC_PACKET_P_DefaultSettings =
{
	0,                                         /* packet memory heap */
	BGRC_PACKET_P_DEFAULT_PACKET_BUFFER_SIZE,  /* packet_buffer_size */
	0,                                         /* packet_buffer_store */
	NULL,                                      /* private_data */
	{
		0,                                     /* bounded memory offset */
		0                                      /* bounded memory size */
	}
};

/***************************************************************************/
BERR_Code BGRC_Packet_GetDefaultCreateContextSettings(
	BGRC_Handle hGrc,
	BGRC_PacketContext_CreateSettings *pSettings )
{
	BSTD_UNUSED(hGrc);
	BDBG_OBJECT_ASSERT(hGrc, BGRC);

	if( pSettings )
		*pSettings = BGRC_PACKET_P_DefaultSettings;

	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Packet_CreateContext(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle *phContext,
	BGRC_PacketContext_CreateSettings *pSettings )
{
	static  uint32_t s_ulNumCreates = 1;

	BMEM_Handle packet_heap = (pSettings && pSettings->packet_buffer_heap) ? pSettings->packet_buffer_heap : hGrc->hMemory;
	BERR_Code err = BERR_SUCCESS;

	BGRC_PacketContext_Handle hContext = (BGRC_PacketContext_Handle) BKNI_Malloc( sizeof (BGRC_P_PacketContext) );
	if( hContext == 0 )
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);

	BKNI_Memset( (void *) hContext, 0, sizeof (BGRC_P_PacketContext) );
	BDBG_OBJECT_SET(hContext, BGRC_PacketContext);

	BDBG_OBJECT_ASSERT(hGrc, BGRC);
	BDBG_ASSERT(phContext);

	/* add context to list */
	BLST_D_INSERT_HEAD(&hGrc->context_list, hContext, context_link);
	hContext->ulId = (hGrc->ulDeviceNum << 30) | (++s_ulNumCreates & 0x3fffffff);

	/* allocate packet buffer */ 
	hContext->buffer_total_size = pSettings ? pSettings->packet_buffer_size : BGRC_PACKET_P_DefaultSettings.packet_buffer_size;
	hContext->buffer_alloc_addr = BMEM_Heap_AllocAligned( packet_heap, hContext->buffer_total_size + BGRC_PACKET_P_EXTRA_PACKET_SIZE, 2, 0 );
	if( hContext->buffer_alloc_addr == 0 )
	{
		err = BERR_OUT_OF_SYSTEM_MEMORY;
		BGRC_Packet_DestroyContext( hGrc, hContext );
		return BERR_TRACE(err);
	}

	err = BMEM_ConvertAddressToCached( packet_heap, hContext->buffer_alloc_addr, (void *) &hContext->buffer_base_addr );
	if( err != BERR_SUCCESS )
	{
		err = BERR_OUT_OF_SYSTEM_MEMORY;
		BGRC_Packet_DestroyContext( hGrc, hContext );
		return BERR_TRACE(err);
	}

	/* allocate sync blit output plane */
	hContext->sync_output_addr = BMEM_Heap_AllocAligned( hGrc->hMemory, 1024, BGRC_PACKET_P_MEMORY_ALIGN_BITS, 0 );
	if( hContext->sync_output_addr == NULL )
	{
		err = BERR_OUT_OF_DEVICE_MEMORY;
		BGRC_Packet_DestroyContext( hGrc, hContext );
		return BERR_TRACE(err);
	}
	err = BMEM_ConvertAddressToOffset( hGrc->hMemory, hContext->sync_output_addr, &hContext->sync_output_offset );
	if( err != BERR_SUCCESS )
	{
		err = BERR_OUT_OF_DEVICE_MEMORY;
		BGRC_Packet_DestroyContext( hGrc, hContext );
		return BERR_TRACE(err);
	}

	err = BMEM_ConvertAddressToCached( hGrc->hMemory, hContext->sync_output_addr, (void *) &hContext->sync_output_cache_addr );
	if( err != BERR_SUCCESS )
	{
		err = BERR_OUT_OF_SYSTEM_MEMORY;
		BGRC_Packet_DestroyContext( hGrc, hContext );
		return BERR_TRACE(err);
	}

	hContext->sync_output_cache_addr[0] = 0;
	BMEM_FlushCache( hGrc->hMemory, hContext->sync_output_cache_addr, 4 );

	/* reset state */
	BGRC_PACKET_P_ResetState( hContext );

	/* disable 3-2 pulldown cadence detect */
#if ((BCHP_CHIP==7038) && (BCHP_VER <  BCHP_VER_C0)) || \
	((BCHP_CHIP==3560) && (BCHP_VER >= BCHP_VER_B0)) || \
	((BCHP_CHIP==3563) && (BCHP_VER >= BCHP_VER_A0))
	BREG_Write32( hGrc->hRegister, BCHP_M2MC_BLEND_PULLDOWN_COUNTER_ENABLE, BCHP_M2MC_BLEND_PULLDOWN_COUNTER_ENABLE_ENABLE_DISABLE );
#endif

	/* store settings */
	if( pSettings )
		hContext->create_settings = *pSettings;

	*phContext = hContext;
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Packet_DestroyContext(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext )
{
	BDBG_ASSERT(hGrc);
	BDBG_ASSERT(hContext);
	BDBG_OBJECT_ASSERT(hGrc, BGRC);
	BDBG_OBJECT_ASSERT(hContext, BGRC_PacketContext);

	BDBG_ASSERT(hGrc->callback_isr);
		/* flush packet processing */

		/* flush blits */

	/* remove context from list */
	BLST_D_REMOVE(&hGrc->context_list, hContext, context_link);

	/* destroy callbacks */
	if( hContext->hCallback )
	{
		BINT_DisableCallback( hContext->hCallback );
		BINT_DestroyCallback( hContext->hCallback );
	}

	/* free memory */
	if( hContext->sync_output_addr )
		BMEM_Free( hGrc->hMemory, hContext->sync_output_addr );

	if( hContext->buffer_alloc_addr )
	{
		BMEM_Handle packet_heap = hContext->create_settings.packet_buffer_heap ? hContext->create_settings.packet_buffer_heap : hGrc->hMemory;
		BMEM_Free( packet_heap, hContext->buffer_alloc_addr );
	}

	BDBG_OBJECT_DESTROY(hContext, BGRC_PacketContext);
	BKNI_Free( hContext );


/***************************************************************************/



	return BERR_SUCCESS;
}

/***************************************************************************/
int BGRC_PACKET_P_GetPacketMemory(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext,
	size_t size_in )
{
	int buffer_available = 0;

	BDBG_ASSERT(hGrc);
	BDBG_ASSERT(hContext);
	BDBG_OBJECT_ASSERT(hGrc, BGRC);
	BDBG_OBJECT_ASSERT(hContext, BGRC_PacketContext);

	/* get available buffer size */
	if( hContext->buffer_submit_size && (hContext->buffer_submit_offset >= hContext->buffer_alloc_offset) )
	{
		buffer_available = hContext->buffer_submit_offset - hContext->buffer_alloc_offset;
	}
	else
	{
		buffer_available = hContext->buffer_total_size - hContext->buffer_alloc_offset;
		if( buffer_available < (int) (size_in + BGRC_PACKET_P_EXTRA_PACKET_SIZE) )
		{
			/* wrap buffer if less than needed */
			if( (hContext->buffer_submit_size == 0) || (hContext->buffer_submit_offset >= BGRC_PACKET_P_EXTRA_PACKET_SIZE) )
			{
				/* submit end-of-buffer packet */
				BM2MC_PACKET_Header *header = (BM2MC_PACKET_Header *) ((uint8_t *) hContext->buffer_base_addr + hContext->buffer_alloc_offset);
				header->type = 0;
				header->size = sizeof (BM2MC_PACKET_Header) / sizeof (uint32_t);
				header->execute = 0;
				header->sync = 0;
				hContext->last_size_out = sizeof (BM2MC_PACKET_Header);
				BGRC_Packet_SubmitPackets( hGrc, hContext, sizeof (BM2MC_PACKET_Header) );

				/* reset alloc offset and get available size */
				hContext->buffer_alloc_offset = 0;
				buffer_available = hContext->buffer_submit_size ? hContext->buffer_submit_offset : hContext->buffer_total_size;
			}
		}
	}

	return buffer_available;
}
/***************************************************************************/
BERR_Code BGRC_Packet_GetPacketMemory(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext,
	void **buffer,
	size_t *size_out, 
	size_t size_in )
{
	int buffer_available = 0;
	int alloc_size = 0;

	BDBG_ASSERT(hGrc);
	BDBG_ASSERT(hContext);
	BDBG_ASSERT(buffer);
	BDBG_ASSERT(size_in >= sizeof (BM2MC_PACKET_Header));
	BDBG_OBJECT_ASSERT(hGrc, BGRC);
	BDBG_OBJECT_ASSERT(hContext, BGRC_PacketContext);

	/* get available packet buffer memory */
	buffer_available = BGRC_PACKET_P_GetPacketMemory( hGrc, hContext, size_in );

	/* check if buffer is full */
	if( buffer_available < (int) (size_in + BGRC_PACKET_P_EXTRA_PACKET_SIZE) )
	{
		/* check if packets need submitting */
		if( hContext->buffer_submit_size )
		{
			BGRC_PACKET_P_SubmitPackets( hGrc, hContext );
			buffer_available = BGRC_PACKET_P_GetPacketMemory( hGrc, hContext, size_in );
		}
	}

	/* check if buffer is full */
	if( buffer_available < (int) (size_in + BGRC_PACKET_P_EXTRA_PACKET_SIZE) )
	{
		*buffer = NULL;
	}
	else
	{
		*buffer = (uint8_t *) hContext->buffer_base_addr + hContext->buffer_alloc_offset;
		alloc_size = buffer_available - BGRC_PACKET_P_EXTRA_PACKET_SIZE;
	}

	hContext->last_size_out = alloc_size;

	if( size_out )
		*size_out = alloc_size;

	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Packet_SubmitPackets(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext,
	size_t size )
{
	BDBG_ASSERT(hGrc);
	BDBG_ASSERT(hContext);
	BDBG_OBJECT_ASSERT(hGrc, BGRC);
	BDBG_OBJECT_ASSERT(hContext, BGRC_PacketContext);

	if( size > (size_t) hContext->last_size_out )
	{
		BDBG_ERR(("BGRC_Packet_SubmitPackets %d bytes exceeds last BGRC_Packet_GetPacketMemory %d bytes. System memory is now corrupted.", size, hContext->last_size_out));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	hContext->last_size_out = 0;
	hContext->packets_submitted = true;

	if( size >= sizeof (BM2MC_PACKET_Header) )
	{
		/* increase size of submitted buffer */
		hContext->buffer_submit_size += size;
		hContext->buffer_alloc_offset += size;

		/* submit packets */
		if( hContext->buffer_submit_size > hContext->create_settings.packet_buffer_store )
			BGRC_PACKET_P_SubmitPackets( hGrc, hContext );
	}

	/* return message indicating packet processing is incomplete */
	if( hContext->buffer_submit_size )
		return BGRC_PACKET_MSG_PACKETS_INCOMPLETE;

	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Packet_AdvancePackets( 
	BGRC_Handle hGrc, 
	BGRC_PacketContext_Handle hContext )
{
	BERR_Code err = BERR_SUCCESS;
	BGRC_PacketContext_Handle hLastContext = 0;
	BGRC_PacketContext_Handle hCurrContext;

	BDBG_ASSERT(hGrc);
	BDBG_OBJECT_ASSERT(hGrc, BGRC);

	hCurrContext = hContext ? hContext : BLST_D_FIRST(&hGrc->context_list);

	/* loop through contexts */
	while( hCurrContext )
	{
		/* check if packets left to process */
		if( hCurrContext->buffer_submit_size )
		{
			/* loop until packet processing complete if using event */
			/* submit packets */
			BGRC_PACKET_P_SubmitPackets( hGrc, hCurrContext );
		}

		/* save error if a context's packets still need processing */
		if( hCurrContext->buffer_submit_size )
			err = BGRC_PACKET_MSG_PACKETS_INCOMPLETE;

		/* get next context */
		hLastContext = hCurrContext;
		hCurrContext = hContext ? 0 : BLST_D_NEXT(hCurrContext, context_link);
	}

	/* put last context at the head of the list to give it first access to free memory */
	if( (hContext == 0) && hLastContext )
	{
		BLST_D_REMOVE(&hGrc->context_list, hLastContext, context_link);
		BLST_D_INSERT_HEAD(&hGrc->context_list, hLastContext, context_link);
	}

	return err;
}

/***************************************************************************/
BERR_Code BGRC_Packet_SyncPackets(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext )
{
/* avoid BGRC_Packet_GetStatus short circuit for now */
#if USE_GLOBAL_STATE_TO_SHORT_CIRCUIT_SYNC
	bool context_idle = false;
#endif
	BERR_Code err = BERR_SUCCESS;

	BDBG_ASSERT(hGrc);
	BDBG_ASSERT(hContext);
	BDBG_OBJECT_ASSERT(hGrc, BGRC);
	BDBG_OBJECT_ASSERT(hContext, BGRC_PacketContext);

	/* can not call in again before previous request has been cleared */
	if (BGRC_PACKET_P_SyncState_eCleared != hContext->eSyncState)
	{
		return BERR_TRACE(BERR_NOT_SUPPORTED);
	}

	/* return message indicating context's blits are complete */
	if( !hContext->packets_submitted )
	{
		return BGRC_PACKET_MSG_BLITS_COMPLETE;
	}

#if USE_GLOBAL_STATE_TO_SHORT_CIRCUIT_SYNC
	/* check if context is busy */
	if( hContext->buffer_submit_size == 0 )
	{
		BGRC_Packet_Status status;
		err = BGRC_Packet_GetStatus( hGrc, &status );
		if( err == BERR_SUCCESS )
			context_idle = !status.m2mc_busy;
	}

	/* return message indicating context's blits are complete */
	if( context_idle )
	{
		return BGRC_PACKET_MSG_BLITS_COMPLETE;
	}
#endif

	/* do sync blit */
	hContext->eSyncState = BGRC_PACKET_P_SyncState_eRequested;
	err = BGRC_PACKET_P_SyncBlit( hGrc, hContext );
	while( BGRC_PACKET_P_SyncState_eQueuedInHw != hContext->eSyncState )
	{
		BGRC_Packet_AdvancePackets( hGrc, hContext );
		err = BGRC_PACKET_P_SyncBlit( hGrc, hContext );
	}

	/* advance processing for any remaining packets */
	if( hContext->buffer_submit_size )
		err = BGRC_Packet_AdvancePackets( hGrc, hContext );

	return err;
}

/***************************************************************************/
BERR_Code BGRC_Packet_GetContextStatus(
	BGRC_Handle hGrc,
	BGRC_Packet_ContextStatus *status_array,
	size_t *size_out,
	size_t size_in )
{
	BGRC_PacketContext_Handle hContext;
	size_t context_index = 0;

	BDBG_ASSERT(hGrc);
	BDBG_OBJECT_ASSERT(hGrc, BGRC);
	BDBG_ASSERT(status_array);
	BDBG_ASSERT(size_out);
	BDBG_ASSERT(size_in);

	/* get first context */
	hContext = BLST_D_FIRST(&hGrc->context_list);
	while( hContext )
	{
		/* check if context sync'ed */
		bool context_sync = false;

		if (BGRC_PACKET_P_SyncState_eQueuedInHw == hContext->eSyncState) {
			BMEM_FlushCache( hGrc->hMemory, hContext->sync_output_cache_addr, 4 );
			context_sync = (0 != hContext->sync_output_cache_addr[0]);
			if( !context_sync )
			{
				uint32_t list_status, blit_status, cur_desc;
				list_status = BGRC_P_ReadReg32( hGrc->hRegister, LIST_STATUS );
				blit_status = BGRC_P_ReadReg32( hGrc->hRegister, BLIT_STATUS );
				cur_desc = BGRC_P_ReadReg32( hGrc->hRegister, LIST_CURR_PKT_ADDR );
				context_sync = (((BGRC_M2MC(LIST_STATUS_FINISHED_MASK) == list_status) &&
								 (BGRC_M2MC(BLIT_STATUS_STATUS_IDLE) == blit_status)) ||
								(cur_desc == hContext->sync_pkt_offset));
			}
			if( context_sync ) {
				hContext->eSyncState = BGRC_PACKET_P_SyncState_eCleared;
				hContext->packets_submitted = false;
			}
		}

		/* check if context sync'ed or packets still need processing */
		if( context_sync || hContext->advance_interrupt || hContext->buffer_submit_size )
		{
			uint32_t buffer_available = 0;

			if( hContext->advance_interrupt || hContext->buffer_submit_size )
			{
				/* calculate available packer buffer size */
				uint32_t end_offset = (hContext->buffer_submit_size && (hContext->buffer_submit_offset >= hContext->buffer_alloc_offset)) ?
					hContext->buffer_submit_offset : hContext->buffer_total_size;
				uint32_t extra_size = (end_offset == hContext->buffer_total_size) ? hContext->buffer_submit_offset : 0;

				buffer_available = end_offset - hContext->buffer_alloc_offset + extra_size;
				hContext->advance_interrupt = false;
			}

			/* fill status entry */
			status_array[context_index].hContext = hContext;
			status_array[context_index].private_data = hContext->create_settings.private_data;
			status_array[context_index].sync = context_sync;
			status_array[context_index].packet_buffer_available = buffer_available;
			context_index++;
		}

		/* exit if size of array is not large enough */
		if( context_index == size_in ) {
			/* this is not an error, but could be a problem if hit repeatedly */
			BDBG_WRN(("BGRC_Packet_GetContextStatus hit %d limit", size_in));
			break;
		}

		/* get next context */
		hContext = BLST_D_NEXT(hContext, context_link);
	}

	*size_out = context_index;
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Packet_GetStatus(
	BGRC_Handle hGrc,
	BGRC_Packet_Status *status )
{
	BDBG_ASSERT(hGrc);
	BDBG_ASSERT(status);
	BDBG_OBJECT_ASSERT(hGrc, BGRC);

	status->m2mc_busy = (!hGrc->advance_interrupt);
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Packet_ConvertPixelFormat(
	BM2MC_PACKET_PixelFormat *pformat,
	BPXL_Format pxl_format )
{
	BDBG_ASSERT(pformat);

	*pformat = BGRC_PACKET_P_ConvertPixelFormat( pxl_format );
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Packet_ConvertFilter( 
	BM2MC_PACKET_FilterCoeffs *coeffs,
	BGRC_FilterCoeffs filter,
	size_t src_size,
	size_t out_size )
{
	BDBG_ASSERT(coeffs);

	BGRC_PACKET_P_ConvertFilterCoeffs( coeffs, filter, src_size, out_size );
	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BGRC_Packet_ConvertColorMatrix( 
	BM2MC_PACKET_ColorMatrix *matrix_out, 
	const int32_t *matrix_in, 
	size_t shift )
{
	BDBG_ASSERT(matrix_out);
	BDBG_ASSERT(matrix_in);

	BGRC_PACKET_P_ConvertColorMatrix( matrix_out, matrix_in, shift );
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetSourceSurfacePacket( BGRC_Handle hGrc, void **ppPacket, 
	BSUR_Surface_Handle srcSurface0, BSUR_Surface_Handle srcSurface1, uint32_t color )
{
	BM2MC_PACKET_PixelFormat m2mc_format0 = BM2MC_PACKET_PixelFormat_eUnknown;
	BM2MC_PACKET_PixelFormat m2mc_format1 = BM2MC_PACKET_PixelFormat_eUnknown;
	uint32_t offset0, pitch0;
	uint32_t offset1, pitch1;
	BPXL_Format format = 0;
	void *pMemory;

	if( srcSurface0 )
	{
		BSUR_Surface_GetAddress( srcSurface0, &pMemory, &pitch0 );
		BSUR_Surface_GetOffset( srcSurface0, &offset0 );
		BSUR_Surface_GetFormat( srcSurface0, &format );
		BGRC_Packet_ConvertPixelFormat( &m2mc_format0, format );
	}
	if( srcSurface1 )
	{
		BSUR_Surface_GetAddress( srcSurface1, &pMemory, &pitch1 );
		BSUR_Surface_GetOffset( srcSurface1, &offset1 );
		BSUR_Surface_GetFormat( srcSurface1, &format );
		BGRC_Packet_ConvertPixelFormat( &m2mc_format1, format );
	}

	if( srcSurface0 && srcSurface1 )
	{
		BM2MC_PACKET_PacketSourceFeeders *pPacket = (BM2MC_PACKET_PacketSourceFeeders *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, SourceFeeders, false );
		pPacket->plane0.address = offset0;
		pPacket->plane0.pitch = pitch0;
		pPacket->plane0.format = m2mc_format0;
		pPacket->plane0.width = 0;
		pPacket->plane0.height = 0;
		pPacket->plane1.address = offset1;
		pPacket->plane1.pitch = pitch1;
		pPacket->plane1.format = m2mc_format1;
		pPacket->plane1.width = 0;
		pPacket->plane1.height = 0;
		pPacket->color = color;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	else if( srcSurface0 )
	{
		BM2MC_PACKET_PacketSourceFeeder *pPacket = (BM2MC_PACKET_PacketSourceFeeder *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, SourceFeeder, false );
		pPacket->plane.address = offset0;
		pPacket->plane.pitch = pitch0;
		pPacket->plane.format = m2mc_format0;
		pPacket->plane.width = 0;
		pPacket->plane.height = 0;
		pPacket->color = color;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	else
	{
		BM2MC_PACKET_PacketSourceColor *pPacket = (BM2MC_PACKET_PacketSourceColor *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, SourceColor, false );
		pPacket->color = color;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetSourcePlanePacket( BGRC_Handle hGrc, void **ppPacket, 
	const BM2MC_PACKET_Plane *pSrcPlane0, const BM2MC_PACKET_Plane *pSrcPlane1, uint32_t color )
{
	if( pSrcPlane0 && pSrcPlane1 )
	{
		BM2MC_PACKET_PacketSourceFeeders *pPacket = (BM2MC_PACKET_PacketSourceFeeders *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, SourceFeeders, false );
		pPacket->plane0 = *pSrcPlane0;
		pPacket->plane1 = *pSrcPlane1;
		pPacket->color = color;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	else if( pSrcPlane0 )
	{
		BM2MC_PACKET_PacketSourceFeeder *pPacket = (BM2MC_PACKET_PacketSourceFeeder *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, SourceFeeder, false );
		pPacket->plane = *pSrcPlane0;
		pPacket->color = color;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	else
	{
		BM2MC_PACKET_PacketSourceColor *pPacket = (BM2MC_PACKET_PacketSourceColor *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, SourceColor, false );
		pPacket->color = color;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetSourceDestripePacket( BGRC_Handle hGrc, void **ppPacket, 
	uint32_t lumaOffset, uint32_t chromaOffset, uint32_t pitch, uint32_t color )
{
	BM2MC_PACKET_PixelFormat m2mc_format0;
	BM2MC_PACKET_PixelFormat m2mc_format1;
	BM2MC_PACKET_PacketSourceFeeders *pPacket = (BM2MC_PACKET_PacketSourceFeeders *) (*ppPacket);

	BGRC_Packet_ConvertPixelFormat( &m2mc_format0, BPXL_eY8 );
	BGRC_Packet_ConvertPixelFormat( &m2mc_format1, BPXL_eCb8_Cr8 );

	BM2MC_PACKET_INIT( pPacket, SourceFeeders, false );
	pPacket->plane0.address = lumaOffset;
	pPacket->plane0.pitch = pitch;
	pPacket->plane0.format = m2mc_format0;
	pPacket->plane0.width = 0;
	pPacket->plane0.height = 0;
	pPacket->plane1.address = chromaOffset;
	pPacket->plane1.pitch = pitch;
	pPacket->plane1.format = m2mc_format1;
	pPacket->plane1.width = 0;
	pPacket->plane1.height = 0;
	pPacket->color = color;
	BM2MC_PACKET_TERM( pPacket );
	*ppPacket = (void *) pPacket;

	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetSourceControl( BGRC_Handle hGrc, void **ppPacket, 
	bool zero_pad, bool chroma_filter, bool linear_destripe )
{
	BM2MC_PACKET_PacketSourceControl *pPacket = (BM2MC_PACKET_PacketSourceControl *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, SourceControl, false );
	pPacket->zero_pad = zero_pad;
	pPacket->chroma_filter = chroma_filter;
	pPacket->linear_destripe = linear_destripe;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetDestinationSurfacePacket( BGRC_Handle hGrc, void **ppPacket, 
	BSUR_Surface_Handle surface, uint32_t color )
{
	if( surface )
	{
		BM2MC_PACKET_PixelFormat m2mc_format;
		BPXL_Format format;
		uint32_t offset;
		uint32_t pitch;
		void *pMemory;

		BM2MC_PACKET_PacketDestinationFeeder *pPacket = (BM2MC_PACKET_PacketDestinationFeeder *) (*ppPacket);
		BSUR_Surface_GetFormat( surface, &format );
		BSUR_Surface_GetAddress( surface, &pMemory, &pitch );
		BSUR_Surface_GetOffset( surface, &offset );
		BGRC_Packet_ConvertPixelFormat( &m2mc_format, format );

		BM2MC_PACKET_INIT( pPacket, DestinationFeeder, false );
		pPacket->plane.address = offset;
		pPacket->plane.pitch = pitch;
		pPacket->plane.format = m2mc_format;
		pPacket->plane.width = 0;
		pPacket->plane.height = 0;
		pPacket->color = color;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	else
	{
		BM2MC_PACKET_PacketDestinationNone *pPacket = (BM2MC_PACKET_PacketDestinationNone *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, DestinationNone, false );
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetDestinationPlanePacket( BGRC_Handle hGrc, void **ppPacket, 
	const BM2MC_PACKET_Plane *pPlane, uint32_t color )
{
	if( pPlane )
	{
		BM2MC_PACKET_PacketDestinationFeeder *pPacket = (BM2MC_PACKET_PacketDestinationFeeder *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, DestinationFeeder, false );
		pPacket->plane = *pPlane;
		pPacket->color = color;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	else
	{
		BM2MC_PACKET_PacketDestinationNone *pPacket = (BM2MC_PACKET_PacketDestinationNone *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, DestinationNone, false );
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetDestinationControl( BGRC_Handle hGrc, void **ppPacket, 
	bool zero_pad, bool chroma_filter )
{
	BM2MC_PACKET_PacketDestinationControl *pPacket = (BM2MC_PACKET_PacketDestinationControl *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, DestinationControl, false );
	pPacket->zero_pad = zero_pad;
	pPacket->chroma_filter = chroma_filter;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetOutputSurfacePacket( BGRC_Handle hGrc, void **ppPacket, 
	BSUR_Surface_Handle surface )
{
	BM2MC_PACKET_PixelFormat m2mc_format;
	BPXL_Format format;
	uint32_t offset;
	uint32_t pitch;
	void *pMemory;

	BM2MC_PACKET_PacketOutputFeeder *pPacket = (BM2MC_PACKET_PacketOutputFeeder *) (*ppPacket);
	BSUR_Surface_GetFormat( surface, &format );
	BSUR_Surface_GetAddress( surface, &pMemory, &pitch );
	BSUR_Surface_GetOffset( surface, &offset );
	BGRC_Packet_ConvertPixelFormat( &m2mc_format, format );

	BM2MC_PACKET_INIT( pPacket, OutputFeeder, false );
	pPacket->plane.address = offset;
	pPacket->plane.pitch = pitch;
	pPacket->plane.format = m2mc_format;
	pPacket->plane.width = 0;
	pPacket->plane.height = 0;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetOutputPlanePacket( BGRC_Handle hGrc, void **ppPacket, 
	const BM2MC_PACKET_Plane *pPlane )
{
	BM2MC_PACKET_PacketOutputFeeder *pPacket = (BM2MC_PACKET_PacketOutputFeeder *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, OutputFeeder, false );
	pPacket->plane = *pPlane;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetOutputOffsetPacket( BGRC_Handle hGrc, void **ppPacket, 
	BM2MC_PACKET_PixelFormat format, uint32_t offset, uint16_t pitch )
{
	BM2MC_PACKET_PacketOutputFeeder *pPacket = (BM2MC_PACKET_PacketOutputFeeder *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, OutputFeeder, false );
	pPacket->plane.address = offset;
	pPacket->plane.pitch = pitch;
	pPacket->plane.format = format;
	pPacket->plane.width = 0;
	pPacket->plane.height = 0;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetOutputControl( BGRC_Handle hGrc, void **ppPacket, 
	bool dither, bool chroma_filter )
{
	BM2MC_PACKET_PacketOutputControl *pPacket = (BM2MC_PACKET_PacketOutputControl *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, OutputControl, false );
	pPacket->dither = dither;
	pPacket->chroma_filter = chroma_filter;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetBlendPacket( BGRC_Handle hGrc, void **ppPacket, 
	BM2MC_PACKET_Blend *pColor, BM2MC_PACKET_Blend *pAlpha, uint32_t color )
{
	BM2MC_PACKET_PacketBlend *pPacket = (BM2MC_PACKET_PacketBlend *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, Blend, false );
	pPacket->color_blend = *pColor;
	pPacket->alpha_blend = *pAlpha;
	pPacket->color = color;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetRopPacket( BGRC_Handle hGrc, void **ppPacket, 
	uint8_t rop, uint32_t* pattern, uint32_t color0, uint32_t color1 )
{
	BM2MC_PACKET_PacketRop *pPacket = (BM2MC_PACKET_PacketRop *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, Rop, false );
	pPacket->rop = rop;
	pPacket->pattern0 = pattern ? pattern[0] : 0;
	pPacket->pattern1 = pattern ? pattern[1] : 0;
	pPacket->color0 = color0;
	pPacket->color1 = color1;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetSourceColorkeyPacket( BGRC_Handle hGrc, void **ppPacket, 
	bool enable, uint32_t high, uint32_t low, uint32_t mask, uint32_t replacement, uint32_t replacement_mask )
{
	BM2MC_PACKET_PacketSourceColorkeyEnable *pPacket = (BM2MC_PACKET_PacketSourceColorkeyEnable *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, SourceColorkeyEnable, false );
	pPacket->enable = enable ? 1 : 0;
	BM2MC_PACKET_TERM( pPacket );
	*ppPacket = (void *) pPacket;

	if( enable )
	{
		BM2MC_PACKET_PacketSourceColorkey *pPacket = (BM2MC_PACKET_PacketSourceColorkey *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, SourceColorkey, false );
		pPacket->high = high;
		pPacket->low = low;
		pPacket->mask = mask;
		pPacket->replacement = replacement;
		pPacket->replacement_mask = replacement_mask;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetDestinationColorkeyPacket( BGRC_Handle hGrc, void **ppPacket, 
	bool enable, uint32_t high, uint32_t low, uint32_t mask, uint32_t replacement, uint32_t replacement_mask )
{
	BM2MC_PACKET_PacketDestinationColorkeyEnable *pPacket = (BM2MC_PACKET_PacketDestinationColorkeyEnable *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, DestinationColorkeyEnable, false );
	pPacket->enable = enable ? 1 : 0;
	BM2MC_PACKET_TERM( pPacket );
	*ppPacket = (void *) pPacket;

	if( enable )
	{
		BM2MC_PACKET_PacketDestinationColorkey *pPacket = (BM2MC_PACKET_PacketDestinationColorkey *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, DestinationColorkey, false );
		pPacket->high = high;
		pPacket->low = low;
		pPacket->mask = mask;
		pPacket->replacement = replacement;
		pPacket->replacement_mask = replacement_mask;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetFilterPacket( BGRC_Handle hGrc, void **ppPacket, 
	BGRC_FilterCoeffs hor, BGRC_FilterCoeffs ver, BM2MC_PACKET_Rectangle *pSrcRect, BM2MC_PACKET_Rectangle *pOutRect )
{
	bool enable = (hor != BGRC_FilterCoeffs_ePointSample) || (ver != BGRC_FilterCoeffs_ePointSample);

	BM2MC_PACKET_PacketFilterEnable *pPacket = (BM2MC_PACKET_PacketFilterEnable *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, FilterEnable, false );
	pPacket->enable = enable ? 1 : 0;
	BM2MC_PACKET_TERM( pPacket );
	*ppPacket = (void *) pPacket;

	{
		BM2MC_PACKET_PacketFilter *pPacket = (BM2MC_PACKET_PacketFilter *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, Filter, false );
		BGRC_Packet_ConvertFilter( &pPacket->hor, hor, pSrcRect->width, pOutRect->width );
		BGRC_Packet_ConvertFilter( &pPacket->ver, ver, pSrcRect->height, pOutRect->height );
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetColorMatrixPacket( BGRC_Handle hGrc, void **ppPacket, 
	const int32_t matrix[], uint32_t shift )
{
	BM2MC_PACKET_PacketSourceColorMatrixEnable *pPacket = (BM2MC_PACKET_PacketSourceColorMatrixEnable *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, SourceColorMatrixEnable, false );
	pPacket->enable = matrix ? 1 : 0;
	BM2MC_PACKET_TERM( pPacket );
	*ppPacket = (void *) pPacket;

	if( matrix )
	{
		BM2MC_PACKET_PacketSourceColorMatrix *pPacket = (BM2MC_PACKET_PacketSourceColorMatrix *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, SourceColorMatrix, false );
		BGRC_Packet_ConvertColorMatrix( &pPacket->matrix, matrix, shift );
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetSourcePalette( BGRC_Handle hGrc, void **ppPacket,
	BSUR_Surface_Handle surface )
{
	if( surface )
	{
		BSUR_Palette_Handle palette = 0;
		BSUR_Surface_GetPalette( surface, &palette );
		if( palette )
		{
			uint32_t offset = 0;
			BSUR_Palette_GetOffset( palette, &offset );
			if( offset )
			{
				BM2MC_PACKET_PacketSourcePalette *pPacket = (BM2MC_PACKET_PacketSourcePalette *) (*ppPacket);
				BM2MC_PACKET_INIT( pPacket, SourcePalette, false );
				pPacket->address = offset;
				BM2MC_PACKET_TERM( pPacket );
				*ppPacket = (void *) pPacket;
			}
		}
	}
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetAlphaPremultiply( BGRC_Handle hGrc, void **ppPacket, bool enable )
{
	BM2MC_PACKET_PacketAlphaPremultiply *pPacket = (BM2MC_PACKET_PacketAlphaPremultiply *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, AlphaPremultiply, false );
	pPacket->enable = enable ? 1 : 0;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetMirrorPacket( BGRC_Handle hGrc, void **ppPacket,
	bool srcHor, bool srcVer, bool dstHor, bool dstVer, bool outHor, bool outVer )
{
	BM2MC_PACKET_PacketMirror *pPacket = (BM2MC_PACKET_PacketMirror *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, Mirror, false );
	pPacket->src_hor = srcHor;
	pPacket->src_ver = srcVer;
	pPacket->dst_hor = dstHor;
	pPacket->dst_ver = dstVer;
	pPacket->out_hor = outHor;
	pPacket->out_ver = outVer;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetFixedScalePacket( BGRC_Handle hGrc, void **ppPacket,
	int32_t hor_phase, int32_t ver_phase, uint32_t hor_step, uint32_t ver_step, uint32_t shift )
{
	BM2MC_PACKET_PacketFixedScale *pPacket = (BM2MC_PACKET_PacketFixedScale *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, FixedScale, false );
	pPacket->hor_phase = hor_phase;
	pPacket->ver_phase = ver_phase;
	pPacket->hor_step = hor_step;
	pPacket->ver_step = ver_step;
	pPacket->shift = shift;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetDestripeFixedScalePacket( BGRC_Handle hGrc, void **ppPacket, 
	BM2MC_PACKET_Rectangle *pChromaRect, int32_t hor_luma_phase, int32_t ver_luma_phase, int32_t hor_chroma_phase, int32_t ver_chroma_phase, 
	uint32_t hor_luma_step, uint32_t ver_luma_step, uint32_t shift )
{
	BM2MC_PACKET_PacketDestripeFixedScale *pPacket = (BM2MC_PACKET_PacketDestripeFixedScale *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, DestripeFixedScale, false );
	pPacket->chroma_rect = *pChromaRect;
	pPacket->hor_luma_phase = hor_luma_phase;
	pPacket->ver_luma_phase = ver_luma_phase;
	pPacket->hor_chroma_phase = hor_chroma_phase;
	pPacket->ver_chroma_phase = ver_chroma_phase;
	pPacket->hor_luma_step = hor_luma_step;
	pPacket->ver_luma_step = ver_luma_step;
	pPacket->shift = shift;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetBlitPacket( BGRC_Handle hGrc, void **ppPacket,
	BM2MC_PACKET_Rectangle *pSrcRect, BM2MC_PACKET_Point *pOutPoint, BM2MC_PACKET_Point *pDstPoint )
{
	if( pDstPoint )
	{
		BM2MC_PACKET_PacketBlendBlit *pPacket = (BM2MC_PACKET_PacketBlendBlit *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, BlendBlit, true );
		pPacket->src_rect = *pSrcRect;
		pPacket->out_point = *pOutPoint;
		pPacket->dst_point = *pDstPoint;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	else if( pOutPoint )
	{
		BM2MC_PACKET_PacketCopyBlit *pPacket = (BM2MC_PACKET_PacketCopyBlit *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, CopyBlit, true );
		pPacket->src_rect = *pSrcRect;
		pPacket->out_point = *pOutPoint;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	else
	{
		BM2MC_PACKET_PacketFillBlit *pPacket = (BM2MC_PACKET_PacketFillBlit *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, FillBlit, true );
		pPacket->rect = *pSrcRect;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetScaleBlitPacket( BGRC_Handle hGrc, void **ppPacket,
	BM2MC_PACKET_Rectangle *pSrcRect, BM2MC_PACKET_Rectangle *pOutRect, BM2MC_PACKET_Point *pDstPoint )
{
	if( pDstPoint )
	{
		BM2MC_PACKET_PacketScaleBlendBlit *pPacket = (BM2MC_PACKET_PacketScaleBlendBlit *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, ScaleBlendBlit, true );
		pPacket->src_rect = *pSrcRect;
		pPacket->out_rect = *pOutRect;
		pPacket->dst_point = *pDstPoint;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	else
	{
		BM2MC_PACKET_PacketScaleBlit *pPacket = (BM2MC_PACKET_PacketScaleBlit *) (*ppPacket);
		BM2MC_PACKET_INIT( pPacket, ScaleBlit, true );
		pPacket->src_rect = *pSrcRect;
		pPacket->out_rect = *pOutRect;
		BM2MC_PACKET_TERM( pPacket );
		*ppPacket = (void *) pPacket;
	}
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetDestripeBlitPacket( BGRC_Handle hGrc, void **ppPacket,
	BM2MC_PACKET_Rectangle *pSrcRect, BM2MC_PACKET_Rectangle *pOutRect, 
	uint32_t stripeWidth, uint32_t lumaStripeHeight, uint32_t chromaStripeHeight )
{
	BM2MC_PACKET_PacketDestripeBlit *pPacket = (BM2MC_PACKET_PacketDestripeBlit *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, DestripeBlit, true );
	pPacket->src_rect = *pSrcRect;
	pPacket->out_rect = *pOutRect;
	pPacket->dst_point = *((BM2MC_PACKET_Point *) pOutRect);
	pPacket->source_stripe_width = stripeWidth;
	pPacket->luma_stripe_height = lumaStripeHeight;
	pPacket->chroma_stripe_height = chromaStripeHeight;
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetResetState( BGRC_Handle hGrc, void **ppPacket )
{
	BM2MC_PACKET_PacketResetState *pPacket = (BM2MC_PACKET_PacketResetState *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, ResetState, false );
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetSaveState( BGRC_Handle hGrc, void **ppPacket )
{
	BM2MC_PACKET_PacketSaveState *pPacket = (BM2MC_PACKET_PacketSaveState *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, SaveState, false );
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/*****************************************************************************/
BERR_Code BGRC_Packet_SetRestoreState( BGRC_Handle hGrc, void **ppPacket )
{
	BM2MC_PACKET_PacketRestoreState *pPacket = (BM2MC_PACKET_PacketRestoreState *) (*ppPacket);
	BM2MC_PACKET_INIT( pPacket, RestoreState, false );
	BM2MC_PACKET_TERM( pPacket );

	*ppPacket = (void *) pPacket;
	BSTD_UNUSED(hGrc);
	return BERR_SUCCESS;
}

/* End of File */
