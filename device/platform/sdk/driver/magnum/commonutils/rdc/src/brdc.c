/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brdc.c $
 * $brcm_Revision: Hydra_Software_Devel/148 $
 * $brcm_Date: 10/16/12 3:16p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/rdc/src/brdc.c $
 * 
 * Hydra_Software_Devel/148   10/16/12 3:16p hongtaoz
 * SW7445-54, SW7445-14: fixed typo of BVNF_8 bint id; added 7445 RDC
 * support;
 * 
 * Hydra_Software_Devel/147   9/20/12 4:28p yuxiaz
 * SW7425-1835, SW7425-3928: Add API to get max value for RDC timer.
 * BRDC_Slot_GetTimerSnapshot_isr now returns timer snapshot in
 * microseconds.
 * 
 * Hydra_Software_Devel/146   8/15/12 6:09p tdo
 * SW7445-14: Add support for RDC
 *
 * Hydra_Software_Devel/145   8/13/12 3:21p yuxiaz
 * SW7425-3626: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 *
 * Hydra_Software_Devel/144   8/10/12 1:50p yuxiaz
 * SW7425-3626: Clean up RDC: Rename BRDC_List_GetStartCachedAddress_is to
 * BRDC_List_GetStartAddress_is, BRDC_Slot_SetCachedList_isr to
 * BRDC_Slot_SetList_isr, BRDC_Slot_SetCachedListDual_isr to
 * BRDC_Slot_SetListDual_isr.
 *
 * Hydra_Software_Devel/143   7/30/12 4:17p tdo
 * SW7445-14: Add support for RDC
 *
 * Hydra_Software_Devel/142   7/13/12 11:00a tdo
 * SW7563-8: Add support for RDC
 *
 * Hydra_Software_Devel/141   5/31/12 4:42p syang
 * SW7425-2093: fix build warning
 *
 * Hydra_Software_Devel/140   5/30/12 7:39p syang
 * SW7425-2093: add scratch reg alloc track msg
 *
 * Hydra_Software_Devel/139   4/27/12 9:27a pntruong
 * SW7584-4: Correct trigger number.
 *
 * Hydra_Software_Devel/138   4/26/12 3:05p pntruong
 * SW7584-4: Initial support.
 *
 * Hydra_Software_Devel/137   12/16/11 7:41p bselva
 * SW7360-6: Added appframework support for 7360 platform
 *
 * Hydra_Software_Devel/137   12/16/11 7:32p bselva
 * SW7360-6: Added appframework support for 7360 platform
 *
 * Hydra_Software_Devel/136   11/1/11 9:55a pntruong
 * SW7435-23: Initial rdc support for 7435.
 *
 * Hydra_Software_Devel/135   10/6/11 2:13p pntruong
 * SW7429-16: Initial support.
 *
 * Hydra_Software_Devel/134   8/11/11 9:50p pntruong
 * SW7231-318: Initial vdc support with error free build.
 *
 * Hydra_Software_Devel/134   8/11/11 9:46p pntruong
 * SW7231-318: Initial vdc support with error free build.  Added hddvi
 * triggers.
 *
 * Hydra_Software_Devel/133   4/13/11 7:09p pntruong
 * SW7425-339: Fixed Ax build errors.
 *
 * Hydra_Software_Devel/132   4/13/11 6:01p pntruong
 * SW7425-339: Added support for new vec/cmp triggers.  Fixed force
 * trigger index to use mapping table instead of enum.
 *
 * Hydra_Software_Devel/131   3/15/11 4:43p pntruong
 * SWDTV-5971: Added support for the 35233 DTV chip.
 *
 * Hydra_Software_Devel/130   2/15/11 10:34a pntruong
 * SW7420-1456: Fixed coverity errors.
 *
 * Hydra_Software_Devel/129   2/9/11 3:34p pntruong
 * SW7420-1456: Initial standby power management that used chp's pm
 * functionalities.  Additional standardize coding style and uses of dbg
 * object.
 *
 * Hydra_Software_Devel/SW7420-1456/2   2/3/11 6:15p jtna
 * SW7420-1456: add BRDC_Standby and BRDC_Resume
 *
 * Hydra_Software_Devel/SW7420-1456/1   2/2/11 8:07p jtna
 * SW7420-1456: initial work on VDC/VBI power management
 *
 * Hydra_Software_Devel/128   11/30/10 2:13p pntruong
 * SWBLURAY-23673: Initial support.
 *
 * Hydra_Software_Devel/127   11/17/10 10:59p hongtaoz
 * SW7422-13, SW7425-25: bring up 7422 VEC 656out; 7422/25 have two
 * 656out, no 656in;
 *
 * Hydra_Software_Devel/126   11/8/10 10:10p pntruong
 * SW35125-21: Added 35135 support.
 *
 * Hydra_Software_Devel/125   10/29/10 12:11a pntruong
 * SW7231-2: Initial support.
 *
 * Hydra_Software_Devel/124   9/8/10 4:09p hongtaoz
 * SW7425-25: fixed trigger info for 7425;
 *
 * Hydra_Software_Devel/123   9/2/10 1:32p vanessah
 * SW7358-9: SW7425-25:  RDC support
 *
 * Hydra_Software_Devel/122   8/25/10 5:20p vanessah
 * SW7425-25:  Add support to 7346 7344 7231 7358 7552 7135
 *
 * Hydra_Software_Devel/120   6/22/10 4:14p pntruong
 * SW7422-12: Fixed the position of the new triggers.
 *
 * Hydra_Software_Devel/119   6/22/10 3:02p pntruong
 * SW7422-12: Fixed build errors.  Corrected naming to follow previous
 * naming convention.
 *
 * Hydra_Software_Devel/118   6/22/10 11:39a vanessah
 * SW7422-12:  To support appframework. Missing files added:
 * magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
 * do list: 1. in framework_board.c, more initialization to be done.  2.
 * More registers mapping, like clock generation as well as
 * BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
 *
 * Hydra_Software_Devel/117   5/26/10 5:22p darnstein
 * SW7335-696: the capture buffer used for debugging gets allocated in
 * BRDC_Open(). If this allocation fails, the error condition was being
 * ignored.
 *
 * Hydra_Software_Devel/116   3/18/10 11:37a yuxiaz
 * SW7405-3954: Remove dependence of brdc.h in bavc.h
 *
 * Hydra_Software_Devel/114   12/9/09 4:19p tdo
 * SW7420-497: Using physical address to program RDC blockout
 *
 * Hydra_Software_Devel/113   11/30/09 10:36a pntruong
 * SW35230-28: Added 35230 support.
 *
 * Hydra_Software_Devel/112   11/18/09 10:53a pntruong
 * SW7408-13: Fixed typo.
 *
 * Hydra_Software_Devel/111   11/18/09 10:28a pntruong
 * SW7408-13: Added initial Appframework/VDC support for 7408.
 *
 * Hydra_Software_Devel/110   11/16/09 4:01p rpan
 * SW7468-25: Initial support for 7468.
 *
 * Hydra_Software_Devel/109   10/22/09 5:09p pntruong
 * SW3548-2568: fixed build warings.
 *
 * Hydra_Software_Devel/108   9/2/09 5:30p albertl
 * SW7630-8: Added 7630 RDC support.
 *
 * Hydra_Software_Devel/107   8/25/09 7:25p albertl
 * SW7125-10: Initial 7125 support.
 *
 * Hydra_Software_Devel/106   7/28/09 1:50p pntruong
 * PR55225: add support for 7342 chipset.
 *
 * Hydra_Software_Devel/105   7/14/09 10:50a pntruong
 * PR53796: Adopted to rdb changed.
 *
 * Hydra_Software_Devel/104   6/23/09 11:50a pntruong
 * PR55793: Fixed coverity warnings.
 *
 * Hydra_Software_Devel/103   6/11/09 2:48p darnstein
 * PR55219: add trigger list for 7340 chipset.
 *
 * Hydra_Software_Devel/102   6/10/09 4:34p rpan
 * PR55793: Addressed a coverity issue.
 *
 * Hydra_Software_Devel/101   6/9/09 6:20p pntruong
 * PR55861: Added support for hw blockout and timestamp.
 *
 * Hydra_Software_Devel/100   6/9/09 5:35p pntruong
 * PR55861: VDC: Add initial support for appframework and vdc.
 *
 * Hydra_Software_Devel/99   6/2/09 4:17p pntruong
 * PR55533: Added preliminary support for 7550.
 *
 * Hydra_Software_Devel/98   5/27/09 4:29p rpan
 * PR55480: Added trigger assignment for ITU-656 output on 7400 and 7405.
 *
 * Hydra_Software_Devel/97   4/27/09 11:48a jhaberf
 * PR53796: Added BCM35130 DTV chip to list of supported chips for rdc.
 *
 * Hydra_Software_Devel/96   3/24/09 5:28p albertl
 * PR52513: Moved RDC debug globals into hList structure.
 *
 * Hydra_Software_Devel/95   3/23/09 9:34p tdo
 * PR52966: BRDC support for 7635 "Dune" chip
 *
 * Hydra_Software_Devel/94   2/12/09 4:41p tdo
 * PR51629: Correct RDC table size
 *
 * Hydra_Software_Devel/93   2/9/09 11:17a jrubio
 * PR51629: remove duplicate entry for 7336 P3XD
 *
 * Hydra_Software_Devel/92   2/6/09 12:37p tdo
 * PR51627: Update px3d trigger
 *
 * Hydra_Software_Devel/91   1/27/09 8:24p tdo
 * PR51627: add VDC 7336 PI support
 *
 * Hydra_Software_Devel/90   1/20/09 8:48p tdo
 * PR45792, PR45793: Remove warning
 *
 * Hydra_Software_Devel/89   1/20/09 8:30p tdo
 * PR45792, PR45793: Add Timer Service API to RDC. Add HW RDC timer
 * support.  Modify new RDC instruction.
 *
 * Hydra_Software_Devel/88   1/8/09 2:22p rpan
 * PR50927: Improved the RUL execution tracking mechanism to handle the
 * very first RUL.
 *
 * Hydra_Software_Devel/87   12/22/08 11:47p pntruong
 * PR50652: Added support for 7405c0.
 *
 * Hydra_Software_Devel/86   11/24/08 3:35p albertl
 * PR49393:  Changed BRDC_Slot_RulConfigRulSize_isr to use
 * BRDC_List_SetNumEntries_isr and return void.
 *
 * Hydra_Software_Devel/85   9/12/08 11:15a yuxiaz
 * PR46480: B0: Remove unused RDC trigger from list
 *
 * Hydra_Software_Devel/84   9/5/08 4:26p tdo
 * PR46484: Bringup appframework for 7420
 *
 * Hydra_Software_Devel/83   7/7/08 5:05p tdo
 * PR33685: BRDC support for 7601
 *
 * Hydra_Software_Devel/82   3/27/08 10:00p pntruong
 * PR41004: Fixed return code, and assert validity of input handles.
 *
 * Hydra_Software_Devel/81   3/10/08 8:08p jessem
 * PR 38623: Enclosed setting of new register blocks to blockout in a
 * critical section.
 *
 * Hydra_Software_Devel/80   2/28/08 5:00p jessem
 * PR 38623: Added RDMA blockout support.
 *
 * Hydra_Software_Devel/79   2/19/08 4:55p pntruong
 * PR38863, PR39416: Updated to include 656 output and px3d trigger.
 *
 * Hydra_Software_Devel/78   1/24/08 9:47a pntruong
 * PR38863: Initial 3548 suport.
 *
 * Hydra_Software_Devel/77   11/20/07 11:07p tdo
 * PR36889: Remove strictly alias warnings
 *
 * Hydra_Software_Devel/76   11/16/07 5:29p tdo
 * PR36898: Add VDC PI support for 7335
 *
 * Hydra_Software_Devel/75   10/11/07 6:17p syang
 * PR 35061:  coverity (REVERSE_INULL) fix
 *
 * Hydra_Software_Devel/74   10/5/07 6:43p pntruong
 * PR34714: Updated more trigger info.
 *
 * Hydra_Software_Devel/73   10/4/07 11:51a pntruong
 * PR34714: Update RDC trigger definitions, and removed warnings and
 * global vars.
 *
 * Hydra_Software_Devel/72   9/30/07 3:20p tdo
 * PR33685: Basic RDC support for 7443.
 *
 * Hydra_Software_Devel/71   9/21/07 3:40p hongtaoz
 * PR34955: added RUL size self config API to support PsF 1080p feature;
 *
 * Hydra_Software_Devel/70   9/20/07 5:20p yuxiaz
 * PR34527: Add 7325 support
 *
 * Hydra_Software_Devel/69   9/13/07 4:12p pntruong
 * PR 28750 : Added RDC hooks for RUL capture for splash screen.
 *
 ***************************************************************************/

#include "bstd.h"                /* standard types */
#include "breg_mem.h"            /* Chip register access memory mapped */
#include "bkni.h"                /* Memory management */
#include "bdbg.h"                /* Debug message */

#ifdef BCHP_PWR_SUPPORT
#include "bchp_pwr.h"
#endif

#include "brdc.h"
#include "brdc_dbg.h"
#include "brdc_private.h"
#include "brdc_blockout_priv.h"

/* Interrupt Id */
#include "bchp_int_id_bvnf_intr2_0.h"
#include "bchp_int_id_bvnf_intr2_1.h"
#if (BCHP_CHIP==7445)
#include "bchp_int_id_bvnf_intr2_8.h"
#include "bchp_int_id_bvnf_intr2_9.h"
#endif

BDBG_MODULE(BRDC);
BDBG_OBJECT_ID(BRDC_RDC);
BDBG_OBJECT_ID(BRDC_SLT);
BDBG_OBJECT_ID(BRDC_LST);


/* Support register udpate blockout by HW? */
#ifdef BCHP_RDC_br_0_start_addr
#define BRDC_P_SUPPORT_HW_BLOCKOUT         (1)
#else
#define BRDC_P_SUPPORT_HW_BLOCKOUT         (0)
#endif

#ifdef BCHP_RDC_desc_0_tm_snapshot
#define BRDC_P_SUPPORT_TIMESTAMP           (1)
#else
#define BRDC_P_SUPPORT_TIMESTAMP           (0)
#endif

#if(BRDC_P_SUPPORT_TIMESTAMP)
/* RDC timers is 32 bit */
#define BRDC_P_MAX_TIMER_COUNTER           (0xffffffff)

/* Chips (version earlier than  3.1.0) run at 108Mhz :
 * 7125, 7340, 7342, 7408 , 7420, 7468, 7550
 */
#if (BCHP_CHIP==7125) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || \
    (BCHP_CHIP==7408) || (BCHP_CHIP==7420) || (BCHP_CHIP==7468) || \
    (BCHP_CHIP==7550)
#define BRDC_P_TIMESTAMP_CLOCK_RATE        (108ul)
#else
/* Chips (version later than or equal to 3.1.0) run at 216Mhz */
#define BRDC_P_TIMESTAMP_CLOCK_RATE        (216ul)
#endif

#endif

/* Shorthands to reduce typing. */
#ifdef BCHP_INT_ID_BVNF_INTR2_8_RDC_DESC_32_INTR
#define BRDC_P_MAKE_SLOT_LO_INFO(slot_id)                           \
{                                                                   \
	(BCHP_INT_ID_BVNF_INTR2_0_RDC_DESC_##slot_id##_INTR),           \
	BRDC_P_TRACK_REG_ADDR(BRDC_P_SCRATCH_REG_START+(slot_id))       \
}
#define BRDC_P_MAKE_SLOT_HI_INFO(slot_id)                           \
{                                                                   \
	(BCHP_INT_ID_BVNF_INTR2_8_RDC_DESC_##slot_id##_INTR),           \
	BRDC_P_TRACK_REG_ADDR(BRDC_P_SCRATCH_REG_START+(slot_id))       \
}
#define BRDC_P_MAKE_TRIG_LO_INFO_NORM(enum_name, int_id, rdb_name)  \
{                                                                   \
	(BRDC_Trigger_##enum_name),                                     \
	(BCHP_INT_ID_BVNF_INTR2_1_RDC_TRIG_##int_id##_INTR),            \
	(BRDC_P_TRIGGER(rdb_name)),                                     \
	(#enum_name)                                                    \
}
#define BRDC_P_MAKE_TRIG_HI_INFO_NORM(enum_name, int_id, rdb_name)  \
{                                                                   \
	(BRDC_Trigger_##enum_name),                                     \
	(BCHP_INT_ID_BVNF_INTR2_9_RDC_TRIG_##int_id##_INTR),            \
	(BRDC_P_TRIGGER(rdb_name)),                                     \
	(#enum_name)                                                    \
}
#elif BCHP_INT_ID_BVNF_INTR2_0_RDC_DESC_0_INTR
#define BRDC_P_MAKE_SLOT_LO_INFO(slot_id)                           \
{                                                                   \
	(BCHP_INT_ID_BVNF_INTR2_0_RDC_DESC_##slot_id##_INTR),           \
	BRDC_P_TRACK_REG_ADDR(BRDC_P_SCRATCH_REG_START+(slot_id))       \
}
#define BRDC_P_MAKE_SLOT_HI_INFO(slot_id)                           \
{                                                                   \
	(BRDC_P_NULL_BINTID),                                           \
	BRDC_P_TRACK_REG_ADDR(BRDC_P_SCRATCH_REG_START+(slot_id))       \
}
#define BRDC_P_MAKE_TRIG_LO_INFO_NORM(enum_name, int_id, rdb_name)  \
{                                                                   \
	(BRDC_Trigger_##enum_name),                                     \
	(BCHP_INT_ID_BVNF_INTR2_1_RDC_TRIG_##int_id##_INTR),            \
	(BRDC_P_TRIGGER(rdb_name)),                                     \
	(#enum_name)                                                    \
}
#define BRDC_P_MAKE_TRIG_HI_INFO_NORM(enum_name, int_id, rdb_name)  \
{                                                                   \
	(BRDC_Trigger_##enum_name),                                     \
	(BRDC_P_NULL_BINTID),                                           \
	(BRDC_P_TRIGGER_UNKNOWN_VAL),                                   \
	(#enum_name)                                                    \
}
#else
#define BRDC_P_MAKE_SLOT_LO_INFO(slot_id)                           \
{                                                                   \
	(BCHP_INT_ID_RDC_DESC_##slot_id##_INTR),                        \
	BRDC_P_TRACK_REG_ADDR(BRDC_P_SCRATCH_REG_START+(slot_id))       \
}
#define BRDC_P_MAKE_SLOT_HI_INFO(slot_id)                           \
{                                                                   \
	(BRDC_P_NULL_BINTID),                                           \
	BRDC_P_TRACK_REG_ADDR(BRDC_P_SCRATCH_REG_START+(slot_id))       \
}
#define BRDC_P_MAKE_TRIG_LO_INFO_NORM(enum_name, int_id, rdb_name)  \
{                                                                   \
	(BRDC_Trigger_##enum_name),                                     \
	(BCHP_INT_ID_RDC_TRIG_##int_id##_INTR),                         \
	(BRDC_P_TRIGGER(rdb_name)),                                     \
	(#enum_name)                                                    \
}
#define BRDC_P_MAKE_TRIG_HI_INFO_NORM(enum_name, int_id, rdb_name)  \
{                                                                   \
	(BRDC_Trigger_##enum_name),                                     \
	(BRDC_P_NULL_BINTID),                                           \
	(BRDC_P_TRIGGER_UNKNOWN_VAL),                                   \
	(#enum_name)                                                    \
}
#endif

#define BRDC_P_MAKE_TRIG_INFO_COMB(enum_name, int_id, rdb_name)  \
{                                                                \
	(BRDC_Trigger_##enum_name),                                  \
	(BRDC_P_NULL_BINTID),                                        \
	(BRDC_P_TRIGGER(rdb_name)),                                  \
	(#enum_name)                                                 \
}

#define BRDC_P_MAKE_TRIG_LH_INFO_NULL(enum_name, int_id, rdb_name)  \
{                                                                \
	(BRDC_Trigger_##enum_name),                                  \
	(BRDC_P_NULL_BINTID),                                        \
	(BRDC_P_TRIGGER_UNKNOWN_VAL),                                \
	(#enum_name)                                                 \
}

/* Slot Info */
typedef struct
{
	/* When slot done execute it will fire interrupt, and this interrupt id
	 * is use in according with BINT (magnum interrupt manager). */
	BINT_Id            SlotIntId;

	/* This address is for the slot to update the count when it execute.  We're
	 * using this register to check if the RUL last assigned to slot has been
	 * executed or not. */
	uint32_t           ulTrackRegAddr;

} BRDC_P_SlotInfo;

/* INDEX: Slot id.  Slot's INT ID, tracking address mapping, */
static const BRDC_P_SlotInfo s_aRdcSlotInfo[] =
{
	BRDC_P_MAKE_SLOT_LO_INFO(0),
	BRDC_P_MAKE_SLOT_LO_INFO(1),
	BRDC_P_MAKE_SLOT_LO_INFO(2),
	BRDC_P_MAKE_SLOT_LO_INFO(3),
	BRDC_P_MAKE_SLOT_LO_INFO(4),

	BRDC_P_MAKE_SLOT_LO_INFO(5),
	BRDC_P_MAKE_SLOT_LO_INFO(6),
	BRDC_P_MAKE_SLOT_LO_INFO(7),
	BRDC_P_MAKE_SLOT_LO_INFO(8),
	BRDC_P_MAKE_SLOT_LO_INFO(9),

	BRDC_P_MAKE_SLOT_LO_INFO(10),
	BRDC_P_MAKE_SLOT_LO_INFO(11),
	BRDC_P_MAKE_SLOT_LO_INFO(12),
	BRDC_P_MAKE_SLOT_LO_INFO(13),
	BRDC_P_MAKE_SLOT_LO_INFO(14),

	BRDC_P_MAKE_SLOT_LO_INFO(15),
	BRDC_P_MAKE_SLOT_LO_INFO(16),
	BRDC_P_MAKE_SLOT_LO_INFO(17),
	BRDC_P_MAKE_SLOT_LO_INFO(18),
	BRDC_P_MAKE_SLOT_LO_INFO(19),

	BRDC_P_MAKE_SLOT_LO_INFO(20),
	BRDC_P_MAKE_SLOT_LO_INFO(21),
	BRDC_P_MAKE_SLOT_LO_INFO(22),
	BRDC_P_MAKE_SLOT_LO_INFO(23),
	BRDC_P_MAKE_SLOT_LO_INFO(24),

	BRDC_P_MAKE_SLOT_LO_INFO(25),
	BRDC_P_MAKE_SLOT_LO_INFO(26),
	BRDC_P_MAKE_SLOT_LO_INFO(27),
	BRDC_P_MAKE_SLOT_LO_INFO(28),
	BRDC_P_MAKE_SLOT_LO_INFO(29),

	BRDC_P_MAKE_SLOT_LO_INFO(30),
	BRDC_P_MAKE_SLOT_LO_INFO(31),
	BRDC_P_MAKE_SLOT_HI_INFO(32),
	BRDC_P_MAKE_SLOT_HI_INFO(33),
	BRDC_P_MAKE_SLOT_HI_INFO(34),

	BRDC_P_MAKE_SLOT_HI_INFO(35),
	BRDC_P_MAKE_SLOT_HI_INFO(36),
	BRDC_P_MAKE_SLOT_HI_INFO(37),
	BRDC_P_MAKE_SLOT_HI_INFO(38),
	BRDC_P_MAKE_SLOT_HI_INFO(39),

	BRDC_P_MAKE_SLOT_HI_INFO(40),
	BRDC_P_MAKE_SLOT_HI_INFO(41),
	BRDC_P_MAKE_SLOT_HI_INFO(42),
	BRDC_P_MAKE_SLOT_HI_INFO(43),
	BRDC_P_MAKE_SLOT_HI_INFO(44),

	BRDC_P_MAKE_SLOT_HI_INFO(45),
	BRDC_P_MAKE_SLOT_HI_INFO(46),
	BRDC_P_MAKE_SLOT_HI_INFO(47),
	BRDC_P_MAKE_SLOT_HI_INFO(48),
	BRDC_P_MAKE_SLOT_HI_INFO(49),

	BRDC_P_MAKE_SLOT_HI_INFO(50),
	BRDC_P_MAKE_SLOT_HI_INFO(51),
	BRDC_P_MAKE_SLOT_HI_INFO(52),
	BRDC_P_MAKE_SLOT_HI_INFO(53),
	BRDC_P_MAKE_SLOT_HI_INFO(54),

	BRDC_P_MAKE_SLOT_HI_INFO(55),
	BRDC_P_MAKE_SLOT_HI_INFO(56),
	BRDC_P_MAKE_SLOT_HI_INFO(57),
	BRDC_P_MAKE_SLOT_HI_INFO(58),
	BRDC_P_MAKE_SLOT_HI_INFO(59),

	BRDC_P_MAKE_SLOT_HI_INFO(60),
	BRDC_P_MAKE_SLOT_HI_INFO(61),
	BRDC_P_MAKE_SLOT_HI_INFO(62),
	BRDC_P_MAKE_SLOT_HI_INFO(63),

	/* NULL */
	{BRDC_P_NULL_BINTID,           BRDC_P_NO_TRACKING_ADDR}
};


/* INDEX: Trigger Id.  Trigger's INT ID!  This chip specifics.
 * DO NOT: ifdefs and nested ifdefs that become impossible to decipher.
 * Look at the BCHP_BRDC_desc_0_config for valid triggers. */
static const BRDC_TrigInfo s_aRdcTrigInfo[] =
{
	/*                    enum_name,    int_id,
	 *                                      rdb_name         */

#if (BCHP_CHIP==7038)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap4Trig0,   14, cap_4_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap4Trig1,   15, cap_4_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, dtg_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, dtg_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec0Trig0,  16, vdec_0_trig_0  ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec0Trig1,  17, vdec_0_trig_1  ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig0, 23, hd_dvi_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig1, 24, hd_dvi_1_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig1,   26, letterbox_1    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd0Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7438)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap4Trig0,   14, cap_4_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap4Trig1,   15, cap_4_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, dtg_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, dtg_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec0Trig0,  16, vdec_0_trig_0  ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec0Trig1,  17, vdec_0_trig_1  ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc1Trig,   27, m2mc_1_trig    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig0, 23, hd_dvi_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig1, 24, hd_dvi_1_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig1,   26, letterbox_1    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd0Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==3560)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, dtg_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, dtg_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec0Trig0,  16, vdec_0_trig_0  ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec0Trig1,  17, vdec_0_trig_1  ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec1Trig0,  27, vdec_1_trig_0  ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec1Trig1,  28, vdec_1_trig_1  ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig0, 23, hd_dvi_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig1, 24, hd_dvi_1_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    29, dvo_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    30, dvo_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd0Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==3563)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, dtg_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, dtg_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec0Trig0,  16, vdec_0_trig_0  ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec0Trig1,  17, vdec_0_trig_1  ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec1Trig0,  27, vdec_1_trig_0  ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec1Trig1,  28, vdec_1_trig_1  ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig0, 23, hd_dvi_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig1, 24, hd_dvi_1_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    29, dvo_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    30, dvo_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd0Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eAnr0Trig0,    6, anr_0_vpp_trig_0),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==3548) || (BCHP_CHIP==3556)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, dtg_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, dtg_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec0Trig0,  16, vdec_0_trig_0  ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVdec0Trig1,  17, vdec_0_trig_1  ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_0_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig0, 23, hd_dvi_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig1, 24, hd_dvi_1_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    29, dvo_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    30, dvo_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     27, mfd_0_eof_trig ),
#if (BCHP_VER >= BCHP_VER_B0)
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
#else
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     28, mfd_1_eof_trig ),
#endif
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eAnr0Trig0,    6, anr_0_mctf_trig),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(ePx3d0Trig0,  10, px3d_trig      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==35230) || (BCHP_CHIP==35125)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, vec_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, vec_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc0Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig0, 23, hd_dvi_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig1, 24, hd_dvi_1_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    29, dvo_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    30, dvo_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     27, mfd_0_eof_trig ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==35233)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, vec_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, vec_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig0, 23, hd_dvi_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi1Trig1, 24, hd_dvi_1_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    12, dvo_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    13, dvo_trig_1     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig2,    14, dvo_trig_2     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig3,    15, dvo_trig_3     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     27, mfd_0_eof_trig ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     28, mfd_1_eof_trig ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7118)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7346) ||(BCHP_CHIP==7344)||(BCHP_CHIP==7231)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, itu_r656_out_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, itu_r656_out_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
#if (BCHP_CHIP==7231) && (BCHP_VER >= BCHP_VER_B0)
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
#else
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
#endif
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig1,   26, letterbox_1    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    14, hd_dvi_out_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    15, hd_dvi_out_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7429)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig1,   26, letterbox_1    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig0,   8, vec_source_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig1,   9, vec_source_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig0,  10, vec_source_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig1,  11, vec_source_1_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7400)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec2Trig0,   16, ter_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec2Trig1,   17, ter_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, itu_r656_out_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, itu_r656_out_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_0_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6561Trig0,   27, itu_r656_1_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6561Trig1,   28, itu_r656_1_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc1Trig,   31, m2mc_trig_1    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig1,   26, letterbox_1    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7401) || (BCHP_CHIP==7403)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_0_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7405)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, itu_r656_out_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, itu_r656_out_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_0_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDgp0Trig0,   26, dgp_0_trigger  ),
#if (BCHP_VER >= BCHP_VER_C0)
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(ePx3d0Trig0,  21, px3d_trig      ),
#else
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
#endif
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7440)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc1Trig,   31, m2mc_1_trig    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7552) || (BCHP_CHIP==7358) || (BCHP_CHIP==7360)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    14, hd_dvi_out_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    15, hd_dvi_out_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7601)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc1Trig,   31, m2mc_1_trig    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    27, dvo_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    28, dvo_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7630)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig2,   12, prim_vec_trig_2),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig3,   13, prim_vec_trig_3),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig2,   14, sec_vec_trig_2 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig3,   15, sec_vec_trig_3 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc1Trig,   31, m2mc_1_trig    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    27, dvo_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    28, dvo_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7635)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc1Trig,   31, m2mc_1_trig    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    27, dvo_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    28, dvo_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7325) || (BCHP_CHIP==7335)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, itu_r656_out_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, itu_r656_out_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    14, hd_dvi_out_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    15, hd_dvi_out_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7336)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, itu_r656_out_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, itu_r656_out_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    14, hd_dvi_out_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    15, hd_dvi_out_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(ePx3d0Trig0,  21, px3d_trig      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7420)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec2Trig0,   16, ter_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec2Trig1,   17, ter_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, itu_r656_out_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, itu_r656_out_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_0_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6561Trig0,   27, itu_r656_1_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6561Trig1,   28, itu_r656_1_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig1,   26, letterbox_1    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    14, hd_dvi_out_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    15, hd_dvi_out_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(ePx3d0Trig0,  31, px3d_0         ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7422) || (BCHP_CHIP==7425)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap4Trig0,    8, cap_4_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap4Trig1,    9, cap_4_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),

#if (BCHP_VER >= BCHP_VER_B0)
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig1,   -1, UNKNOWN        ),
#else
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,   10, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,   11, prim_vec_trig_1),
#endif

	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),

#if (BCHP_VER >= BCHP_VER_B0)
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
#else
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   12, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   13, sec_vec_trig_1 ),
#endif
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),

#if (BCHP_VER >= BCHP_VER_B0)
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
#else
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    16, itu_r656_0_out_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    17, itu_r656_0_out_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtg1Trig0,   18, itu_r656_1_out_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtg1Trig1,   19, itu_r656_1_out_1),
#endif

	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   22, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 25, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 26, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),

#if (BCHP_VER >= BCHP_VER_B0)
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
#else
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eStg0Trig0,   23, stg_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eStg0Trig1,   24, stg_0_trig_1   ),
#endif

	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),

#if (BCHP_VER >= BCHP_VER_B0)
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
#else
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    20, hd_dvi_out_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    21, hd_dvi_out_1   ),
#endif

	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     28, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     29, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd2Eof,     30, mfd_2_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),

#if (BCHP_VER >= BCHP_VER_B0)
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig0,  10, vec_source_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig1,  11, vec_source_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig0,  12, vec_source_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig1,  13, vec_source_1_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_2Trig0,  14, vec_source_2_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_2Trig1,  15, vec_source_2_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_3Trig0,  16, vec_source_3_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_3Trig1,  17, vec_source_3_trig_1),
#else
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
#endif
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7435)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap4Trig0,    8, cap_4_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap4Trig1,    9, cap_4_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap5Trig0,   10, cap_5_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap5Trig1,   11, cap_5_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   24, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc1Trig,   27, m2mc_trig_1    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 25, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 26, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     28, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     29, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd2Eof,     30, mfd_2_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd3Eof,     31, mfd_3_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig0,  12, vec_source_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig1,  13, vec_source_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig0,  14, vec_source_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig1,  15, vec_source_1_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_2Trig0,  16, vec_source_2_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_2Trig1,  17, vec_source_2_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_3Trig0,  18, vec_source_3_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_3Trig1,  19, vec_source_3_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_4Trig0,  20, vec_source_4_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_4Trig1,  21, vec_source_4_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_5Trig0,  22, vec_source_5_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_5Trig1,  23, vec_source_5_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7550)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig0,    12, itu_r656_out_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDtgTrig1,    13, itu_r656_out_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_0_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6561Trig0,   27, itu_r656_1_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6561Trig1,   28, itu_r656_1_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig1,   26, letterbox_1    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    14, hd_dvi_out_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    15, hd_dvi_out_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(ePx3d0Trig0,  31, px3d_0         ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7340) || (BCHP_CHIP==7342)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    14, hd_dvi_out_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    15, hd_dvi_out_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(ePx3d0Trig0,  31, px3d_0         ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7125)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    14, hd_dvi_out_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    15, hd_dvi_out_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(ePx3d0Trig0,  31, px3d_0         ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7468)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    14, hd_dvi_out_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    15, hd_dvi_out_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(ePx3d0Trig0,  31, px3d_0         ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7408)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eLboxTrig0,   25, letterbox_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    14, hd_dvi_out_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    15, hd_dvi_out_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(ePx3d0Trig0,  31, px3d_0         ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7640)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig0,    8, prim_vec_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec0Trig1,    9, prim_vec_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig0,   10, sec_vec_trig_0 ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eVec1Trig1,   11, sec_vec_trig_1 ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig0,   18, itu_r656_0_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(e6560Trig1,   19, itu_r656_0_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc1Trig,   31, m2mc_1_trig    ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig0, 21, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eHdDvi0Trig1, 22, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig0,    27, dvo_trig_0     ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eDvoTrig1,    28, dvo_trig_1     ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7584)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd1Eof,     30, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig0,   8, vec_source_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig1,   9, vec_source_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig0,  10, vec_source_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig1,  11, vec_source_1_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7563)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap3Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap4Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap5Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap6Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCap7Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eM2mc0Trig,   20, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eM2mc1Trig,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi0Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eMfd0Eof,     29, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd1Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd2Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd3Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd4Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eMfd5Eof,     -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig0,   8, vec_source_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig1,   9, vec_source_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig0,  10, vec_source_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig1,  11, vec_source_1_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_2Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_3Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_4Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_5Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eCmp_6Trig1,  -1, UNKNOWN        ),

#elif (BCHP_CHIP==7445)

	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig0,    0, cap_0_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap0Trig1,    1, cap_0_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig0,    2, cap_1_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap1Trig1,    3, cap_1_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig0,    4, cap_2_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap2Trig1,    5, cap_2_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig0,    6, cap_3_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap3Trig1,    7, cap_3_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap4Trig0,    8, cap_4_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap4Trig1,    9, cap_4_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap5Trig0,   10, cap_5_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap5Trig1,   11, cap_5_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap6Trig0,   12, cap_6_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap6Trig1,   13, cap_6_trig_1   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap7Trig0,   14, cap_7_trig_0   ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCap7Trig1,   15, cap_7_trig_1   ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec0Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec1Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig2,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVec2Trig3,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtgTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDtg1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec0Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eVdec1Trig1,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6560Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(e6561Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eM2mc0Trig,   48, m2mc_trig_0    ),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eM2mc1Trig,   49, m2mc_trig_1    ),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eHdDvi0Trig0, 44, hd_dvi_0_trig_0),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eHdDvi0Trig1, 45, hd_dvi_0_trig_1),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig0, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eHdDvi1Trig1, -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eStg0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eLboxTrig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig0,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig1,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig2,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDvoTrig3,    -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eMfd0Eof,     52, mfd_0_eof      ),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eMfd1Eof,     53, mfd_1_eof      ),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eMfd2Eof,     54, mfd_2_eof      ),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eMfd3Eof,     55, mfd_3_eof      ),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eMfd4Eof,     56, mfd_4_eof      ),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eMfd5Eof,     57, mfd_5_eof      ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr0Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eAnr1Trig1,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(eDgp0Trig0,   -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(ePx3d0Trig0,  -1, UNKNOWN        ),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig0,  24, vec_source_0_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_0Trig1,  25, vec_source_0_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig0,  26, vec_source_1_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_1Trig1,  27, vec_source_1_trig_1),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_2Trig0,  28, vec_source_2_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_2Trig1,  29, vec_source_2_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_3Trig0,  30, vec_source_3_trig_0),
	BRDC_P_MAKE_TRIG_LO_INFO_NORM(eCmp_3Trig1,  31, vec_source_3_trig_0),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eCmp_4Trig0,  32, vec_source_4_trig_0),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eCmp_4Trig1,  33, vec_source_4_trig_0),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eCmp_5Trig0,  34, vec_source_5_trig_0),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eCmp_5Trig1,  35, vec_source_5_trig_0),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eCmp_6Trig0,  36, vec_source_6_trig_0),
	BRDC_P_MAKE_TRIG_HI_INFO_NORM(eCmp_6Trig1,  37, vec_source_6_trig_0),

#else
#error "Port reqired for RDC."
#endif

	/* Common to all chips then put here. */
	BRDC_P_MAKE_TRIG_INFO_COMB(eComboTrig0,  -1, comb_trig_0    ),
	BRDC_P_MAKE_TRIG_INFO_COMB(eComboTrig1,  -1, comb_trig_1    ),
	BRDC_P_MAKE_TRIG_INFO_COMB(eComboTrig2,  -1, comb_trig_2    ),
	BRDC_P_MAKE_TRIG_INFO_COMB(eComboTrig3,  -1, comb_trig_3    ),
	BRDC_P_MAKE_TRIG_LH_INFO_NULL(UNKNOWN,      -1, UNKNOWN        )
};

/* Count to make we have enough entries. */
#define BRDC_P_SLOT_COUNT \
	(sizeof(s_aRdcSlotInfo) / sizeof(BRDC_P_SlotInfo))

/* Count to make we have enough entries. */
#define BRDC_P_TRIGGER_COUNT \
	(sizeof(s_aRdcTrigInfo) / sizeof(BRDC_TrigInfo))

#ifdef IKOS_EMULATION
extern int timeout_ns(uint32_t size);    /* in ikos_main.c */
#endif

/***************************************************************************
 *
 */
BERR_Code BRDC_GetDefaultSettings
	( BCHP_Handle                      hChp,
	  BRDC_Settings                   *pRdcSettings )
{
	BDBG_ENTER(BRDC_GetDefaultSettings);

	BSTD_UNUSED(hChp); /* hush warnings */
	BKNI_Memset(pRdcSettings, 0, sizeof(BRDC_Settings));

	BDBG_LEAVE(BRDC_GetDefaultSettings);
	return BERR_SUCCESS ;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_Open
	( BRDC_Handle                     *phRdc,
	  BCHP_Handle                      hChp,
	  BREG_Handle                      hReg,
	  BMEM_Handle                      hMem,
	  const BRDC_Settings             *pRdcSettings )
{
	uint32_t i;
	BRDC_Handle    hRdc = NULL;
	BERR_Code      err = BERR_SUCCESS;
	int            eSlotId;

	BDBG_ENTER(BRDC_Open);

	/* Check input parameters */
	BDBG_ASSERT(hChp);
	BDBG_ASSERT(hReg);
	BDBG_ASSERT(hMem);

	/* Make sure the lookup table is correct size. */
	if((BRDC_P_TRIGGER_COUNT != (BRDC_Trigger_UNKNOWN+1)) ||
	   (BRDC_P_SLOT_COUNT != (BRDC_SlotId_eSlotMAX+1)))
	{
		BDBG_ERR(( "Table ill-constructed s_aRdcTrigInfo!" ));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Matched the enum order! */
	for(i = 0; i < BRDC_P_TRIGGER_COUNT; i++)
	{
		if(i != s_aRdcTrigInfo[i].eTrigger)
		{
			BDBG_WRN(("Bad trigger info constructed for: %s",
				s_aRdcTrigInfo[i].pchTrigName));
			break;
		}

		if(BRDC_P_TRIGGER_UNKNOWN_VAL != s_aRdcTrigInfo[i].ulTrigVal)
		{
			BDBG_MSG(("%15s = %d", s_aRdcTrigInfo[i].pchTrigName,
				s_aRdcTrigInfo[i].ulTrigVal));
		}
		else
		{
			BDBG_MSG(("%15s = %d (n/a)", s_aRdcTrigInfo[i].pchTrigName,
				s_aRdcTrigInfo[i].ulTrigVal));
		}
	}

	/* Create RDC handle */
	hRdc = (BRDC_Handle)BKNI_Malloc(sizeof(BRDC_P_Handle));
	if( !hRdc )
	{
		BDBG_ERR(( "Out of System Memory" ));
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Initialize context of RDC handle */
	BKNI_Memset((void*)hRdc, 0x0, sizeof(BRDC_P_Handle));
	BDBG_OBJECT_SET(hRdc, BRDC_RDC);

	/* Copy the Settings if non NULL */
	if(pRdcSettings)
	{
		hRdc->stRdcSettings = *pRdcSettings;
	}

	/* Initialize slots */
	for( eSlotId = BRDC_SlotId_eSlot0; eSlotId < BRDC_SlotId_eSlotMAX; eSlotId++ )
	{
		hRdc->bSlotUsed[eSlotId] = false;
		hRdc->apSlot[eSlotId]    = NULL;
	}

	/* Update RDC */
	hRdc->hReg = hReg;
	hRdc->hChp = hChp;
	hRdc->hMem = hMem;
	hRdc->aTrigInfo = s_aRdcTrigInfo;

	/* RDC requires power to BVN */
#ifdef BCHP_PWR_RESOURCE_BVN
	BCHP_PWR_AcquireResource(hRdc->hChp, BCHP_PWR_RESOURCE_BVN);
#endif
	err = BRDC_P_SoftReset(hRdc);
	if (err != BERR_SUCCESS)
	{
		goto error;
	}

	/* RDMA Block-out */
	err = BRDC_P_RdcBlockOutInit(hRdc);
	if (err != BERR_SUCCESS)
	{
		goto error;
	}

#ifdef BRDC_USE_CAPTURE_BUFFER
	err = BRDC_DBG_CreateCaptureBuffer(&hRdc->captureBuffer, BRDC_P_MAX_COUNT);
	if (err != BERR_SUCCESS)
	{
		goto error;
	}
#endif

	*phRdc = hRdc;

	BDBG_MSG(("slots=%d, start=%d, end=%d, avail=%d",
		BRDC_P_NUM_OF_SLOTS,
		BRDC_P_SCRATCH_REG_START,
		BRDC_P_SCRATCH_REG_END,
		BRDC_P_SCRATCH_FIRST_AVAILABLE));

	BDBG_LEAVE(BRDC_Open);
	return err;

error:
#ifdef BCHP_PWR_RESOURCE_BVN
	/* a failed open releases power */
	BCHP_PWR_ReleaseResource(hRdc->hChp, BCHP_PWR_RESOURCE_BVN);
#endif
	BDBG_OBJECT_DESTROY(hRdc, BRDC_RDC);
	BKNI_Free(hRdc);

	return err;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_Close
	( BRDC_Handle                      hRdc )
{
	int       eSlotId;
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_Close);

	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

	/* Check if all slots are freed */
	for( eSlotId = BRDC_SlotId_eSlot0; eSlotId < BRDC_SlotId_eSlotMAX; eSlotId++ )
	{
		if( hRdc->bSlotUsed[eSlotId] || (hRdc->apSlot[eSlotId] != NULL) )
		{
			BDBG_ERR(( "Leaked Resource: Slot %d is not free", eSlotId ));
			err = BERR_TRACE(BERR_LEAKED_RESOURCE);
			goto done;
		}
	}

	/* Close related RDMA block-out objects */
	err = BRDC_P_RdcBlockOutDestroy(hRdc);
	if (err != BERR_SUCCESS)
	{
		goto done;
	}

#ifdef BRDC_USE_CAPTURE_BUFFER
	BRDC_DBG_DestroyCaptureBuffer(&hRdc->captureBuffer);
#endif

#ifdef BCHP_PWR_RESOURCE_BVN
	BCHP_PWR_ReleaseResource(hRdc->hChp, BCHP_PWR_RESOURCE_BVN);
#endif
	BDBG_OBJECT_DESTROY(hRdc, BRDC_RDC);
	BKNI_Free(hRdc);

done:
	BDBG_LEAVE(BRDC_Close);
	return err;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_Standby
	( BRDC_Handle                      hRdc,
	  const BRDC_StandbySettings      *pSettings )
{
	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

	/* nothing to check here */
	BSTD_UNUSED(pSettings);
#ifdef BCHP_PWR_RESOURCE_BVN
	BCHP_PWR_ReleaseResource(hRdc->hChp, BCHP_PWR_RESOURCE_BVN);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_Resume
	( BRDC_Handle                      hRdc )
{
	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);
#ifdef BCHP_PWR_RESOURCE_BVN
	BCHP_PWR_AcquireResource(hRdc->hChp, BCHP_PWR_RESOURCE_BVN);
#endif
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
uint32_t BRDC_P_AllocScratchReg
	( BRDC_Handle                      hRdc,
	  const char*                      pchFilename, /* source filename where block is allocated from */
	  int                              iLine )      /* line number in file where allocation occurs */
{
	uint32_t ulIndex;
	uint32_t ulReg = 0;

	BDBG_ENTER(BRDC_AllocScratchReg);

	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

	BKNI_EnterCriticalSection();
	for(ulIndex = 0; ulIndex <= BRDC_P_SCRATCH_REG_END - BRDC_P_SCRATCH_FIRST_AVAILABLE; ulIndex++)
	{
		if(!hRdc->abScratchRegUsed[ulIndex])
		{
			hRdc->abScratchRegUsed[ulIndex] = true;
			ulReg = BRDC_P_SCRATCH_REG_ADDR(ulIndex + BRDC_P_SCRATCH_FIRST_AVAILABLE);
			break;
		}
	}
	BKNI_LeaveCriticalSection();

	BDBG_MSG(("BRDC_AllocScratchReg idx %d in %s at line %d",
		ulIndex + BRDC_P_SCRATCH_FIRST_AVAILABLE, pchFilename, iLine));
#if !(BDBG_DEBUG_BUILD)
	BSTD_UNUSED(pchFilename);
	BSTD_UNUSED(iLine);
#endif

	BDBG_LEAVE(BRDC_AllocScratchReg);
	return ulReg;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_FreeScratchReg
	( BRDC_Handle                         hRdc,
	  uint32_t                            ulReg )
{
	uint32_t ulIndex;
	BERR_Code err = BERR_SUCCESS;
	BDBG_ENTER(BRDC_FreeScratchReg);

	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

	if((ulReg > BRDC_P_SCRATCH_REG_ADDR(BRDC_P_SCRATCH_REG_END)) ||
	   (ulReg < BRDC_P_SCRATCH_REG_ADDR(BRDC_P_SCRATCH_FIRST_AVAILABLE)))
	{
		BDBG_ERR(( "Invalid parameter" ));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	ulIndex = (ulReg - BRDC_P_SCRATCH_REG_ADDR(BRDC_P_SCRATCH_FIRST_AVAILABLE)) / sizeof(uint32_t);
	BKNI_EnterCriticalSection();
	if(hRdc->abScratchRegUsed[ulIndex])
	{
		hRdc->abScratchRegUsed[ulIndex] = false;
	}
	else
	{
		BDBG_ERR(( "Scratch register 0x%x is not in use!", ulReg ));
		err = BERR_INVALID_PARAMETER;
	}
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BRDC_FreeScratchReg);
	return err;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_EnableSameTrigger_isr
	( BRDC_Handle                      hRdc,
	  bool                             bEnable )
{
	uint32_t          ulRegVal;
	BERR_Code         err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_EnableSameTrigger_isr);

	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

#if (!BRDC_P_SUPPORT_SEGMENTED_RUL)
	ulRegVal = BRDC_P_Read32(hRdc, BCHP_RDC_config);
	ulRegVal &= ~BCHP_MASK(RDC_config, same_trigger);
	ulRegVal |= BCHP_FIELD_DATA(RDC_config, same_trigger, bEnable);
	BRDC_P_Write32(hRdc, BCHP_RDC_config, ulRegVal);
#else
	BSTD_UNUSED(err);
	BSTD_UNUSED(bEnable);
	BSTD_UNUSED(ulRegVal);
#endif

	BDBG_LEAVE(BRDC_EnableSameTrigger_isr);
	return err;

}

/***************************************************************************
 *
 */
BERR_Code BRDC_SetComboTrigMode_isr
	( BRDC_Handle                      hRdc,
	  BRDC_Trigger                     eComboTrig,
	  BRDC_ComboTrigMode               eMode )
{
	uint32_t          ulRegVal;
	uint32_t          ulBitShift;
	uint32_t          ulBitMask = 0;
	BERR_Code         err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_SetComboTrigMode_isr);

	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

	if((eComboTrig < BRDC_Trigger_eComboTrig0) ||
	   (eComboTrig > BRDC_Trigger_eComboTrig3))
	{
		BDBG_ERR(( "Invalid parameter" ));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	ulBitShift = BCHP_SHIFT(RDC_config, trig_combine_mode) +
		eComboTrig - BRDC_Trigger_eComboTrig0;
	ulBitMask = 1 << ulBitShift;

	ulRegVal = BRDC_P_Read32(hRdc, BCHP_RDC_config);
	ulRegVal &= ~ulBitMask;

	ulBitMask = eMode << ulBitShift;
	ulRegVal |= ulBitMask;
	BRDC_P_Write32(hRdc, BCHP_RDC_config, ulRegVal);

	BDBG_LEAVE(BRDC_SetComboTrigMode_isr);
	return err;

}

/***************************************************************************
 *
 */
BERR_Code BRDC_SetComboTriggers_isr
	( BRDC_Handle                      hRdc,
	  BRDC_Trigger                     eComboTrig,
	  uint32_t                         ulTriggersMask )
{
	uint32_t          ulRegVal;
	BERR_Code         err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_SetComboTriggers_isr);

	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

	if((eComboTrig < BRDC_Trigger_eComboTrig0) ||
	   (eComboTrig > BRDC_Trigger_eComboTrig3))
	{
		BDBG_ERR(( "Invalid parameter" ));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	ulRegVal = BCHP_FIELD_DATA(RDC_comb_mask_0, mask, ulTriggersMask);
	BRDC_P_Write32(hRdc, BCHP_RDC_comb_mask_0 +
		(eComboTrig - BRDC_Trigger_eComboTrig0) * (BCHP_RDC_comb_mask_1 - BCHP_RDC_comb_mask_0),
		ulRegVal);

	BDBG_LEAVE(BRDC_SetComboTriggers_isr);
	return err;

}


/***************************************************************************
 *
 */
BERR_Code BRDC_SetRdcBlockOut
	( BRDC_Handle                      hRdc,
	  const BRDC_BlockOut             *pstBlockOut,
	  uint32_t                         ulRegBlock )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_SetRdcBlockOut);

	BDBG_ASSERT(pstBlockOut);
	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

	/* Check if registers are valid */
	err = BRDC_P_ValidateBlockOutRegisters(pstBlockOut, ulRegBlock);
	if (err != BERR_SUCCESS)
	{
		return err;
	}
	else
	{
		BKNI_EnterCriticalSection();

		/* Disable current blockout */
		hRdc->bRdcBlockOutEnabled = false;

		/* Store blockout info */
		BKNI_Memcpy((void *)&hRdc->astBlockOut[ulRegBlock], (void *)pstBlockOut, sizeof(BRDC_BlockOut));

		hRdc->bRdcBlockOutEnabled = true;

#ifdef BCHP_RDC_br_0_start_addr
		{
			uint32_t ulOffset = (BCHP_RDC_br_1_start_addr - BCHP_RDC_br_0_start_addr) * ulRegBlock;
			BRDC_P_Write32(hRdc, BCHP_RDC_br_0_start_addr + ulOffset, BRDC_REGISTER(pstBlockOut->ulStartRegAddr));
			BRDC_P_Write32(hRdc, BCHP_RDC_br_0_end_addr + ulOffset, BRDC_REGISTER(pstBlockOut->ulStartRegAddr + (pstBlockOut->ulBlockSize - 1) * sizeof(uint32_t)));
			BRDC_P_Write32(hRdc, BCHP_RDC_br_0_enable + ulOffset, pstBlockOut->bEnable);
		}
#endif

		BKNI_LeaveCriticalSection();
	}

	BDBG_LEAVE(BRDC_SetRdcBlockOut);
	return err;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_GetRdcBlockOut
	( BRDC_Handle                      hRdc,
	  BRDC_BlockOut                   *pstBlockOut,
	  uint32_t                         ulRegBlock )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_GetRdcBlockOut);

	BDBG_ASSERT(pstBlockOut);
	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

	BKNI_Memcpy((void *)pstBlockOut, (void *)&hRdc->astBlockOut[ulRegBlock], sizeof(BRDC_BlockOut));

	BDBG_LEAVE(BRDC_GetRdcBlockOut);
	return err;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_Trigger_Execute_isr
	( BRDC_Handle                      hRdc,
	  BRDC_Trigger                     eRDCTrigger )
{
	uint32_t          ulRegVal;
	BERR_Code         err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_Trigger_Execute_isr);

	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

	if(eRDCTrigger == BRDC_Trigger_UNKNOWN)
	{
		BDBG_ERR(( "Invalid parameter" ));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	ulRegVal = BRDC_P_Read32(hRdc, BCHP_RDC_force_trigger);
	ulRegVal &= ~BCHP_MASK(RDC_force_trigger, trigger_index);
	ulRegVal |= BCHP_FIELD_DATA(RDC_force_trigger, trigger_index, hRdc->aTrigInfo[eRDCTrigger].ulTrigVal);
	BRDC_P_Write32(hRdc, BCHP_RDC_force_trigger, ulRegVal);

	BDBG_LEAVE(BRDC_Trigger_Execute_isr);
	return err;

}


/***************************************************************************
 *
 */
const BRDC_TrigInfo* BRDC_Trigger_GetInfo
	( BRDC_Handle                      hRdc,
	  BRDC_Trigger                     eRDCTrigger )
{
	BDBG_ENTER(BRDC_Trigger_GetInfo);

	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

	if(BRDC_Trigger_UNKNOWN == eRDCTrigger)
	{
		BDBG_ERR(( "Invalid parameter."));
		return NULL;
	}

	/* Trigger not supported on this chip. */
	if(BRDC_P_NULL_BINTID == hRdc->aTrigInfo[eRDCTrigger].TrigIntId)
	{
		BDBG_ERR(("Invalid trigger!  (%s) not supported on this chip",
			hRdc->aTrigInfo[eRDCTrigger].pchTrigName));
		return NULL;
	}

	BDBG_LEAVE(BRDC_Trigger_GetInfo);
	return &(hRdc->aTrigInfo[eRDCTrigger]);

}


/***************************************************************************
 *
 */
BERR_Code BRDC_List_Create
	( BRDC_Handle                      hRdc,
	  uint32_t                         ulMaxEntries,
	  BRDC_List_Handle                *phList )
{
	BERR_Code         err = BERR_SUCCESS;
	BRDC_List_Handle  hList = NULL;

	BDBG_ENTER(BRDC_List_Create);

	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);
	BDBG_ASSERT(phList);

	/* (1) Create list handle */
	hList = (BRDC_List_Handle)BKNI_Malloc(sizeof(BRDC_P_List_Handle));
	if( !hList )
	{
		BDBG_ERR(( "Out of System Memory" ));
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Initialize context of list */
	BKNI_Memset((void*)hList, 0x0, sizeof(BRDC_P_List_Handle));
	BDBG_OBJECT_SET(hList, BRDC_LST);

	/* (2) Create memory for entries in list. Must be 256 bit aligned */
	hList->pulRULAddr = (uint32_t*) BMEM_AllocAligned(hRdc->hMem,
		sizeof(uint32_t)* ulMaxEntries, 5, 0);
	if( !hList->pulRULAddr )
	{
		BDBG_ERR(( "Out of Device Memory" ));
		BDBG_OBJECT_DESTROY(hList, BRDC_LST);
		BKNI_Free(hList);
		return BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
	}

	/* Initialized list */
	hList->hRdc              = hRdc;
	hList->ulEntries         = 0;
	hList->ulMaxEntries      = ulMaxEntries;
	hList->ulNumSlotAssigned = 0;
	hList->bLastExecuted     = false;

	err = BMEM_ConvertAddressToCached(hRdc->hMem, (void*)hList->pulRULAddr,
		(void**)((void *)&hList->pulRULCacheAddr));
	if (BERR_SUCCESS != err)
	{
		BDBG_ERR(( "Failed to convert address 0x%x to cached", (int)hList->pulRULAddr  ));
		BMEM_Free(hRdc->hMem, hList->pulRULAddr);
		BDBG_OBJECT_DESTROY(hList, BRDC_LST);
		BKNI_Free(hList);
		return BERR_TRACE(err);
	}

	err = BMEM_ConvertAddressToOffset(hRdc->hMem,
		(void*)hList->pulRULAddr, &hList->ulAddrOffset);
	if (BERR_SUCCESS != err)
	{
		BDBG_ERR(( "Failed to convert virtual address 0x%x to physical address", (int)hList->pulRULAddr  ));
		BMEM_Free(hRdc->hMem, hList->pulRULAddr);
		BDBG_OBJECT_DESTROY(hList, BRDC_LST);
		BKNI_Free(hList);
		return BERR_TRACE(err);;
	}
	hList->eNextEntry        = BRDC_DBG_ListEntry_eCommand;
	hList->pulCurListAddr    = NULL;
	hList->ulNumEntries      = 0;
	hList->ulCurrCommand     = 0;
	hList->iCommandIndex     = 0;
	hList->iDataCount        = 0;

#if (BCHP_CHIP==7038)
	/* RDC block cannot use RUL lists that beyond the 128M range.
	 * Check if the list is within the range */
	if( hList->ulAddrOffset > 0x8000000 )
	{
		BDBG_ERR(("RUL List beyong 128M range."));
		BMEM_Free(hRdc->hMem, hList->pulRULAddr);
		BDBG_OBJECT_DESTROY(hList, BRDC_LST);
		BKNI_Free(hList);
		return BERR_TRACE(BRDC_LIST_ERR_MEMORY_BEYOND_128M);
	}
#endif

	/* (3) Assigned slot linked-list */
	hList->pSlotAssigned = (BRDC_P_Slot_Head *) BKNI_Malloc(sizeof(BRDC_P_Slot_Head));
	if( hList->pSlotAssigned == NULL )
	{
		BMEM_Free(hRdc->hMem, hList->pulRULAddr);
		BKNI_Free(hList);
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BLST_D_INIT(hList->pSlotAssigned);

	/* Return to user */
	*phList = hList;

	BDBG_LEAVE(BRDC_List_Create);
	return err ;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_List_Destroy
	( BRDC_List_Handle                 hList )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_List_Destroy);

	BDBG_OBJECT_ASSERT(hList, BRDC_LST);

	if( (hList->ulNumSlotAssigned) ||
	    ((hList->pSlotAssigned) && (BLST_D_FIRST(hList->pSlotAssigned) != NULL)) )
	{
		BDBG_ERR(( "List is assgined to a slot" ));
		return BERR_TRACE(BRDC_LIST_ERR_ASSIGNED_TO_SLOT);
	}

#ifndef IKOS_EMULATION
	if( hList->pulRULAddr )
		BMEM_Free(hList->hRdc->hMem, hList->pulRULAddr);
#endif

	if( hList->pSlotAssigned )
	{
		BKNI_Free(hList->pSlotAssigned);
	}

	BKNI_Free(hList);

	BDBG_LEAVE(BRDC_List_Destroy);
	return err;
}


/***************************************************************************
 *
 */
uint32_t *BRDC_List_GetStartAddress_isr
	( BRDC_List_Handle                 hList )
{
	BDBG_ENTER(BRDC_List_GetStartAddress_isr);

	BDBG_OBJECT_ASSERT(hList, BRDC_LST);

	BDBG_LEAVE(BRDC_List_GetStartAddress_isr);
	return (hList->pulRULCacheAddr);
}


/***************************************************************************
 *
 */
BERR_Code BRDC_List_SetNumEntries_isr
	( BRDC_List_Handle                 hList,
	  uint32_t                         ulNumEntries )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_List_SetNumEntries_isr);

	BDBG_OBJECT_ASSERT(hList, BRDC_LST);

	if( ulNumEntries > hList->ulMaxEntries )
	{
		BDBG_ERR(( "More than Max Entries %d > %d ",
			ulNumEntries, hList->ulMaxEntries ));
		BDBG_ERR(("Memory overrun already done, system could be unstable"));
		return BERR_TRACE(BRDC_LIST_ERR_ENTRIES_MORE_THAN_MAX);
	}

	hList->ulEntries = ulNumEntries;

	BDBG_LEAVE(BRDC_List_SetNumEntries_isr);
	return err;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_List_GetNumEntries_isr
	( BRDC_List_Handle                 hList,
	  uint32_t                        *pulNumEntries )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_List_GetNumEntries_isr);

	BDBG_OBJECT_ASSERT(hList, BRDC_LST);

	if( pulNumEntries )
	{
		*pulNumEntries = hList->ulEntries;
	}

	BDBG_LEAVE(BRDC_List_GetNumEntries_isr);
	return err;
}


/***************************************************************************
 *
 */
BERR_Code BRDC_List_GetMaxEntries_isr
	( BRDC_List_Handle                 hList,
	  uint32_t                        *pulMaxEntries )
{
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_List_GetMaxEntries_isr);

	BDBG_OBJECT_ASSERT(hList, BRDC_LST);

	if( pulMaxEntries )
	{
		*pulMaxEntries = hList->ulMaxEntries;
	}

	BDBG_LEAVE(BRDC_List_GetMaxEntries_isr);
	return err;
}


/***************************************************************************
 *
 */
bool BRDC_List_GetLastExecStatus_isr
	( BRDC_List_Handle                 hList )
{
	BDBG_ENTER(BRDC_List_GetLastExecStatus_isr);
	BDBG_OBJECT_ASSERT(hList, BRDC_LST);
	BDBG_LEAVE(BRDC_List_GetLastExecStatus_isr);
	return hList->bLastExecuted;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_Slot_Create
	( BRDC_Handle                      hRdc,
	  BRDC_Slot_Handle                *phSlot )
{
	BERR_Code         err   = BERR_SUCCESS;
	BRDC_SlotId       eSlotId;
	BRDC_Slot_Handle  hSlot = NULL;

	BDBG_ENTER(BRDC_Slot_Create);

	BDBG_OBJECT_ASSERT(hRdc, BRDC_RDC);

	/* Create slot */
	hSlot = (BRDC_Slot_Handle)BKNI_Malloc(sizeof(BRDC_P_Slot_Handle));
	if( !hSlot )
	{
		BDBG_ERR(( "Out of System Memory" ));
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Initialize context of slot handle */
	BKNI_Memset((void*)hSlot, 0x0, sizeof(BRDC_P_Slot_Handle));
	BDBG_OBJECT_SET(hSlot, BRDC_SLT);

	/* Use critical section when find next available slot */
	BKNI_EnterCriticalSection();
	err = BERR_TRACE(BRDC_Slot_P_GetNextSlot(hRdc, &eSlotId));
	BKNI_LeaveCriticalSection();
	if( err != BERR_SUCCESS )
	{
		BKNI_Free(hSlot);
		goto done;
	}

	if(s_aRdcSlotInfo[eSlotId].SlotIntId == BRDC_P_NULL_BINTID)
	{
		BKNI_Free(hSlot);
		goto done;
	}

	/* Assign the slot to the calling module */
	hSlot->eSlotId     = eSlotId;
	hSlot->hRdc        = hRdc;
	hSlot->bRecurring  = false;
	hSlot->eRDCTrigger = BRDC_Trigger_UNKNOWN;
	hSlot->hList       = NULL;
	hSlot->ulRegOffset = ((BCHP_RDC_desc_1_addr - BCHP_RDC_desc_0_addr) *
		(eSlotId - BRDC_SlotId_eSlot0));
	hSlot->SlotIntId   = s_aRdcSlotInfo[eSlotId].SlotIntId;

	/* Keeping track if last RUL executed. */
	if(BRDC_P_NO_TRACKING_ADDR != s_aRdcSlotInfo[eSlotId].ulTrackRegAddr)
	{
#ifdef BRDC_DISABLE_TRACK_EXECUTION
		hSlot->bTrackExecution  = false ;
#else
		hSlot->bTrackExecution  = true ;
#endif
		hSlot->ulTrackCount     = 0;
		hSlot->ulTrackRegAddr   = s_aRdcSlotInfo[eSlotId].ulTrackRegAddr;
		BDBG_MSG(("Creating slot[%d] with track executions!", hSlot->eSlotId));
	}
	else
	{
		BDBG_WRN(("Creating slot[%d] without track executions!", hSlot->eSlotId));
	}

	/* Update RDC */
	hRdc->bSlotUsed[eSlotId] = true;
	hRdc->apSlot[eSlotId] = hSlot;

	/* All done */
	*phSlot = hSlot;

done:
	BDBG_LEAVE(BRDC_Slot_Create);
	return err ;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_Slot_Destroy
	( BRDC_Slot_Handle                 hSlot )
{
	BERR_Code         err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_Slot_Destroy);

	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);

	/* Disable the slot */
	BKNI_EnterCriticalSection();
	err = BERR_TRACE(BRDC_Slot_Disable_isr(hSlot));
	BKNI_LeaveCriticalSection();
	if( err != BERR_SUCCESS )
		goto done;

	/* Update RDC: This slot becomes available */
	hSlot->hRdc->bSlotUsed[hSlot->eSlotId] = false;
	hSlot->hRdc->apSlot[hSlot->eSlotId]    = NULL;

	/* Clear list from the slot */
	if( hSlot->hList )
	{
		hSlot->hList->ulNumSlotAssigned--;

		/* Remove slot */
		BLST_D_REMOVE(hSlot->hList->pSlotAssigned, hSlot, link);
	}

	/* Free slot */
	BKNI_Free(hSlot);

done:
	BDBG_LEAVE(( "BRDC_Slot_Destroy" ));
	return err;
}


/***************************************************************************
 *
 */
BERR_Code BRDC_Slot_GetId
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_SlotId                     *pSlotId )
{
	BDBG_ENTER(BRDC_Slot_GetId);

	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);

	*pSlotId = hSlot->eSlotId;

	BDBG_LEAVE(BRDC_Slot_GetId);
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
BERR_Code BRDC_Slot_GetList_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                *phList )
{
	/* return list handle */
	*phList = hSlot->hList;

	/* success */
	return BERR_SUCCESS;
}

/***************************************************************************
 * This function calls the following _isr functions:
 *      BRDC_P_AcquireSemaphore_isr
 *      BRDC_P_ReleaseSemaphore_isr
 */
BERR_Code BRDC_P_Slot_SetList_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                 hList )
{
	uint32_t ulRegVal;
	BERR_Code  err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_P_Slot_SetList_isr);

	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);
	BDBG_OBJECT_ASSERT(hList, BRDC_LST);
	BDBG_ASSERT(hList->hRdc == hSlot->hRdc);

	/* Can't set empty list. */
	if( hList->ulEntries < 1 )
	{
		BDBG_ERR(( "Empty List" ));
		return BERR_TRACE(BRDC_SLOT_ERR_EMPTY_LIST);
	}

#if(!BRDC_P_SUPPORT_HW_BLOCKOUT)
	if (BRDC_P_IsRdcBlockOutEnabled(hList->hRdc))
	{
		err = BRDC_P_ParseAndReplaceRul_isr(hList);
		if (err != BERR_SUCCESS)
			goto done;
	}
#endif

#ifdef BRDC_USE_CAPTURE_BUFFER
	/* Write to log before RUL could possibly be executed */
	BRDC_DBG_WriteCapture_isr(&hList->hRdc->captureBuffer, hSlot, hList);
#endif

	/* Call the intercept if non NULL */
	if(hSlot->hRdc->stRdcSettings.pfnSlot_SetList_Intercept_isr)
	{
		if( (err = BERR_TRACE(hSlot->hRdc->stRdcSettings.pfnSlot_SetList_Intercept_isr(
			hSlot, hList, hSlot->eRDCTrigger) )) )
		{
			goto done;
		}
	}

	/* Acquire semaphore */
	err = BERR_TRACE(BRDC_P_AcquireSemaphore_isr(hSlot->hRdc, hSlot->eSlotId));
	if (err != BERR_SUCCESS)
	{
		/* Cannot acquire semaphore */
		BDBG_ERR(( "Cannot acquire semaphore for slot %d", hSlot->eSlotId ));
		goto done;
	}

	/* Get semaphore. Fill in hardware registers */
	/* Set RDC_desc_x_addr */
	BRDC_Slot_P_Write32(hSlot, BCHP_RDC_desc_0_addr + hSlot->ulRegOffset,
		hList->ulAddrOffset);

	/* Read RDC_desc_x_config */
	ulRegVal = BRDC_Slot_P_Read32(hSlot, BCHP_RDC_desc_0_config + hSlot->ulRegOffset);

	/* Clear count */
	ulRegVal &= ~BCHP_MASK(RDC_desc_0_config, count);

	/* Set count and update register */
	ulRegVal |= BCHP_FIELD_DATA(RDC_desc_0_config, count, hList->ulEntries -1);
	BRDC_Slot_P_Write32(hSlot, BCHP_RDC_desc_0_config + hSlot->ulRegOffset, ulRegVal);

	/* Release semaphore */
	BRDC_P_ReleaseSemaphore_isr(hSlot->hRdc, hSlot->eSlotId);

	/* Keep track of software copy */
	/* Already have list assigned to slot */
	if( hSlot->hList )
	{
		/* Clear the old list from slot */
		hSlot->hList->ulNumSlotAssigned--;

		/* Remove slot */
		BLST_D_REMOVE(hSlot->hList->pSlotAssigned, hSlot, link);
	}

	/* Assign list to slot */
	hList->ulNumSlotAssigned++;

	/* Add slot */
	BLST_D_INSERT_HEAD(hList->pSlotAssigned, hSlot, link);
	hSlot->hList = hList;

done:
	BDBG_LEAVE(BRDC_P_Slot_SetList_isr);
	return err;
}

/***************************************************************************
 * This function calls the following _isr functions:
 *      BRDC_P_AcquireSemaphore_isr
 *      BRDC_P_ReleaseSemaphore_isr
 * This function set a list to dual slots
 */
BERR_Code BRDC_P_Slot_SetListDual_isr
	( BRDC_Slot_Handle                 hSlot1,
	  BRDC_Slot_Handle                 hSlot2,
	  BRDC_List_Handle                 hList )
{
	uint32_t ulRegVal;
	BERR_Code  err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_P_Slot_SetListDual_isr);

	BDBG_ASSERT(hSlot1);
	BDBG_ASSERT(hSlot2);
	BDBG_OBJECT_ASSERT(hList, BRDC_LST);
	BDBG_ASSERT(hList->hRdc == hSlot1->hRdc);
	BDBG_ASSERT(hList->hRdc == hSlot2->hRdc);

	/* Can't set empty list. */
	if( hList->ulEntries < 1 )
	{
		BDBG_ERR(( "Empty List" ));
		return BERR_TRACE(BRDC_SLOT_ERR_EMPTY_LIST);
	}

#if(!BRDC_P_SUPPORT_HW_BLOCKOUT)
	if (BRDC_P_IsRdcBlockOutEnabled(hList->hRdc))
	{
		err = BRDC_P_ParseAndReplaceRul_isr(hList);
		if (err != BERR_SUCCESS)
			goto done;
	}
#endif

#ifdef BRDC_USE_CAPTURE_BUFFER
	/* Write to log before RUL could possibly be executed;
	 * log one slot only to reduce log size; */
	BRDC_DBG_WriteCapture_isr(&hList->hRdc->captureBuffer, hSlot1, hList);
#endif

	/* Call the intercept if non NULL */
	if(hSlot1->hRdc->stRdcSettings.pfnSlot_SetList_Intercept_isr)
	{
		if( (err = BERR_TRACE(hSlot1->hRdc->stRdcSettings.pfnSlot_SetList_Intercept_isr(
			hSlot1, hList, hSlot1->eRDCTrigger) )) )
		{
			goto done;
		}
		if( (err = BERR_TRACE(hSlot2->hRdc->stRdcSettings.pfnSlot_SetList_Intercept_isr(
			hSlot2, hList, hSlot2->eRDCTrigger) )) )
		{
			goto done;
		}
	}

	/********************** slot 1 start *****************/
	/* Acquire dual semaphores */
	err = BERR_TRACE(BRDC_P_AcquireSemaphore_isr(hSlot1->hRdc, hSlot1->eSlotId));
	if (err != BERR_SUCCESS)
	{
		/* Cannot acquire semaphore */
		BDBG_ERR(( "Cannot acquire semaphore for slot %d", hSlot1->eSlotId ));
		goto done;
	}

	/* Get semaphore. Fill in hardware registers */
	/* Set RDC_desc_x_addr */
	BRDC_Slot_P_Write32(hSlot1, BCHP_RDC_desc_0_addr + hSlot1->ulRegOffset,
		hList->ulAddrOffset);

	/* Read RDC_desc_x_config */
	ulRegVal = BRDC_Slot_P_Read32(hSlot1, BCHP_RDC_desc_0_config + hSlot1->ulRegOffset);

	/* Clear count */
	ulRegVal &= ~BCHP_MASK(RDC_desc_0_config, count);

	/* Set count and update register */
	ulRegVal |= BCHP_FIELD_DATA(RDC_desc_0_config, count, hList->ulEntries -1);
	BRDC_Slot_P_Write32(hSlot1, BCHP_RDC_desc_0_config + hSlot1->ulRegOffset, ulRegVal);

	/* Release semaphore */
	BRDC_P_ReleaseSemaphore_isr(hSlot1->hRdc, hSlot1->eSlotId);
	/********************** slot 1 end *****************/

	/********************** slot 2 start *****************/
	err = BERR_TRACE(BRDC_P_AcquireSemaphore_isr(hSlot2->hRdc, hSlot2->eSlotId));
	if (err != BERR_SUCCESS)
	{
		/* Cannot acquire semaphore */
		BDBG_ERR(( "Cannot acquire semaphore for slot %d", hSlot2->eSlotId ));
		goto done;
	}

	BRDC_Slot_P_Write32(hSlot2, BCHP_RDC_desc_0_addr + hSlot2->ulRegOffset,
		hList->ulAddrOffset);

	/* Read RDC_desc_x_config */
	ulRegVal = BRDC_Slot_P_Read32(hSlot2, BCHP_RDC_desc_0_config + hSlot2->ulRegOffset);

	/* Clear count */
	ulRegVal &= ~BCHP_MASK(RDC_desc_0_config, count);

	/* Set count and update register */
	ulRegVal |= BCHP_FIELD_DATA(RDC_desc_0_config, count, hList->ulEntries -1);
	BRDC_Slot_P_Write32(hSlot2, BCHP_RDC_desc_0_config + hSlot2->ulRegOffset, ulRegVal);

	/* Release semaphore */
	BRDC_P_ReleaseSemaphore_isr(hSlot2->hRdc, hSlot2->eSlotId);
	/********************** slot 2 end *****************/

	/* Keep track of software copy */
	/* Already have list assigned to slot */
	if( hSlot1->hList )
	{
		/* Clear the old list from slot */
		hSlot1->hList->ulNumSlotAssigned--;

		/* Remove slot */
		BLST_D_REMOVE(hSlot1->hList->pSlotAssigned, hSlot1, link);
	}
	if( hSlot2->hList )
	{
		/* Clear the old list from slot */
		hSlot2->hList->ulNumSlotAssigned--;

		/* Remove slot */
		BLST_D_REMOVE(hSlot2->hList->pSlotAssigned, hSlot2, link);
	}

	/* Assign list to slot */
	hList->ulNumSlotAssigned += 2;

	/* Add slot */
	BLST_D_INSERT_HEAD(hList->pSlotAssigned, hSlot2, link);
	hSlot2->hList = hList;
	BLST_D_INSERT_HEAD(hList->pSlotAssigned, hSlot1, link);
	hSlot1->hList = hList;

done:
	BDBG_LEAVE(BRDC_P_Slot_SetListDual_isr);
	return err;
}


/***************************************************************************
 * This function calls the following _isr functions:
 *      BRDC_P_Slot_SetList_isr
 */
BERR_Code BRDC_Slot_SetList_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                 hList )
{
	BERR_Code  err = BERR_SUCCESS;
	BDBG_ENTER(BRDC_Slot_SetList_isr);

	/* Flush the list before setting it to a slot. */

	/* Update the number of time this list, assigned to a slot. */
	if(hSlot->bTrackExecution)
	{
		uint32_t *pulStart   = hList->pulRULCacheAddr;
		uint32_t *pulCurrent = pulStart + hList->ulEntries;
		*pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pulCurrent++ = BRDC_REGISTER(hSlot->ulTrackRegAddr);
		*pulCurrent++ = ++(hSlot->ulTrackCount);
		hList->ulEntries = (uint32_t)(pulCurrent - pulStart);
	}

	BMEM_FlushCache_isr(hList->hRdc->hMem, hList->pulRULCacheAddr,
		hList->ulEntries * sizeof(uint32_t));

	err = BRDC_P_Slot_SetList_isr(hSlot, hList);
	BDBG_LEAVE(BRDC_Slot_SetList_isr);
	return err;
}

/***************************************************************************
 * This function calls the following _isr functions:
 *      BRDC_P_Slot_SetListDual_isr
 */
BERR_Code BRDC_Slot_SetListDual_isr
	( BRDC_Slot_Handle                 hSlot1,
	  BRDC_Slot_Handle                 hSlot2,
	  BRDC_List_Handle                 hList )
{
	uint32_t *pulStart   = hList->pulRULCacheAddr;
	uint32_t *pulCurrent = pulStart + hList->ulEntries;
	BERR_Code  err = BERR_SUCCESS;
	BDBG_ENTER(BRDC_Slot_SetListDual_isr);

	/* Update the number of time this list, assigned to a slot. */
	if(hSlot1->bTrackExecution)
	{
		*pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pulCurrent++ = BRDC_REGISTER(hSlot1->ulTrackRegAddr);
		*pulCurrent++ = ++(hSlot1->ulTrackCount);
		hList->ulEntries = (uint32_t)(pulCurrent - pulStart);
	}

	if(hSlot2->bTrackExecution)
	{
		*pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pulCurrent++ = BRDC_REGISTER(hSlot2->ulTrackRegAddr);
		*pulCurrent++ = ++(hSlot2->ulTrackCount);
		hList->ulEntries = (uint32_t)(pulCurrent - pulStart);
	}

	/* Flush the list before setting it to dual slots. */
	BMEM_FlushCache_isr(hList->hRdc->hMem, hList->pulRULCacheAddr,
		hList->ulEntries * sizeof(uint32_t));
	err = BRDC_P_Slot_SetListDual_isr(hSlot1, hSlot2, hList);

	BDBG_LEAVE(BRDC_Slot_SetListDual_isr);
	return err;
}

/***************************************************************************
 * This function calls the following _isr functions:
 *      BRDC_P_AcquireSemaphore_isr
 *      BRDC_P_ReleaseSemaphore_isr
 *      BRDC_Slot_P_Write_Registers_isr
 */
BERR_Code BRDC_Slot_ExecuteOnTrigger_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_Trigger                     eRDCTrigger,
	  bool                             bRecurring )
{
	BERR_Code         err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_Slot_ExecuteOnTrigger_isr);

	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);

	if( hSlot->hList->ulEntries < 1 )
	{
		BDBG_ERR(( "Empty List." ));
		return BERR_TRACE(BRDC_SLOT_ERR_EMPTY_LIST);
	}

	hSlot->eRDCTrigger = eRDCTrigger;

	/* Call the intercept if non NULL */
	if(hSlot->hRdc->stRdcSettings.pfnSlot_ExecuteOnTrigger_Intercept_isr)
	{
		if( (err = BERR_TRACE(hSlot->hRdc->stRdcSettings.pfnSlot_ExecuteOnTrigger_Intercept_isr(
			hSlot, eRDCTrigger, bRecurring))) )
		{
			goto done ;
		}
	}

	/* Acquire semaphore */
	err = BERR_TRACE(BRDC_P_AcquireSemaphore_isr(hSlot->hRdc, hSlot->eSlotId));
	if (err != BERR_SUCCESS)
	{
		/* Cannot acquire semaphore */
		goto done;
	}

	/* Fill in hardware registers */
	err = BERR_TRACE(BRDC_Slot_P_Write_Registers_isr(hSlot, eRDCTrigger,
		bRecurring, true));

	/* Release semaphore */
	BRDC_P_ReleaseSemaphore_isr(hSlot->hRdc, hSlot->eSlotId);

done:
	BDBG_LEAVE(BRDC_Slot_ExecuteOnTrigger_isr);
	return err;
}

/***************************************************************************
 * This function calls the following _isr functions:
 *      BRDC_Slot_P_Write_Registers_isr
 *      BRDC_P_AcquireSemaphore_isr
 *      BRDC_P_ReleaseSemaphore_isr
 */
BERR_Code BRDC_Slot_Execute_isr
	( BRDC_Slot_Handle                 hSlot )
{
	BERR_Code         err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_Slot_Execute_isr);

	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);

	if( hSlot->hList->ulEntries < 1 )
	{
		BDBG_ERR(( "Empty List." ));
		return BERR_TRACE(BRDC_SLOT_ERR_EMPTY_LIST);
	}

	/* Call the intercept if non NULL */
	if(hSlot->hRdc->stRdcSettings.pfnSlot_Execute_Intercept_isr)
	{
		if( (err = BERR_TRACE(hSlot->hRdc->stRdcSettings.pfnSlot_Execute_Intercept_isr(
			hSlot, hSlot->eRDCTrigger) )) )
		{
			goto done ;
		}
	}

	/* Acquire semaphore */
	err = BERR_TRACE(BRDC_P_AcquireSemaphore_isr(hSlot->hRdc, hSlot->eSlotId));
	if (err != BERR_SUCCESS)
	{
		/* Cannot acquire semaphore */
		goto done;
	}

	/* Fill in hardware registers */
	/* Don't need trigger, non-repeat */
	err = BERR_TRACE(BRDC_Slot_P_Write_Registers_isr(hSlot,
		BRDC_Trigger_UNKNOWN, false, false));

	/* Release semaphore */
	BRDC_P_ReleaseSemaphore_isr(hSlot->hRdc, hSlot->eSlotId);

done:
	BDBG_LEAVE(BRDC_Slot_Execute_isr);
	return err;

}

/***************************************************************************
 * This function calls the following _isr functions:
 *      BRDC_P_AcquireSemaphore_isr
 *      BRDC_P_ReleaseSemaphore_isr
 */
BERR_Code BRDC_Slot_Disable_isr
	( BRDC_Slot_Handle                 hSlot )
{
	bool              bSlotRecurring;
	uint32_t          ulRegVal;
	BERR_Code         err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_Slot_Disable_isr);

	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);

	/* Acquire semaphore */
	err = BERR_TRACE(BRDC_P_AcquireSemaphore_isr(hSlot->hRdc, hSlot->eSlotId));
	if (err != BERR_SUCCESS)
	{
		/* Cannot acquire semaphore */
		goto done;
	}

	/* Fill in hardware registers */
	ulRegVal = BRDC_Slot_P_Read32(hSlot, BCHP_RDC_desc_0_config + hSlot->ulRegOffset);

	/* All RDC_desc_x_config bit definitions are same */
	bSlotRecurring = (bool)BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_config, repeat);
	if( bSlotRecurring )
	{
		/* All BCHP_RDC_desc_x_config bit definitions are same */
		ulRegVal &= ~BCHP_MASK(RDC_desc_0_config, enable);
		BRDC_Slot_P_Write32(hSlot, BCHP_RDC_desc_0_config + hSlot->ulRegOffset, ulRegVal);
	}

	/* Release semaphore */
	BRDC_P_ReleaseSemaphore_isr(hSlot->hRdc, hSlot->eSlotId);

done:
	BDBG_LEAVE(BRDC_Slot_Disable_isr);
	return err;

}


/***************************************************************************
 *
 */
BINT_Id BRDC_Slot_GetIntId
	( BRDC_Slot_Handle                 hSlot )
{
	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);
	return hSlot->SlotIntId;
}


/***************************************************************************
 *
 */
bool BRDC_Slot_UpdateLastRulStatus_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                 hList,
	  bool                             bEnableTracking )
{
	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);
	BDBG_OBJECT_ASSERT(hList, BRDC_LST);

	if((hSlot->bTrackExecution != bEnableTracking) &&
	   (BRDC_P_NO_TRACKING_ADDR != hSlot->ulTrackRegAddr))
	{
#ifdef BRDC_DISABLE_TRACK_EXECUTION
		hSlot->bTrackExecution = false;
#else
		hSlot->bTrackExecution = bEnableTracking;
#endif
	}

	if(hSlot->bTrackExecution)
	{
		if (hSlot->ulTrackCount <= 1)
		{
			/* This is the first RUL and HW count tracking register
			 * is not programmed yet.
			 */
			hList->bLastExecuted = true;
		}
		else
		{
			uint32_t ulHwTrackCount =
				BREG_Read32(hSlot->hRdc->hReg, hSlot->ulTrackRegAddr);
			hList->bLastExecuted = (hSlot->ulTrackCount == ulHwTrackCount)
				? true : false;
		}
	}
	else
	{
		/* Default last executed if not tracking, true, there are code that
		 * won't move state unless it's certain that the slot last executed. */
		hList->bLastExecuted = true;
	}

	return hList->bLastExecuted;
}

/***************************************************************************
 * This function program RUL to config the length of the RUL;
 */
BERR_Code BRDC_Slot_RulConfigRulSize_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                 hList,
	  uint32_t                         ulCount )
{
	uint32_t *pulStart;
	uint32_t *pulCurrent;
	uint32_t  ulRegVal;

	BDBG_ENTER(BRDC_Slot_RulConfigRulSize_isr);

	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);
	BDBG_OBJECT_ASSERT(hList, BRDC_LST);
	BDBG_ASSERT(hList->hRdc == hSlot->hRdc);

	/* Can't set empty list. */
	if( hList->ulEntries < 1 )
	{
		BDBG_ERR(( "Empty List" ));
		return BERR_INVALID_PARAMETER;
	}

	/* Read RDC_desc_x_config */
	ulRegVal = BRDC_Slot_P_Read32(hSlot, BCHP_RDC_desc_0_config + hSlot->ulRegOffset);

	/* Clear count */
	ulRegVal &= ~BCHP_MASK(RDC_desc_0_config, count);

	/* Set count and update register */
	ulRegVal |= BCHP_FIELD_DATA(RDC_desc_0_config, count, ulCount-1);

	pulStart   = hList->pulRULCacheAddr;
	pulCurrent = pulStart + hList->ulEntries;

	/* hList RUL programs slave slot config register */
	*pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pulCurrent++ = BRDC_REGISTER(BCHP_RDC_desc_0_config + hSlot->ulRegOffset);
	*pulCurrent++ = ulRegVal;

	/* update list size */
	BRDC_List_SetNumEntries_isr(hList, (uint32_t)(pulCurrent - pulStart));

	BDBG_LEAVE(BRDC_Slot_RulConfigRulSize_isr);
	return BERR_SUCCESS;
}

/******************* Chained RUL programming *******************************/

/***************************************************************************
 * This function programs RUL to config the slave slot trigger;
 */
BERR_Code BRDC_Slot_RulConfigSlaveTrigger_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_Slot_Handle                 hSlotSlave,
	  BRDC_List_Handle                 hList,
	  BRDC_Trigger                     eRDCTrigger,
	  bool                             bRecurring )
{
	uint32_t ulTrigSelect;
	uint32_t *pulStart;
	uint32_t *pulCurrent;
	BERR_Code err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_Slot_RulConfigSlaveTrigger_isr);

	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);
	BDBG_OBJECT_ASSERT(hSlotSlave, BRDC_SLT);

	if( hList->ulEntries < 1 )
	{
		BDBG_ERR(( "Empty List." ));
		return BERR_TRACE(BRDC_SLOT_ERR_EMPTY_LIST);
	}

	/* Get trigger select value. */
	ulTrigSelect = hSlot->hRdc->aTrigInfo[eRDCTrigger].ulTrigVal;

	/* Update the desc_config register for trigger select. */
	pulStart   = hList->pulRULCacheAddr;
	pulCurrent = pulStart + hList->ulEntries;

	/* hList RUL programs slave slot config register */
	*pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pulCurrent++ = BRDC_REGISTER(BCHP_RDC_desc_0_config + hSlotSlave->ulRegOffset);
	*pulCurrent++ =
			BCHP_FIELD_DATA(RDC_desc_0_config, enable,         1            ) |
			BCHP_FIELD_DATA(RDC_desc_0_config, repeat,         bRecurring   ) |
			BCHP_FIELD_DATA(RDC_desc_0_config, trigger_select, ulTrigSelect ) |
			/* place holder for the real slave RUL size; */
			BCHP_FIELD_DATA(RDC_desc_0_config, count,          0 );
	hList->ulEntries = (uint32_t)(pulCurrent - pulStart);

	/* store the location of the previous desc_config setting; */
	hSlot->pulRulConfigPrevVal = hSlot->pulRulConfigVal;
	hSlot->pulRulConfigVal = pulCurrent - 1;

	/* if master slot, always use current pointer */
	if(hSlot != hSlotSlave) hSlot->pulRulConfigPrevVal = hSlot->pulRulConfigVal;

	BDBG_LEAVE(BRDC_Slot_RulConfigSlaveTrigger_isr);
	return err;
}

/***************************************************************************
 * This function program previous RUL to config the length of the next RUL;
 */
BERR_Code BRDC_Slot_RulConfigCount_isr
	( BRDC_Slot_Handle                 hSlot,
	  uint32_t                         ulCount )
{
	BERR_Code         err = BERR_SUCCESS;

	BDBG_ENTER(BRDC_Slot_RulConfigCount_isr);

	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);
	BDBG_ASSERT(hSlot->pulRulConfigPrevVal);

	/* (0) update previous RUL's desc_config setting, specifically 'count'; */
	*(hSlot->pulRulConfigPrevVal) = (*(hSlot->pulRulConfigPrevVal) &
		(~BCHP_MASK(RDC_desc_0_config, count))) |
		BCHP_FIELD_DATA(RDC_desc_0_config, count, ulCount - 1);

	BDBG_LEAVE(BRDC_Slot_RulConfigCount_isr);
	return err;
}

/***************************************************************************
 * This function build RUL to program slot descriptor to point to the immediate
 * next chained RUL;
 */
uint32_t BRDC_Slot_RulSetNextAddr_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                 hList )
{
	uint32_t *pulStart;
	uint32_t *pulCurrent;
	uint32_t  ulBitMask;
	uint32_t  ulScrap;

	BDBG_ENTER(BRDC_Slot_RulSetNextAddr_isr);

	BDBG_OBJECT_ASSERT(hSlot, BRDC_SLT);
	BDBG_OBJECT_ASSERT(hList, BRDC_LST);
	BDBG_ASSERT(hList->hRdc == hSlot->hRdc);

	/* Can't set empty list. */
	if( hList->ulEntries < 1 )
	{
		BDBG_ERR(( "Empty List" ));
		return 0;
	}

	/* Update the desc_addr to point to the next sub_RUL.
	 * NOTE: desc addr has to be 256-bit or 8-dword aligned!! */
	ulBitMask = (1 << 3) - 1;

	pulStart   = hList->pulRULCacheAddr;
	pulCurrent = pulStart + hList->ulEntries;

	*pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pulCurrent++ = BRDC_REGISTER(BCHP_RDC_desc_0_addr + hSlot->ulRegOffset);
	hList->ulEntries = (uint32_t)(pulCurrent + 1 - pulStart);
	*pulCurrent = hList->ulAddrOffset +
		((hList->ulEntries + ulBitMask) & (~ulBitMask)) * sizeof(uint32_t);

	/* return the scrap size in dwords */
	ulScrap = (hList->ulEntries & ulBitMask) ?
		(ulBitMask - (hList->ulEntries & ulBitMask) + 1) : 0;

	/* update list size */
	hList->ulEntries += ulScrap;

	BDBG_LEAVE(BRDC_Slot_RulSetNextAddr_isr);
	return ulScrap;
}

/***************************************************************************
 * This function build the master RUL to program the slave slot to point to
 * the head of the slave RULs
 */
BERR_Code BRDC_List_RulSetSlaveListHead_isr
	( BRDC_List_Handle                 hList,
	  BRDC_Slot_Handle                 hSlotSlave,
	  BRDC_List_Handle                 hListSlave )
{
	uint32_t *pulStart;
	uint32_t *pulCurrent;
	BERR_Code  err = BERR_SUCCESS;
	BDBG_ENTER(BRDC_List_RulSetSlaveListHead_isr);

	pulStart   = hList->pulRULCacheAddr;
	pulCurrent = pulStart + hList->ulEntries;

	/* (1) point desc_addr to the next RUL; CAREFUL!! */
	*pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pulCurrent++ = BRDC_REGISTER(BCHP_RDC_desc_0_addr + hSlotSlave->ulRegOffset);
	*pulCurrent++ = hListSlave->ulAddrOffset;
	hList->ulEntries = (uint32_t)(pulCurrent - pulStart);

	/* from this upon, the desc_config/addr have to be programmed by chained
	   RULs themselves; */
	BDBG_LEAVE(BRDC_List_RulSetSlaveListHead_isr);
	return err;
}

/***************************************************************************
 * This function build the tail of the chained slave-RULs:
 * it simply disable the slave slot itself;
 */
BERR_Code BRDC_Slot_RulConfigSlaveListTail_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                 hList )
{
	uint32_t ulTrigSelect;
	uint32_t *pulStart;
	uint32_t *pulCurrent;
	BERR_Code  err = BERR_SUCCESS;
	BDBG_ENTER(BRDC_Slot_RulConfigSlaveListTail_isr);

	/* Get trigger select value. */
	ulTrigSelect = hSlot->hRdc->aTrigInfo[BRDC_Trigger_UNKNOWN].ulTrigVal;

	pulStart   = hList->pulRULCacheAddr;
	pulCurrent = pulStart + hList->ulEntries;

	/* (1) disable desc_config  */
	*pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pulCurrent++ = BRDC_REGISTER(BCHP_RDC_desc_0_config + hSlot->ulRegOffset);
	*pulCurrent++ =
			BCHP_FIELD_DATA(RDC_desc_0_config, enable,         0            ) |
			BCHP_FIELD_DATA(RDC_desc_0_config, repeat,         0            ) |
			BCHP_FIELD_DATA(RDC_desc_0_config, trigger_select, ulTrigSelect ) |
			BCHP_FIELD_DATA(RDC_desc_0_config, count,          0            );

	hList->ulEntries = (uint32_t)(pulCurrent - pulStart);

	/* (4) update previous config pointer */
	hSlot->pulRulConfigPrevVal = hSlot->pulRulConfigVal;

	BDBG_LEAVE(BRDC_Slot_RulConfigSlaveListTail_isr);
	return err;
}

/***************************************************************************
 * This function flushes the cached list;
 */
BERR_Code BRDC_Slot_FlushCachedList_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                 hList )
{
	BERR_Code  err = BERR_SUCCESS;
	BSTD_UNUSED(hSlot);
	BDBG_ENTER(BRDC_Slot_FlushCachedList_isr);

	/* Flush the list before setting it to a slot. */

	/* flush the cache; don't change desc_config register; */
	BMEM_FlushCache_isr(hList->hRdc->hMem,
		hList->pulRULCacheAddr,
		hList->ulEntries * sizeof(uint32_t));
#ifdef BRDC_USE_CAPTURE_BUFFER
	/* Write to log before RUL could possibly be executed */
	BRDC_DBG_WriteCapture_isr(&hList->hRdc->captureBuffer, hSlot, hList);
#endif

	BDBG_LEAVE(BRDC_Slot_FlushCachedList_isr);
	return err;
}

/***************************************************************************
 * This function returns the timer snapshot in microseconds for the slot
 */
uint32_t BRDC_Slot_GetTimerSnapshot_isr
	( BRDC_Slot_Handle                 hSlot )
{
	uint32_t  ulTimer = 0;
	
#if(BRDC_P_SUPPORT_TIMESTAMP)
	uint32_t ulOffset = hSlot->eSlotId * (BCHP_RDC_desc_1_tm_snapshot - BCHP_RDC_desc_0_tm_snapshot);

	/* Get tick value */
	ulTimer = BRDC_Slot_P_Read32(hSlot, BCHP_RDC_desc_0_tm_snapshot + ulOffset);

	/* Convert to microseconds */
	ulTimer = ulTimer / BRDC_P_TIMESTAMP_CLOCK_RATE;
#else
	BSTD_UNUSED(hSlot);
#endif

	return ulTimer;
}

/***************************************************************************
 * This function returns the max value for RDC timer in microseconds
 */
uint32_t BRDC_GetTimerMaxValue
	( BRDC_Handle                      hRdc )
{
	BSTD_UNUSED(hRdc);

#if(BRDC_P_SUPPORT_TIMESTAMP)

	return (BRDC_P_MAX_TIMER_COUNTER / BRDC_P_TIMESTAMP_CLOCK_RATE);

#else

	BDBG_ERR(("RDC timer is not supported in HW"));
	return 0;
#endif
}

/* end of file */
