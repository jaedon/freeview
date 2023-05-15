/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brdc.h $
 * $brcm_Revision: Hydra_Software_Devel/97 $
 * $brcm_Date: 9/20/12 4:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/rdc/include/brdc.h $
 * 
 * Hydra_Software_Devel/97   9/20/12 4:28p yuxiaz
 * SW7425-1835, SW7425-3928: Add API to get max value for RDC timer.
 * BRDC_Slot_GetTimerSnapshot_isr now returns timer snapshot in
 * microseconds.
 * 
 * Hydra_Software_Devel/96   8/15/12 6:09p tdo
 * SW7445-14: Add support for RDC
 *
 * Hydra_Software_Devel/95   8/10/12 1:50p yuxiaz
 * SW7425-3626: Clean up RDC: Rename BRDC_List_GetStartCachedAddress_is to
 * BRDC_List_GetStartAddress_is, BRDC_Slot_SetCachedList_isr to
 * BRDC_Slot_SetList_isr, BRDC_Slot_SetCachedListDual_isr to
 * BRDC_Slot_SetListDual_isr.
 *
 * Hydra_Software_Devel/94   7/30/12 4:18p tdo
 * SW7445-14: Add support for RDC
 *
 * Hydra_Software_Devel/93   5/30/12 7:39p syang
 * SW7425-2093: add scratch reg alloc track msg
 *
 * Hydra_Software_Devel/92   11/1/11 9:55a pntruong
 * SW7435-23: Initial rdc support for 7435.
 *
 * Hydra_Software_Devel/91   8/8/11 6:18p vanessah
 * SW7425-923: rollback the STG trigger change
 *
 * Hydra_Software_Devel/90   8/8/11 5:56p vanessah
 * SW7425-923: add 2nd STG support
 *
 * Hydra_Software_Devel/89   4/13/11 6:48p pntruong
 * SW7425-339: Added support for new vec/cmp triggers.  Fixed force
 * trigger index to use mapping table instead of enum.
 *
 * Hydra_Software_Devel/88   3/15/11 4:43p pntruong
 * SWDTV-5971: Added support for the 35233 DTV chip.
 *
 * Hydra_Software_Devel/87   2/9/11 3:34p pntruong
 * SW7420-1456: Initial standby power management that used chp's pm
 * functionalities.  Additional standardize coding style and uses of dbg
 * object.
 *
 * Hydra_Software_Devel/SW7420-1456/1   2/3/11 6:14p jtna
 * SW7420-1456: rename VDC_656 to VDC_656_OUT
 *
 * Hydra_Software_Devel/86   11/17/10 11:00p hongtaoz
 * SW7422-13, SW7425-25: bring up 7422 VEC 656out; 7422/25 have two
 * 656out, no 656in;
 *
 * Hydra_Software_Devel/85   6/22/10 3:02p pntruong
 * SW7422-12: Fixed build errors.  Corrected naming to follow previous
 * naming convention.
 *
 * Hydra_Software_Devel/84   6/22/10 11:39a vanessah
 * SW7422-12:  To support appframework. Missing files added:
 * magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
 * do list: 1. in framework_board.c, more initialization to be done.  2.
 * More registers mapping, like clock generation as well as
 * BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
 *
 * Hydra_Software_Devel/83   3/18/10 11:37a yuxiaz
 * SW7405-3954: Remove dependence of brdc.h in bavc.h
 *
 * Hydra_Software_Devel/81   9/2/09 5:31p albertl
 * SW7630-8: Added 7630 RDC support.
 *
 * Hydra_Software_Devel/80   1/20/09 8:31p tdo
 * PR45792, PR45793: Add Timer Service API to RDC. Add HW RDC timer
 * support.  Modify new RDC instruction.
 *
 * Hydra_Software_Devel/79   11/24/08 3:35p albertl
 * PR49393:  Changed BRDC_Slot_RulConfigRulSize_isr to use
 * BRDC_List_SetNumEntries_isr and return void.
 *
 * Hydra_Software_Devel/78   9/30/08 8:45p tdo
 * PR45793: Add New RDC Instructions: SUM, SUM IMM, COND SKIP, SKIP, EXIT
 *
 * Hydra_Software_Devel/77   2/28/08 6:18p jessem
 * PR 38623: Corrected comments on BRDC_BlockOut struct.
 *
 * Hydra_Software_Devel/76   2/28/08 5:01p jessem
 * PR 38623: Added RDMA blockout support.
 *
 * Hydra_Software_Devel/75   2/19/08 4:56p pntruong
 * PR38863, PR39416: Updated to include 656 output and px3d trigger.
 *
 * Hydra_Software_Devel/74   9/21/07 3:40p hongtaoz
 * PR34955: added RUL size self config API to support PsF 1080p feature;
 *
 * Hydra_Software_Devel/73   9/13/07 4:13p pntruong
 * PR 28750 : Added RDC hooks for RUL capture for splash screen.
 *
 * Hydra_Software_Devel/PR28750/3   9/11/07 5:18p shyam
 * PR 28750 : Add error returns to intercepts
 *
 * Hydra_Software_Devel/PR28750/2   9/7/07 1:45p shyam
 * PR 28750 : Change to RDC hooks as function pointers
 *
 * Hydra_Software_Devel/72   4/16/07 10:55a yuxiaz
 * PR 29722: Add 7405 support.
 *
 * Hydra_Software_Devel/71   1/29/07 10:13p pntruong
 * PR26943: Update documentation in RDMA - cannot access registers outside
 * of BVN.
 *
 * Hydra_Software_Devel/70   12/21/06 7:11p hongtaoz
 * PR25668, PR22768: added API of RDC scratch registers for user to alloc
 * and free;
 *
 * Hydra_Software_Devel/69   12/21/06 1:07p hongtaoz
 * PR25668: h/w changed RDC scratch registers layout; bring up 7403;
 *
 * Hydra_Software_Devel/68   8/15/06 6:36p pntruong
 * PR23177:  Also need to map the unknown trigger for trigger_select.
 *
 * Hydra_Software_Devel/67   8/14/06 7:51p pntruong
 * PR23177: RDC module bringup.
 *
 * Hydra_Software_Devel/66   8/7/06 3:28p pntruong
 * PR23177: RDC module bringup.
 *
 * Hydra_Software_Devel/65   6/13/06 5:07p syang
 * PR 21689: add support for 7118
 *
 * Hydra_Software_Devel/64   4/25/06 2:29p jasonh
 * PR 20949: Fixed M2MC trigger.
 *
 * Hydra_Software_Devel/63   2/15/06 4:02p jasonh
 * PR 19691: Fixed VDEC and HD_DVI trigger names across all chipsets.
 *
 * Hydra_Software_Devel/62   2/14/06 1:25p jasonh
 * PR 19565: Adding support for 7438
 *
 * Hydra_Software_Devel/61   1/25/06 10:59p pntruong
 * PR19172: Black-screen with latest VDC on 97398.  The mosaic added 4
 * more slots for capture compositing surface  this pushes the hddvi slot
 * out, and not all slot has track execution.  This causes the update
 * format rul of hddvi not properly execute and cause hddvi not to lock.
 * Fixes by freeing non-used frame slot in vdec/656/hddvi, and free up
 * more rdc vars to be used for track execution.
 *
 * Hydra_Software_Devel/60   1/17/06 3:49p hongtaoz
 * PR19082: support 7400;
 *
 * Hydra_Software_Devel/59   1/13/06 11:58a jasonh
 * PR 19074: Temporary fix for M2MC trigger. Will be fixed later in RDB.
 *
 * Hydra_Software_Devel/58   1/12/06 1:31p hongtaoz
 * PR18233: added mosaic mode support;
 *
 * Hydra_Software_Devel/MosaicMode_Feature_PR18233/4   1/12/06 11:40a hongtaoz
 * PR18233: added mosaic mode API;
 *
 * Hydra_Software_Devel/57   1/3/06 3:00p yuxiaz
 * PR17593: Removed unused RDC task-context code. Use the _isr version of
 * functions for the non-_isr version.
 *
 * Hydra_Software_Devel/56   12/14/05 10:28a yuxiaz
 * PR15019: Return an error if the list address goes beyond 128M range.
 * Added a new RDC error code.
 *
 * Hydra_Software_Devel/55   10/7/05 4:46p hongtaoz
 * PR15945: added BRDC_Slot_SetListDual_isr and
 * BRDC_Slot_SetCachedListDual_isr support;
 *
 * Hydra_Software_Devel/54   7/18/05 6:05p hongtaoz
 * PR16012: added sec_vec for 7401 support;
 *
 * Hydra_Software_Devel/53   6/29/05 4:19p jasonh
 * PR 16012: Fixing 3560 chip detection.
 *
 * Hydra_Software_Devel/52   6/28/05 5:53p jasonh
 * PR 16012: Adding support for 7401 trigger mapping.
 *
 * Hydra_Software_Devel/51   3/21/05 2:27p hongtaoz
 * PR14494: modified RDC 'unknown' trigger enum value;
 *
 * Hydra_Software_Devel/50   3/17/05 6:44p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/49   3/10/05 3:31p jasonh
 * PR 14418: Fixed triggers for 3560
 *
 * Hydra_Software_Devel/48   2/22/05 5:26p jasonh
 * PR 13937: Provided reverse conversion macros and a function to get the
 * list active in a slot.
 *
 * Hydra_Software_Devel/47   11/29/04 4:57p pntruong
 * PR13076, PR11749:  No need to keep track of synclock dummy rul.
 *
 * Hydra_Software_Devel/46   11/23/04 8:53p pntruong
 * PR13076, PR11749: Video jitter under heavy system load.  Added RUL
 * execution check to reduce number of programmed registers.
 *
 * Hydra_Software_Devel/45   11/2/04 5:27p pntruong
 * PR13076: Added cached memory support for rul.
 *
 * Hydra_Software_Devel/44   10/29/04 3:55p hongtaoz
 * PR12162: adding rdc variables to time-stamp RUL execution time;
 *
 * Hydra_Software_Devel/43   10/28/04 4:07p yuxiaz
 * PR13108: Remove BKNI_Sleep from critical section and _isr functions.
 * Clean up _isr and non_isr functions.
 *
 * Hydra_Software_Devel/42   10/28/04 9:16a yuxiaz
 * PR13100: Create non _isr version of BRDC_List_GetStartAddress,
 * BRDC_Slot_SetList and BRDC_List_SetNumEntries. Change comments for
 * BRDC_Slot_GetId.
 *
 * Hydra_Software_Devel/41   9/21/04 4:24p yuxiaz
 * PR12728: fixed -pedantic warnings.
 *
 * Hydra_Software_Devel/40   6/25/04 3:27p yuxiaz
 * PR 11659: Rename RDC variables reserved for VDC to
 * BRDC_Variable_VDC_num. Move the defination of reserved variables back
 * to RDC.
 *
 * Hydra_Software_Devel/39   6/25/04 9:10a yuxiaz
 * PR 11659: Remove VDC reserved variables. Variables defined in RDC can
 * be used by all the moduals.
 *
 * Hydra_Software_Devel/38   6/17/04 4:05p pntruong
 * PR9957: Prevent create/destroy from effecting hardware in VDC.  Added
 * get interrupt id, so app can create corresponding callback.
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   4/28/04 9:47a yuxiaz
 * PR 10699: Added more RDC variables for multi-buffering debugging
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/15/04 10:15a pntruong
 * PR9813: Need to add initial support for 7038 B0.  Getting mvd_vdc_test
 * to display statics buffers.
 *
 * Hydra_Software_Devel/35   4/7/04 2:14p yuxiaz
 * PR 10404: Combine BRDC_Slot_SetNumEntries_isr and
 * BRDC_Slot_AssignList_isr into BRDC_Slot_SetList_isr so both RUL
 * address and count are set in one atomic function.
 *
 * Hydra_Software_Devel/34   3/23/04 8:21p pntruong
 * PR10219:  Safeguard RDC macro's arguments.
 *
 * Hydra_Software_Devel/33   3/16/04 2:51p yuxiaz
 * PR 10095: Updated document.
 *
 * Hydra_Software_Devel/32   1/21/04 10:30a yuxiaz
 * PR 9076: Change BRDC_Slot_AssignList to _isr for double buffer RUL.
 *
 * Hydra_Software_Devel/31   1/14/04 4:41p yuxiaz
 * PR 9076: Change isr functions to _isr.
 *
 * Hydra_Software_Devel/30   11/21/03 2:50p yuxiaz
 * Re-arrange order of parameters in BRDC_Open.
 *
 * Hydra_Software_Devel/29   10/30/03 2:45p yuxiaz
 * Remove bInterrupt from BRDC_Slot_Execute and
 * BRDC_Slot_ExecuteOnTrigger.
 *
 * Hydra_Software_Devel/28   10/27/03 3:13p yuxiaz
 * Added BRDC_Slot_SetNumEntries
 *
 * Hydra_Software_Devel/27   10/7/03 5:13p yuxiaz
 * Remove BRDC_Slot_TestAndClear.
 *
 * Hydra_Software_Devel/26   10/1/03 2:11p yuxiaz
 * Added BRDC_List_GetMaxEntries.
 *
 * Hydra_Software_Devel/25   9/26/03 5:19p yuxiaz
 * Rename BRDC_Execute_Trigger to BRDC_Trigger_Execute.
 *
 * Hydra_Software_Devel/24   9/26/03 1:50p jasonh
 * Changed argument documentation to be in-line and use in/out.
 *
 * Hydra_Software_Devel/23   9/24/03 4:55p yuxiaz
 * Cnt should be num - 1 in block, window write and copy command.
 *
 * Hydra_Software_Devel/22   9/23/03 5:51p pntruong
 * Removed the 7038_ from header filename generated from RDB.
 *
 * Hydra_Software_Devel/21   9/8/03 9:35a yuxiaz
 * Change unsigned int to uint32_t.
 *
 * Hydra_Software_Devel/20   9/2/03 2:55p yuxiaz
 * Added BRDC_Slot_GetId, moved BRDC_SlotId to brdc.h.
 *
 * Hydra_Software_Devel/19   8/14/03 8:58a yuxiaz
 * Clean up include files.
 *
 * Hydra_Software_Devel/18   8/5/03 3:27p yuxiaz
 * Added BRDC_EnableSameTrigger.
 *
 * Hydra_Software_Devel/17   7/31/03 2:16p pntruong
 * Added BCHP_PHYSICAL_OFFSET in rdb generated header.
 *
 * Hydra_Software_Devel/16   7/25/03 12:12p yuxiaz
 * Change BRDC_Trigger to use defines in RDB. Misc clean up.
 *
 * Hydra_Software_Devel/15   7/17/03 8:50a yuxiaz
 * Added debug message.
 *
 * Hydra_Software_Devel/14   7/14/03 10:25a yuxiaz
 * Change BCHP_PHYSICAL_OFFSET to 0x10000000.
 *
 * Hydra_Software_Devel/13   7/11/03 2:25p yuxiaz
 * Remove BERR_RDC_ID.
 *
 * Hydra_Software_Devel/12   7/10/03 8:41a yuxiaz
 * Rename BRDC_List_GetAddress to BRDC_List_GetStartAddress.
 *
 * Hydra_Software_Devel/11   7/7/03 2:51p yuxiaz
 * Added opcode. Misc clean up.
 *
 * Hydra_Software_Devel/10   7/1/03 4:21p yuxiaz
 * Added BRDC_Execute_Trigger.
 *
 * Hydra_Software_Devel/9   6/30/03 1:15p yuxiaz
 * Added BRDC_Trigger.
 *
 * Hydra_Software_Devel/8   6/27/03 2:54p yuxiaz
 * Added it to commonutils and fix build.
 *
 * Hydra_Software_Devel/7   6/26/03 3:00p yuxiaz
 * Added BRDC_List_GetNumEntries.
 *
 * Hydra_Software_Devel/6   6/18/03 2:15p jasonh
 * Fixed arguments to Open routine.
 *
 * Hydra_Software_Devel/5   6/16/03 3:23p jasonh
 * Changes made due to 2nd code review.
 *
 * Hydra_Software_Devel/4   6/11/03 7:29p jasonh
 * Changes made due to first code review.
 *
 * Hydra_Software_Devel/3   6/6/03 2:36p jasonh
 * Added list commands and flushed out documentation.
 *
 * Hydra_Software_Devel/2   6/2/03 11:48a jasonh
 * Changed RDA->RDC
 *
 * Hydra_Software_Devel/1   5/19/03 10:57a jasonh
 * Initial version.
 *
 ***************************************************************************/
#ifndef BRDC_H__
#define BRDC_H__


#include "bchp.h"                /* Chip information */
#include "berr_ids.h"            /* Error codes */
#include "bmem.h"                /* Chip memory access */
#include "bint.h"                /* Returning the interrupt ID of slot */

#include "bchp_rdc.h"            /* RDC trigger */
#include "bchp_common.h"         /* register physical offset */

#ifdef __cplusplus
extern "C" {
#endif

/*=Module Overview: ********************************************************
Register DMA is the process where commands to read, modify, and write
registers are contained in allocated lists and issued directly by
hardware when a trigger fires. This module provides the interface
for register DMA.

Register DMA can only be used for BVN operations with the current GISB
settings. If Register DMA tries to access a register outside BVN, such as
IFD regerters, a bus timeout interrupt will be generated.

The two basic types in the RDC are the list handle, BRDC_List_Handle, and
the slot handle, BRDC_Slot_Handle. A list handle points to an allocated
list of 32-bit entries capable of storing register DMA commands. When
the user wants to activate a given list, a list handle is assigned to a
slot handle. The slot handle is then triggered and issued by hardware.
Slots should be used by only one module at a time.

The commands within a list follow a specific pattern. Each command must
start with a command header, any of the macros starting with BRDC_OP,
and then are followed by other 32-bit data values. Each BRDC_OP describes
what must follow each command entry.

The basic process is as follows:

When a module needs to use register DMA, when the using module is
opened, it should allocate all lists and slots it needs with
BRDC_ListCreate and BRDC_SlotCreate.

When the module wants to issue a list, it should get the pointer to
list memory with a call to BRDC_List_GetStartAddress. Fill in the list with
any of the BRDC_OP commands and required data, notify RDC of the number of
entries in the list with BRDC_List_SetNumEntries_isr, assign the list to a slot
with BRDC_Slot_SetList_isr, and then trigger the list manually with
BRDC_SlotExecute or have hardware issue with a trigger by calling
BRDC_Slot_AssignTriggerToSlot.

Note that the most optimal command to use to update registers is
BRDC_OP_BLOCK_IMMS_TO_REGS. When possible, this command
should be used.

Note that since the BRDC_OP commands are macros, it is very possible
to statically create lists beforehand and copy them into allocated
lists. This is highly recommended.

When finished, the module should free all slots and lists with calls
to BRDC_List_Destroy and BRDC_Slot_Destroy.
****************************************************************************/

/***************************************************************************
Summary:
	List of errors unique to RDC
****************************************************************************/
#define BRDC_LIST_ERR_ASSIGNED_TO_SLOT         BERR_MAKE_CODE(BERR_RDC_ID, 0)
#define BRDC_LIST_ERR_ENTRIES_MORE_THAN_MAX    BERR_MAKE_CODE(BERR_RDC_ID, 1)
#define BRDC_LIST_ERR_MEMORY_BEYOND_128M       BERR_MAKE_CODE(BERR_RDC_ID, 2)

#define BRDC_SLOT_ERR_ALL_USED                 BERR_MAKE_CODE(BERR_RDC_ID, 3)
#define BRDC_SLOT_ERR_EMPTY_LIST               BERR_MAKE_CODE(BERR_RDC_ID, 4)
#define BRDC_SLOT_ERR_ACQUIRE_SEMAPHORE        BERR_MAKE_CODE(BERR_RDC_ID, 5)

/***************************************************************************
 * Enums
 ***************************************************************************/
/***************************************************************************
Summary:
	List of all slots used by RDC. Must match descriptors in RDB.

Description:

See Also:
	BRDC_Slot_GetId
****************************************************************************/
typedef enum
{
	BRDC_SlotId_eSlot0 = 0,  /* RDC Descriptor 0 */
	BRDC_SlotId_eSlot1,      /* RDC Descriptor 1 */
	BRDC_SlotId_eSlot2,      /* RDC Descriptor 2 */
	BRDC_SlotId_eSlot3,      /* RDC Descriptor 3 */
	BRDC_SlotId_eSlot4,      /* RDC Descriptor 4 */
	BRDC_SlotId_eSlot5,      /* RDC Descriptor 5 */
	BRDC_SlotId_eSlot6,      /* RDC Descriptor 6 */
	BRDC_SlotId_eSlot7,      /* RDC Descriptor 7 */
	BRDC_SlotId_eSlot8,      /* RDC Descriptor 8 */
	BRDC_SlotId_eSlot9,      /* RDC Descriptor 9 */
	BRDC_SlotId_eSlot10,     /* RDC Descriptor 10 */
	BRDC_SlotId_eSlot11,     /* RDC Descriptor 11 */
	BRDC_SlotId_eSlot12,     /* RDC Descriptor 12 */
	BRDC_SlotId_eSlot13,     /* RDC Descriptor 13 */
	BRDC_SlotId_eSlot14,     /* RDC Descriptor 14 */
	BRDC_SlotId_eSlot15,     /* RDC Descriptor 15 */
	BRDC_SlotId_eSlot16,     /* RDC Descriptor 16 */
	BRDC_SlotId_eSlot17,     /* RDC Descriptor 17 */
	BRDC_SlotId_eSlot18,     /* RDC Descriptor 18 */
	BRDC_SlotId_eSlot19,     /* RDC Descriptor 19 */
	BRDC_SlotId_eSlot20,     /* RDC Descriptor 20 */
	BRDC_SlotId_eSlot21,     /* RDC Descriptor 21 */
	BRDC_SlotId_eSlot22,     /* RDC Descriptor 22 */
	BRDC_SlotId_eSlot23,     /* RDC Descriptor 23 */
	BRDC_SlotId_eSlot24,     /* RDC Descriptor 24 */
	BRDC_SlotId_eSlot25,     /* RDC Descriptor 25 */
	BRDC_SlotId_eSlot26,     /* RDC Descriptor 26 */
	BRDC_SlotId_eSlot27,     /* RDC Descriptor 27 */
	BRDC_SlotId_eSlot28,     /* RDC Descriptor 28 */
	BRDC_SlotId_eSlot29,     /* RDC Descriptor 29 */
	BRDC_SlotId_eSlot30,     /* RDC Descriptor 30 */
	BRDC_SlotId_eSlot31,     /* RDC Descriptor 31 */
	BRDC_SlotId_eSlot32,     /* RDC Descriptor 32 */
	BRDC_SlotId_eSlot33,     /* RDC Descriptor 33 */
	BRDC_SlotId_eSlot34,     /* RDC Descriptor 34 */
	BRDC_SlotId_eSlot35,     /* RDC Descriptor 35 */
	BRDC_SlotId_eSlot36,     /* RDC Descriptor 36 */
	BRDC_SlotId_eSlot37,     /* RDC Descriptor 37 */
	BRDC_SlotId_eSlot38,     /* RDC Descriptor 38 */
	BRDC_SlotId_eSlot39,     /* RDC Descriptor 39 */
	BRDC_SlotId_eSlot40,     /* RDC Descriptor 40 */
	BRDC_SlotId_eSlot41,     /* RDC Descriptor 41 */
	BRDC_SlotId_eSlot42,     /* RDC Descriptor 42 */
	BRDC_SlotId_eSlot43,     /* RDC Descriptor 43 */
	BRDC_SlotId_eSlot44,     /* RDC Descriptor 44 */
	BRDC_SlotId_eSlot45,     /* RDC Descriptor 45 */
	BRDC_SlotId_eSlot46,     /* RDC Descriptor 46 */
	BRDC_SlotId_eSlot47,     /* RDC Descriptor 47 */
	BRDC_SlotId_eSlot48,     /* RDC Descriptor 48 */
	BRDC_SlotId_eSlot49,     /* RDC Descriptor 49 */
	BRDC_SlotId_eSlot50,     /* RDC Descriptor 50 */
	BRDC_SlotId_eSlot51,     /* RDC Descriptor 51 */
	BRDC_SlotId_eSlot52,     /* RDC Descriptor 52 */
	BRDC_SlotId_eSlot53,     /* RDC Descriptor 53 */
	BRDC_SlotId_eSlot54,     /* RDC Descriptor 54 */
	BRDC_SlotId_eSlot55,     /* RDC Descriptor 55 */
	BRDC_SlotId_eSlot56,     /* RDC Descriptor 56 */
	BRDC_SlotId_eSlot57,     /* RDC Descriptor 57 */
	BRDC_SlotId_eSlot58,     /* RDC Descriptor 58 */
	BRDC_SlotId_eSlot59,     /* RDC Descriptor 59 */
	BRDC_SlotId_eSlot60,     /* RDC Descriptor 60 */
	BRDC_SlotId_eSlot61,     /* RDC Descriptor 61 */
	BRDC_SlotId_eSlot62,     /* RDC Descriptor 62 */
	BRDC_SlotId_eSlot63,     /* RDC Descriptor 63 */

	/* Add new slot here */

	BRDC_SlotId_eSlotMAX     /* Maximum number of slot. Must be last ! */

} BRDC_SlotId;

/***************************************************************************
Summary:
	List of all triggers used by RDC. Must match trigger select in RDB.

Description:
	These enum are also use for indexing internal tables.  Note, not all
	triggers are available on a given chip.  In given case RDC API will return
	error with not supported.

See Also:
	BRDC_Slot_ExecuteOnTrigger_isr
****************************************************************************/
typedef enum
{
	/* Capture */
	BRDC_Trigger_eCap0Trig0 = 0,
	BRDC_Trigger_eCap0Trig1,

	BRDC_Trigger_eCap1Trig0,
	BRDC_Trigger_eCap1Trig1,

	BRDC_Trigger_eCap2Trig0,
	BRDC_Trigger_eCap2Trig1,

	BRDC_Trigger_eCap3Trig0,
	BRDC_Trigger_eCap3Trig1,

	BRDC_Trigger_eCap4Trig0,
	BRDC_Trigger_eCap4Trig1,

	BRDC_Trigger_eCap5Trig0,
	BRDC_Trigger_eCap5Trig1,

	BRDC_Trigger_eCap6Trig0,
	BRDC_Trigger_eCap6Trig1,

	BRDC_Trigger_eCap7Trig0,
	BRDC_Trigger_eCap7Trig1,

	/* Vec */
	BRDC_Trigger_eVec0Trig0,
	BRDC_Trigger_eVec0Trig1,
	BRDC_Trigger_eVec0Trig2,
	BRDC_Trigger_eVec0Trig3,

	BRDC_Trigger_eVec1Trig0,
	BRDC_Trigger_eVec1Trig1,
	BRDC_Trigger_eVec1Trig2,
	BRDC_Trigger_eVec1Trig3,

	BRDC_Trigger_eVec2Trig0,
	BRDC_Trigger_eVec2Trig1,
	BRDC_Trigger_eVec2Trig2,
	BRDC_Trigger_eVec2Trig3,

	/* aka, 656 output */
	BRDC_Trigger_eDtgTrig0,
	BRDC_Trigger_eDtgTrig1,

	BRDC_Trigger_eDtg1Trig0,
	BRDC_Trigger_eDtg1Trig1,

	/* Analog video decoder (vdec) */
	BRDC_Trigger_eVdec0Trig0,
	BRDC_Trigger_eVdec0Trig1,

	BRDC_Trigger_eVdec1Trig0,
	BRDC_Trigger_eVdec1Trig1,

	/* External ITU-R-656 input source */
	BRDC_Trigger_e6560Trig0,
	BRDC_Trigger_e6560Trig1,

	BRDC_Trigger_e6561Trig0,
	BRDC_Trigger_e6561Trig1,

	/* Memory to Memory compositor (or grc) */
	BRDC_Trigger_eM2mc0Trig,
	BRDC_Trigger_eM2mc1Trig,

	/* HD_DVI input */
	BRDC_Trigger_eHdDvi0Trig0,
	BRDC_Trigger_eHdDvi0Trig1,

	BRDC_Trigger_eHdDvi1Trig0,
	BRDC_Trigger_eHdDvi1Trig1,

	/* Simple Trigger Generator */
	BRDC_Trigger_eStg0Trig0,
	BRDC_Trigger_eStg0Trig1,

	/* Box detection */
	BRDC_Trigger_eLboxTrig0,
	BRDC_Trigger_eLboxTrig1,

	/* DVO output */
	BRDC_Trigger_eDvoTrig0,
	BRDC_Trigger_eDvoTrig1,
	BRDC_Trigger_eDvoTrig2,
	BRDC_Trigger_eDvoTrig3,

	/* Mpeg/AVC feeder */
	BRDC_Trigger_eMfd0Eof,
	BRDC_Trigger_eMfd1Eof,
	BRDC_Trigger_eMfd2Eof,
	BRDC_Trigger_eMfd3Eof,
	BRDC_Trigger_eMfd4Eof,
	BRDC_Trigger_eMfd5Eof,

	/* ANR Triggers */
	BRDC_Trigger_eAnr0Trig0,
	BRDC_Trigger_eAnr0Trig1,
	BRDC_Trigger_eAnr1Trig0,
	BRDC_Trigger_eAnr1Trig1,

	/* DGP Triggers */
	BRDC_Trigger_eDgp0Trig0,

	/* PX3D Triggers */
	BRDC_Trigger_ePx3d0Trig0,

	/* Vec Source or CMP trigger */
	BRDC_Trigger_eCmp_0Trig0,
	BRDC_Trigger_eCmp_0Trig1,

	BRDC_Trigger_eCmp_1Trig0,
	BRDC_Trigger_eCmp_1Trig1,

	BRDC_Trigger_eCmp_2Trig0,
	BRDC_Trigger_eCmp_2Trig1,

	BRDC_Trigger_eCmp_3Trig0,
	BRDC_Trigger_eCmp_3Trig1,

	BRDC_Trigger_eCmp_4Trig0,
	BRDC_Trigger_eCmp_4Trig1,

	BRDC_Trigger_eCmp_5Trig0,
	BRDC_Trigger_eCmp_5Trig1,

	BRDC_Trigger_eCmp_6Trig0,
	BRDC_Trigger_eCmp_6Trig1,

	/* Combined of triggers to trigger*/
	BRDC_Trigger_eComboTrig0,
	BRDC_Trigger_eComboTrig1,
	BRDC_Trigger_eComboTrig2,
	BRDC_Trigger_eComboTrig3,

	/* Unknown trigger ID */
	BRDC_Trigger_UNKNOWN

} BRDC_Trigger;


/***************************************************************************
Summary:
	List of all combined trigger modes used by RDC.

Description:
	BRDC_ComboTrigMode_eAllTriggers - If all of the triggers in the mask are active,
	                                  the trigger for this will fire;
	BRDC_ComboTrigMode_eAnyTrigger  - If any of the triggers in the mask are active,
	                                  the trigger for this will fire.

	Reset value is BRDC_ComboTrigMode_eAllTriggers;

See Also:
	BRDC_SetComboTrigMode_isr
****************************************************************************/
typedef enum
{
	BRDC_ComboTrigMode_eAllTriggers = 0,
	BRDC_ComboTrigMode_eAnyTrigger
} BRDC_ComboTrigMode;

/***************************************************************************
Summary:
	List of all variables used to store temporary values.

Description:
	Used by list operations to hold temporary values. These
	variables are available for all lists to use.

	Referred to as general purpose registers in the hardware documentation.

	Naming convention:
		BRDC_Variable_num:
			The general purpose variables, can be used by all the moduals.

	The following moduals have reserved variables:
		VDC: ? variables are reserved for VDC.

See Also:
	BRDC_OP_VAR_TO_REG,
	BRDC_OP_REG_TO_VAR,
	BRDC_OP_IMM_TO_VAR,
	BRDC_OP_VAR_AND_VAR_TO_VAR,
	BRDC_OP_VAR_AND_IMM_TO_VAR,
	BRDC_OP_VAR_OR_VAR_TO_VAR,
	BRDC_OP_VAR_OR_IMM_TO_VAR,
	BRDC_OP_VAR_XOR_VAR_TO_VAR,
	BRDC_OP_VAR_XOR_IMM_TO_VAR,
	BRDC_OP_NOT_VAR_TO_VAR,
	BRDC_OP_VAR_ROR_TO_VAR
****************************************************************************/
typedef enum
{
	/* General purpose RDC variables*/
	/* Note: Only provide small number variables for general use.
	 * If additional variables are needeed, only add the ones
	 * that are not used. Changing the reserved ones may break
	 * the system.
	 * TODO: expose them in a generic way to be shared by different
	 * PI modules; */
	BRDC_Variable_0 = 0,       /* RDC_data_0 */
	BRDC_Variable_1 = 1,       /* RDC_data_1 */
	BRDC_Variable_2 = 2,       /* RDC_data_2 */
	BRDC_Variable_3 = 3,       /* RDC_data_3 */
	                           /* RDC_data_4:  Reserved do not used */

	                           /* RDC_data_5:  Reserved do not used */
	                           /* RDC_data_6:  Reserved do not used */
	                           /* RDC_data_7:  Reserved do not used */
	                           /* RDC_data_8:  Reserved do not used */
	BRDC_Variable_Max
} BRDC_Variable;

/***************************************************************************
Summary:
	Opcode used for RDC

Description:

See Also:
****************************************************************************/
#define BRDC_SET_OPCODE(opcode)             ((opcode) << 24)
#define BRDC_GET_OPCODE(x)                  ((x) & UINT32_C(0xFF000000))

#define BRDC_OP_NOP_OPCODE                  BRDC_SET_OPCODE(0x0)
#define BRDC_OP_IMM_TO_REG_OPCODE           BRDC_SET_OPCODE(0x1)
#define BRDC_OP_VAR_TO_REG_OPCODE           BRDC_SET_OPCODE(0x2)
#define BRDC_OP_REG_TO_VAR_OPCODE           BRDC_SET_OPCODE(0x3)
#define BRDC_OP_IMM_TO_VAR_OPCODE           BRDC_SET_OPCODE(0x4)
#define BRDC_OP_IMMS_TO_REG_OPCODE          BRDC_SET_OPCODE(0x5)
#define BRDC_OP_IMMS_TO_REGS_OPCODE         BRDC_SET_OPCODE(0x6)
#define BRDC_OP_REG_TO_REG_OPCODE           BRDC_SET_OPCODE(0x7)
#define BRDC_OP_REGS_TO_REGS_OPCODE         BRDC_SET_OPCODE(0x8)
#define BRDC_OP_REG_TO_REGS_OPCODE          BRDC_SET_OPCODE(0x9)
#define BRDC_OP_REGS_TO_REG_OPCODE          BRDC_SET_OPCODE(0xa)
#define BRDC_OP_VAR_AND_VAR_TO_VAR_OPCODE   BRDC_SET_OPCODE(0xb)
#define BRDC_OP_VAR_AND_IMM_TO_VAR_OPCODE   BRDC_SET_OPCODE(0xc)
#define BRDC_OP_VAR_OR_VAR_TO_VAR_OPCODE    BRDC_SET_OPCODE(0xd)
#define BRDC_OP_VAR_OR_IMM_TO_VAR_OPCODE    BRDC_SET_OPCODE(0xe)
#define BRDC_OP_VAR_XOR_VAR_TO_VAR_OPCODE   BRDC_SET_OPCODE(0xf)
#define BRDC_OP_VAR_XOR_IMM_TO_VAR_OPCODE   BRDC_SET_OPCODE(0x10)
#define BRDC_OP_NOT_VAR_TO_VAR_OPCODE       BRDC_SET_OPCODE(0x11)
#define BRDC_OP_VAR_ROR_TO_VAR_OPCODE       BRDC_SET_OPCODE(0x12)
#define BRDC_OP_VAR_SUM_VAR_TO_VAR_OPCODE   BRDC_SET_OPCODE(0x13)
#define BRDC_OP_VAR_SUM_IMM_TO_VAR_OPCODE   BRDC_SET_OPCODE(0x14)
#define BRDC_OP_COND_SKIP_OPCODE            BRDC_SET_OPCODE(0x15)
#define BRDC_OP_SKIP_OPCODE                 BRDC_SET_OPCODE(0x16)
#define BRDC_OP_EXIT_OPCODE                 BRDC_SET_OPCODE(0x17)

/***************************************************************************
Summary:
	Register wrapper

Description:
	When specifying registers in a register list, this macro must be
	used. The purpose of this macro is to handle any issues where
	physical addresses are required instead of base-offsets.

Input:
	The register base-offset.

Output:
	Register value for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_REGISTER(reg)      ((reg) + BCHP_PHYSICAL_OFFSET)

/***************************************************************************
Summmary
	Converts RDC register back to offset register.

Description:
	This macro converts a BRDC_REGISTER register value back to the
	original register value. This function is useful for debugging purposes.

Input:
	A value returned by BRDC_REGISTER.

Output:
	The register base-offset.

See Also:
	BRDC_REGISTER
****************************************************************************/
#define BRDC_REGISTER_TO_OFFSET(rdc)      ((rdc) - BCHP_PHYSICAL_OFFSET)

/***************************************************************************
Summary:
	NOP command.

Description:
	This command can be used as a place holder so that other commands can
	be inserted into the middle of a list without requiring the size
	of the list to be adjusted.

	No additional data is required for this command.

	A NULL operation.
		bit 31 - 24 = 0, opcode
		bit 23 -  0 = 0, reserved

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_NOP()           BRDC_OP_NOP_OPCODE

/***************************************************************************
Summary:
	Immediate to register command.

Description:
	Also called register write immediate. The command created allows
	specified immediate data to be stored directly into a register.

	Following this command is the register to write to wrapped in the
	macro BRDC_REGISTER followed by the immediate data to be stored in
	that register.

	Write a BVN register using a 32-bit address data pair.
		bit 31 - 24 = 1, opcode
		bit 23 -  0 = 0, reserved

Output:
	Command for RDC list.

See Also:
	BRDC_REGISTER,
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_IMM_TO_REG()    BRDC_OP_IMM_TO_REG_OPCODE

/***************************************************************************
Summary:
	Variable to register command.

Description:
	Also called register write, this command stores the contents of a
	variable into a register.

	Following this command is the register to write to wrapped in the
	macro BRDC_REGISTER.

	Write the value of a RDC register into a BVN register.
		bit 31 - 24 = 2, opcode
		bit 23 - 18 = 0, reserved
		bit 17 - 12 = v, src
		bit 11 -  0 = 0, reserved

Input:
	v - The variable to read from.

Output:
	Command for RDC list.

See Also:
    BRDC_REGISTER,
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
/* this operation is followed by the register to write to (1 dword) */
#define BRDC_OP_VAR_TO_REG(v)   ( BRDC_OP_VAR_TO_REG_OPCODE | ((v)<<12) )

/***************************************************************************
Summary:
	Register to variable command.

Description:
	Also called register read, this command takes the contents of an
	register and stores it into a variable.

	Following this command is the register to read from wrapped in the
	macro BRDC_REGISTER

	Read a BVN register into a RDC register.
		bit 31 - 24 = 3, opcode
		bit 23 -  6 = 0, reserved
		bit  5 -  0 = v, dst

Input:
	v - The variable to write to.

Output:
	Command for RDC list.

See Also:
	BRDC_REGISTER,
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_REG_TO_VAR(v)   ( BRDC_OP_REG_TO_VAR_OPCODE | (v) )

/***************************************************************************
Summary:
	Data to variable command.

Description:
	Also called load immediate, this command takes the provided data and
	stores it in a variable.

	Following this command is the data to be stored in the variable.

	Load a 32-bit immediate value into a RDC register.
		bit 31 - 24 = 4, opcode
		bit 23 -  6 = 0, reserved
		bit  5 -  0 = v, dst

Input:
	v - The variable to write to.

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_IMM_TO_VAR(v)   ( BRDC_OP_IMM_TO_VAR_OPCODE | (v) )

/***************************************************************************
Summary:
	Block data to register command.

Description:
	Also called window write, this command takes a set of data and stores
	it into the same register.

	Following this command is the register to write to wrapped in the
	macro BRDC_REGISTER followed by the set of immediate data to be
	written.

	Write a block of data into a BVN window register, can be used to capture
	control information such as a RUL through a capture engines's register window.
		bit 31 - 24 = 5, opcode
		bit 23 - 12 = 0, reserved
		bit 11 -  0 = num -1, cnt

Input:
	num - The number of data values to write.

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_IMMS_TO_REG(num)   ( BRDC_OP_IMMS_TO_REG_OPCODE | ((num)-1) )

/***************************************************************************
Summary:
	Block data to block register command.

Description:
	Also called block write, this command takes a set of data and stores
	it into a consecutive list of registers.

	This command is considered to be the most optimal way to write
	a set of registers. However, since you are not reading the registers
	beforehand, this command cannot be used in a read/write/modify
	manner. Because of this, it is a good practice for software engineers
	to explicitly store values for all bits in the register even
	if those bit fields are reserved.

	Following this command is the first register address to
	write to wrapped in the macro BRDC_REGISTER followed by the set of
	immediate data to be written.

	Write a block of data into an array of BVN registers, can be used to
	update a series of control registers such as a scaler's coefficient
	table.
		bit 31 - 24 = 6, opcode
		bit 23 - 12 = 0, reserved
		bit 11 -  0 = num-1, cnt

Input:
	num - The number of consecutive registers to write.

Output:
	Command for RDC list.

See Also:
	BRDC_REGISTER,
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_IMMS_TO_REGS(num)   ( BRDC_OP_IMMS_TO_REGS_OPCODE | ((num)-1) )

/***************************************************************************
Summary:
	Single register to single register block copy command.

Description:
	Also called window copy, this command reads from a register
	and writes the value into another register a specified
	number of times.

	Following this command is the address of the source register
	wrapped in the BRDC_REGISTER macro followed by the
	address of the destination register wrapped in the BRDC_REGISTER
	macro.

	Copy a block of data from a BVN window register to another BVN
	window register.
		bit 31 - 24 = 7, opcode
		bit 23 - 12 = 0, reserved
		bit 11 -  0 = num-1, cnt

Input:
	num - The number of times to read and write.

Output:
	Command for RDC list.

See Also:
	BRDC_REGISTER,
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_REG_TO_REG(num)     ( BRDC_OP_REG_TO_REG_OPCODE | ((num)-1) )

/***************************************************************************
Summary:
	Registers to registers command.

Description:
	Also called block copy, this command reads from a consecutive set of
	registers and writes those values into another consecutive
	set of registers.

	Following this command is the first register source address
	wrapped in the BRDC_REGISTER macro followed by the first register
	destination address wrapped in the BRDC_REGISTER macro.

	Copy a block of data from an array of BVN registers to another array
	of BVN registers.
		bit 31 - 24 = 8, opcode
		bit 23 - 12 = 0, reserved
		bit 11 -  0 = num-1, cnt

Input:
	num - The number of registers to read from and write to.

Output:
	Command for RDC list.

See Also:
	BRDC_REGISTER,
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_REGS_TO_REGS(num)   ( BRDC_OP_REGS_TO_REGS_OPCODE | ((num)-1) )

/***************************************************************************
Summary:
	Register to registers command.

Description:
	Also called window to block copy, this command reads one external
	register and writes its value into a set of consecutive external
	registers.

	Following this command is the source address of the register
	wrapped in the BRDC_REGISTER macro followed by the first register
	destination address wrapped in the BRDC_REGISTER macro.

	Copy a block of data from a BVN window register to an array
	of BVN registers.
		bit 31 - 24 = 9, opcode
		bit 23 - 12 = 0, reserved
		bit 11 -  0 = num-1, cnt

Input:
	num - The number of registers to write to.

Output:
	Command for RDC list.

See Also:
    BRDC_REGISTER,
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_REG_TO_REGS(num)    ( BRDC_OP_REG_TO_REGS_OPCODE | ((num)-1) )

/***************************************************************************
Summary:
	Registers to register command.

Description:
	Also called block to window copy, this command reads from a consecutive
	set of registers and writes them into one register.

	Following this command is the first address of the source registers
	wrapped in the BREG_REGISTER macro followed by the destination
	register address wrapped in the BREG_REGISTER macro.

	Copy a block of data from an array of BVN registers.to a BVN window register.
		bit 31 - 24 = 0xa, opcode
		bit 23 - 12 = 0,   reserved
		bit 11 -  0 = num-1, cnt

Input:
	num - The number of registers to read from.

Output:
	Command for RDC list.

See Also:
	BRDC_REGISTER,
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_REGS_TO_REG(num)    ( BRDC_OP_REGS_TO_REG_OPCODE | ((num)-1) )

/***************************************************************************
Summary:
	Variable logical AND command.

Description:
	Also called bitwise logic AND, this command ANDs values two
	variables and stores the result into another variable.

	No additional data is required for this command.

	Perform a bitwise logic AND operation of two RDC registers.
		bit 31 - 24 = 0xb, opcode
		bit 23 - 18 = 0,   reserved
		bit 17 - 12 = v1,  src1
		bit 11 -  6 = v2,  src2
		bit  5 -  0 = v3,  dst

Input:
	v1 - Source variable
	v2 - Source variable
	v3 - Destination variable

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_VAR_AND_VAR_TO_VAR(v1, v2, v3)  \
	( BRDC_OP_VAR_AND_VAR_TO_VAR_OPCODE | ((v1)<<12) | ((v2)<<6) | (v3) )

/***************************************************************************
Summary:
	Variable logical AND with immediate command.

Description:
	Also called bitwise logic AND immediate, this command ANDs a value
	stored in a variable with a specified immediate value and stores the
	result into another variable.

	Following this command is the data to use in the AND operation.

	Perform a bitwise logic AND operation of a RDC register and an immediate value.
		bit 31 - 24 = 0xc, opcode
		bit 23 - 18 = 0,   reserved
		bit 17 - 12 = v1,  src
		bit 11 -  6 = 0,   reserved
		bit  5 -  0 = v2,  dst

Input:
	v1 - Source variable
	v2 - Destination variable

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_VAR_AND_IMM_TO_VAR(v1, v2)  \
	( BRDC_OP_VAR_AND_IMM_TO_VAR_OPCODE | ((v1)<<12) | (v2) )

/***************************************************************************
Summary:
	Variable logical OR command.

Description:
	Also called bitwise logic OR, this command ORs values from two
	variables and stores the result into another variable.

	No additional data is required for this command.

	Perform a bitwise logic OR operation of two RDC registers.
		bit 31 - 24 = 0xd, opcode
		bit 23 - 18 = 0,   reserved
		bit 17 - 12 = v1,  src1
		bit 11 -  6 = v2,  src2
		bit  5 -  0 = v3,  dst

Input:
	v1 - Source variable
	v2 - Source variable
	v3 - Destination variable

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_VAR_OR_VAR_TO_VAR(v1, v2, v3)  \
	( BRDC_OP_VAR_OR_VAR_TO_VAR_OPCODE | ((v1)<<12) | ((v2)<<6) | (v3) )

/***************************************************************************
Summary:
	Variable logical OR with immediate command.

Description:
	Also called bitwise logic OR immediate, this command ORs a value
	stored in a variable with a specified immediate value and stores the
	result into another variable.

	Following this command is the data to use in the OR operation.

	Perform a bitwise logic OR operation of a RDC register and an immediate value.
		bit 31 - 24 = 0xe, opcode
		bit 23 - 18 = 0,   reserved
		bit 17 - 12 = v1,  src
		bit 11 -  6 = 0,   reserved
		bit  5 -  0 = v2,  dst

Input:
	v1 - Source variable
	v2 - Destination variable

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_VAR_OR_IMM_TO_VAR(v1, v2)  \
	( BRDC_OP_VAR_OR_IMM_TO_VAR_OPCODE | ((v1)<<12) | (v2) )

/***************************************************************************
Summary:
	Variable logical XOR command.

Description:
	Also called bitwise logic XOR, this command XORs values from two
	variables and stores the result into another variable.

	No additional data is required for this command.

	Perform a bitwise logic XOR operation of two RDC registers.
		bit 31 - 24 = 0xf, opcode
		bit 23 - 18 = 0,   reserved
		bit 17 - 12 = v1,  src1
		bit 11 -  6 = v2,  src2
		bit  5 -  0 = v3,  dst

Input:
    v1 - Source variable
	v2 - Source variable
	v3 - Destination variable

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_VAR_XOR_VAR_TO_VAR(v1, v2, v3)  \
	( BRDC_OP_VAR_XOR_VAR_TO_VAR_OPCODE | ((v1)<<12) | ((v2)<<6) | (v3) )

/***************************************************************************
Summary:
	Variable logical XOR with immediate command.

Description:
	Also called bitwise logic XOR immediate, this command XORs a value
	stored in a variable with a specified immediate value and stores the
	result into another variable.

	Following this command is the data to use in the XOR operation.

	Perform a bitwise logic XOR operation of a RDC register and an immediate value.
		bit 31 - 24 = 0x10, opcode
		bit 23 - 18 = 0,   reserved
		bit 17 - 12 = v1,  src
		bit 11 -  6 = 0,   reserved
		bit  5 -  0 = v2,  dst

Input:
	v1 - Source variable
	v2 - Destination variable

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_VAR_XOR_IMM_TO_VAR(v1, v2)  \
	( BRDC_OP_VAR_XOR_IMM_TO_VAR_OPCODE | ((v1)<<12) | (v2) )

/***************************************************************************
Summary:
	Variable logical NOT command.

Description:
	Also called bitwise logic NOT, this command does a NOT operation on
	a variable and stores the result into another variable.

	No additional data is required for this command.

	Perform a bitwise inverse a RDC register.
		bit 31 - 24 = 0x11, opcode
		bit 23 - 18 = 0,   reserved
		bit 17 - 12 = v1,  src
		bit 11 -  6 = 0,   reserved
		bit  5 -  0 = v2,  dst

Input:
	v1 - Source variable
	v2 - Destination variable

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_NOT_VAR_TO_VAR(v1, v2)  \
	( BRDC_OP_NOT_VAR_TO_VAR_OPCODE | ((v1)<<12) | (v2) )

/***************************************************************************
Summary:
	Variable rotate right command.

Description:
	This command right rotates the bits within a variable and stores
	the result into another variable. Values that are shifted off
	the LSB will become the bits for the MSB.

	No additional data is required for this command.

	Rotate a RDC register to the right by the amount specified in the instruction.
	Lower significant bits are shifted to the upper significant bits.
		bit 31 - 24 = 0x12, opcode
		bit 23      = 0,   reserved
		bit 22 - 18 = rotate, rot
		bit 17 - 12 = v1,  src
		bit 11 -  6 = 0,   reserved
		bit  5 -  0 = v2,  dst

Input:
	v1 - Source variable
	v2 - Destination variable

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_VAR_ROR_TO_VAR(v1, rotate, v2)  \
	( BRDC_OP_VAR_ROR_TO_VAR_OPCODE | ((rotate)<<18) | ((v1)<<12) | (v2) )

/***************************************************************************
Summary:
	Variable SUM command.

Description:
	This command SUMs values from two variables and stores the result
	into another variable.

	No additional data is required for this command.

	Perform a SUM operation of two RDC registers.
		bit 31 - 24 = 0x13, opcode
		bit 23 - 18 = 0,    reserved
		bit 17 - 12 = v1,   src1
		bit 11 -  6 = v2,   src2
		bit  5 -  0 = v3,   dst

Input:
    v1 - Source variable
	v2 - Source variable
	v3 - Destination variable

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_VAR_SUM_VAR_TO_VAR(v1, v2, v3)  \
	( BRDC_OP_VAR_SUM_VAR_TO_VAR_OPCODE | ((v1)<<12) | ((v2)<<6) | (v3) )

/***************************************************************************
Summary:
	Variable SUM with immediate command.

Description:
	This command SUMs a value stored in a variable with a specified
	immediate value and stores the result into another variable.

	Following this command is the data to use in the SUM operation.

	Perform a SUM operation of a RDC register and an immediate value.
		bit 31 - 24 = 0x14, opcode
		bit 23 - 18 = 0,    reserved
		bit 17 - 12 = v1,   src
		bit 11 -  6 = 0,    reserved
		bit  5 -  0 = v2,   dst

Input:
	v1 - Source variable
	v2 - Destination variable

Output:
	Command for RDC list.

See Also:
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_VAR_SUM_IMM_TO_VAR(v1, v2)  \
	( BRDC_OP_VAR_SUM_IMM_TO_VAR_OPCODE | ((v1)<<12) | (v2) )

/***************************************************************************
Summary:
	Conditional forward skip command.

Description:
	Also called conditional forward branch, this command does a forward
	branch if the specified general purpose register is non-zero.

	Following this command is the amount of address to forward skip to.

	Perform a conditional skip
		bit 31 - 24 = 0x15, opcode
		bit 23 - 18 = 0,    reserved
		bit 17 - 12 = v,    src
		bit 11 -  0 = 0,    reserved

Input:
	v - The variable to read from.

Output:
	Command for RDC list.

See Also:
    BRDC_REGISTER,
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_COND_SKIP(v)   ( BRDC_OP_COND_SKIP_OPCODE | ((v)<<12) )

/***************************************************************************
Summary:
	Forward skip command.

Description:
	Also called forward branch, this command does a forward branch
	unconditionally.

	Following this command is the amount of address to forward skip to.

	Perform an unconditional branch
		bit 31 - 24 = 0x16, opcode
		bit 23 -  0 = 0,    reserved

Output:
	Command for RDC list.

See Also:
    BRDC_REGISTER,
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_SKIP()         ( BRDC_OP_SKIP_OPCODE )

/***************************************************************************
Summary:
	Exit command.

Description:
	This command ends RUL execution.

	No additional data is required for this command.

	Exit RUL execution
		bit 31 - 24 = 0x17, opcode
		bit 23 -  0 = 0,    reserved

Output:
	Command for RDC list.

See Also:
    BRDC_REGISTER,
	BRDC_List_Create,
	BRDC_List_GetStartAddress_isr
****************************************************************************/
#define BRDC_OP_EXIT()         ( BRDC_OP_EXIT_OPCODE )

/***************************************************************************
Summary:
	Maximum number of register blocks used with RDC block out

Description:


See Also:
	BRDC_BlockOut
****************************************************************************/
#define BRDC_MAX_RDC_BLOCKOUT_LIST_COUNT    8

/***************************************************************************
Summary:
	The handle for the register DMA module.

Description:
	This is the main module handle required in order to create command list
	or command slots.

See Also:
	BRDC_Open
****************************************************************************/
typedef struct BRDC_P_Handle *BRDC_Handle;

/***************************************************************************
Summary:
	A handle representing a command slot.

Description:

See Also:
	BRDC_Slot_Create
****************************************************************************/
typedef struct BRDC_P_Slot_Handle *BRDC_Slot_Handle;

/***************************************************************************
Summary:
	A handle representing a command list.

Description:

See Also:
	BRDC_List_Create
****************************************************************************/
typedef struct BRDC_P_List_Handle *BRDC_List_Handle;

/***************************************************************************
Summary:
	Data structure describing the information for a block of registers that
	will be blocked out during RDMA.

Description:

	ulStartRegAddr - the starting register. Use the offset.
	ulBlockSize    - the register block size
	bEnable        - if true, block out this register block, else, don't block

See Also:
	BRDC_SetRdcBlockOut
	BRDC_GetRdcBlockOut
****************************************************************************/
typedef struct
{
	uint32_t  ulStartRegAddr;
	uint32_t  ulBlockSize;
	bool      bEnable;
} BRDC_BlockOut;

/***************************************************************************
Summary:
	Data structure descibe the trigger information that are relevant for other
	module to use.

Description:
	This data structure describes the trigger infomrations.  BVDC and upper
	level software can use this sturcture to create callback with given
	trigger.

See Also:
	BRDC_GetTriggerInfo, BRDC_Trigger, BINT_Id
****************************************************************************/
typedef struct
{
	/* Trigger enum */
	BRDC_Trigger       eTrigger;

	/* When trigger fire it will also fire this interrupt, and this interrupt id
	 * is use in according with BINT. */
	BINT_Id            TrigIntId;

	/* Translate into hardware value.  This is chip dependence.  For internal
	 * RDC use only. */
	uint32_t           ulTrigVal;

	/* Trigger name string. */
	const char        *pchTrigName;

} BRDC_TrigInfo;

/***************************************************************************
Summary:
	Global Settings for the RDC

Description:
	This data structure is used to allow the app to set global settings for
	the RDC.

See Also:
	BRDC_Open
****************************************************************************/
typedef struct BRDC_Settings
{
	/* Intercept function for the BRDC_SetList_isr function */
	BERR_Code (*pfnSlot_SetList_Intercept_isr)
	(
		BRDC_Slot_Handle hSlot,
		BRDC_List_Handle hList,
		BRDC_Trigger     eTrigger
	) ;

	/* Intercept function for the BRDC_Slot_Execute_isr function */
	BERR_Code (*pfnSlot_Execute_Intercept_isr)
	(
		BRDC_Slot_Handle hSlot,
		BRDC_Trigger     eTrigger
	) ;

	/* Intercept function for the BRDC_Slot_ExecuteOnTrigger_isr
	   function */
	BERR_Code (*pfnSlot_ExecuteOnTrigger_Intercept_isr)
	(
		BRDC_Slot_Handle hSlot,
		BRDC_Trigger     eRDCTrigger,
		bool             bRecurring
	);
} BRDC_Settings;

/*****************************************************************************
Summary:
	Structure for RDC power standby settings

	pvStub - Stub holder for future expansion.

See Also:
	BRDC_Standby, BRDC_Resume
 *****************************************************************************/
typedef struct BRDC_StandbySettings
{
	void                              *pvStub; /* Stub holder */

} BRDC_StandbySettings;


/***************************************************************************
Summary:
	Returns the default settings structure for the module

Description:
	The default settings structure is returned by this function to be
	passed to BRDC_Open

Returns:
	BERR_SUCCESS - Successfully stored the default settings int the
	               parameter

See Also:
	BRDC_Open
****************************************************************************/
BERR_Code BRDC_GetDefaultSettings
	( BCHP_Handle                      hChp,    /* [in] Chip handle */
	  BRDC_Settings                   *pRdcSettings ); /* [out] Return default Settings for RDC */

/***************************************************************************
Summary:
	Opens the register DMA module.

Description:
	This open and initialized the RDMA modules.  In addition it also unmask
	the BCHP_SUN_GISB_ARB_REQ_MASK_rdc_MASK, which allow RDC to access
	registers outside of BVN block.

Returns:
	BERR_SUCCESS - Handle was successfully created..
	BERR_OUT_OF_SYSTEM_MEMORY - Unable to allocate memory for the handle.

See Also:
	BRDC_Close,
	BRDC_List_Create.
	BRDC_Slot_Create
****************************************************************************/
BERR_Code BRDC_Open
	( BRDC_Handle                     *phRdc,  /* [out] Register DMA handle to be returned. */
	  BCHP_Handle                      hChp,    /* [in] Chip handle. */
	  BREG_Handle                      hReg,    /* [in] Register access handle. */
	  BMEM_Handle                      hMem,    /* [in] Handle to memory heap to use for allocation. */
	  const BRDC_Settings             *pRdcSettings ); /* [in] Settings for RDC */

/***************************************************************************
Summary:
	Closes the register DMA module.

Description:

Returns:
	BERR_SUCCESS - Module was successfully closed.
	BERR_LEAKED_RESOURCE - Module was closed without freeing all
	allocated surface and palette handles.

See Also:
	BRDC_Open
****************************************************************************/
BERR_Code BRDC_Close
	( BRDC_Handle                      hRdc ); /* [in] Register DMA handle to close */

/*****************************************************************************
Summary:
	Get default standby settings.

See Also:
	BRDC_Standby
*****************************************************************************/
void BRDC_GetDefaultStandbySettings
	( BRDC_StandbySettings            *pStandbypSettings ); /* [out] Storage for default settings. */

/*****************************************************************************
Summary:
	Enter standby mode

Description:
	This function enters standby mode with the RDC module, if supported.
	The RDC modules.  If standby mode is not supported, calling this
	function has no effect.

	When in standby mode, the device clocks are turned off, resulting in a
	minimal power state.

	No BRDC_* calls should be made until standby mode is exitted by calling
	BRDC_Resume().  Calling BRDC_* api while in standy result in undefined
	results.

Returns:
	BERR_SUCCESS - If standby is successful, otherwise error

See Also:
	BRDC_Resume
*****************************************************************************/
BERR_Code BRDC_Standby
	( BRDC_Handle                      hRdc, /* [in] RDC module handle. */
	  const BRDC_StandbySettings      *pStandbypSettings ); /* [in] Settings for going into standy mode. */

/*****************************************************************************
Summary:
	Exit standby mode

Description:
	This function exits from standby mode with the RDC module, if supported.
	After exitting standby mode, upper-level SW is free to call
	BRDC_* functions.

Returns:
	BERR_SUCCESS - If standby is successful, otherwise error

See Also:
	BRDC_Standby
*****************************************************************************/
BERR_Code BRDC_Resume
	( BRDC_Handle                      hRdc ); /* [in] RDC module handle. */

/***************************************************************************
Summary:
	Get a scratch register

Description:
	There are a pool of scratch registers available in RDC module. The scratch
	registers can be used by RUL to temporarily store information in middle
	of RUL operation or double buffer some information across RUL events.
	This function gets hold of an RDC scratch register.

Inputs:
	hRdc     - RDC handle;

Outputs:

Returns:
	This function returns the address of an RDC scratch register.
	A zero address will be returned if no available RDC scratch register.

See Also:
	BRDC_FreeScratchReg.
****************************************************************************/
#define BRDC_AllocScratchReg(hRdc) \
    BRDC_P_AllocScratchReg(hRdc, __FILE__, __LINE__);

uint32_t BRDC_P_AllocScratchReg
	( BRDC_Handle                      hRdc,
	  const char*                      pchFilename, /* source filename where block is allocated from */
	  int                              iLine );     /* line number in file where allocation occurs */

/***************************************************************************
Summary:
	return a scratch register back to RDC's pool

Description:
	There are a pool of scratch registers available in RDC module. The scratch
	registers can be used by RUL to temporarily store information in middle
	of RUL operation or double buffer some information across RUL events.
	This function frees an RDC scratch register back to the pool.

Inputs:
	hRdc     - RDC handle;
	ulReg    - to-be-freed scratch register address;

Outputs:

Returns:
	BERR_SUCCESS if the scratch register is successully freed to the pool.
	BERR_INVALID_PARAMETER will be returned if ulReg is not a RDC scratch
	register.

See Also:
	BRDC_AllocScratchReg.
****************************************************************************/
BERR_Code BRDC_FreeScratchReg
	( BRDC_Handle                      hRdc,   /* [in] Register DMA handle to get scratch register */
	  uint32_t                         ulReg );/* [in] Address of the to-be-freed scratch register */

/***************************************************************************
Summary:
	Enables same trigger.

Description:
	Multiple slots using the same trigger are executed before any slots with a
	different trigger when enabled. Slots are executed without regard for
	which trigger was fired otherwise.

	Slot 0 has highest priority. Slot 31 has lowest priority.

	Trigger 0 has highest priority. Trigger 20 has lowest priority.

Returns:

See Also:
	BRDC_Trigger_Execute_isr
	BRDC_Slot_ExecuteOnTrigger_isr
	BRDC_Slot_Execute_isr
****************************************************************************/
BERR_Code BRDC_EnableSameTrigger_isr
	( BRDC_Handle                      hRdc,
	  bool                             bEnable );

/***************************************************************************
Summary:
	Set combine trigger mode.

Description:

Returns:

See Also:
	BRDC_Trigger
	BRDC_CombMode
****************************************************************************/
BERR_Code BRDC_SetComboTrigMode_isr
	( BRDC_Handle                      hRdc,
	  BRDC_Trigger                     eComboTrig,
	  BRDC_ComboTrigMode               eMode );

/***************************************************************************
 *
 */
BERR_Code BRDC_SetComboTriggers_isr
	( BRDC_Handle                      hRdc,
	  BRDC_Trigger                     eComboTrig,
	  uint32_t                         ulTriggersMask );

/***************************************************************************
Summary:
	Sets a register range to be blocked out during RDMA.

Description:
	If any of the registers specified by this range is part of an RUL during
	RDMA, these registers will be excluded from the RUL if enabled. If disabled,
	these registers will be included in the RUL.

Inputs:
	hRdc           - the RDC handle
	pstBlockOut    - the register block with the enable/disable info.
	ulRegBlock     - identifies this register block. A max of 8 blocks are allowed.
	                 The first block id is 0, the last is 7.
Returns:

See Also:
	BRDC_GetRdcBlockOut
	BRDC_BlockOut
****************************************************************************/
BERR_Code BRDC_SetRdcBlockOut
	( BRDC_Handle                      hRdc,
	  const BRDC_BlockOut             *pstBlockOut,
	  uint32_t                         ulRegBlock );

/***************************************************************************
Summary:
	Gets a register range that is blocked out during RDMA.

Description:
	If any of the registers specified by this range is part of an RUL during
	RDMA, these registers will be excluded from the RUL if enabled. If disabled,
	these registers will be included in the RUL.

Inputs:
	hRdc		   - the RDC handle
	ulRegBlock	   - identifies this register block. A max of 8 blocks are allowed.
					 The first block id is 0, the last is 7.

Outputs:
	pstBlockOut    - the register block with the enable/disable info.

Returns:

See Also:
	BRDC_SetRdcBlockOut
	BRDC_BlockOut
****************************************************************************/
BERR_Code BRDC_GetRdcBlockOut
	( BRDC_Handle                      hRdc,
	  BRDC_BlockOut                   *pstBlockOut,
	  uint32_t                         ulRegBlock );

/***************************************************************************
Summary:
	Activates multiple slots on a trigger

Description:
	Multiple slots using the trigger are executed when a specific trigger fires.

	If you want to execute a slot with a trigger call BRDC_Slot_ExecuteOnTrigger_isr.

	If you want to execute a slot immediately without a trigger call
	BRDC_Slot_Execute_isr instead.

Returns:

See Also:
	BRDC_Slot_ExecuteOnTrigger_isr
	BRDC_Slot_Execute_isr
****************************************************************************/
BERR_Code BRDC_Trigger_Execute_isr
	( BRDC_Handle                      hRdc,          /* [in] RDC module handle */
	  BRDC_Trigger                     eRDCTrigger ); /* [in] The trigger used to fire the slot */

/***************************************************************************
Summary:
	Get trigger information

Description:
	This function return the information of a given trigger enum.  Information
	included but not limitied to trigger's bint id and name.  On chipset
	that does not include eRDCTrigger a NULL pointer will be return.

Returns:
	pointer to (BRDC_TrigInfo*) eRDCTrigger's information.  NULL if error,
	invalid trigger, or trigger not supported.

See Also:
	BRDC_TrigInfo.
****************************************************************************/
const BRDC_TrigInfo* BRDC_Trigger_GetInfo
	( BRDC_Handle                      hRdc,           /* [in] RDC module handle */
	  BRDC_Trigger                     eRDCTrigger );  /* [in] The trigger used to fire the slot */

/***************************************************************************
Summary:
	Creates a list to contain RDC commands and data.

Description:
	This is the object used to store commands to be issued.

	Users are required to allocate a list using this function, use the
	function BRDC_List_GetStartAddress_isr to get a pointer to the data within the
	list, store BRDC_OP commands and data within the list, and then
	specify the number of command/data entries in the list with a call to
	BRDC_List_SetNumEntries_isr.

	Each command is a 32-bit value. Some commands have 1 or more additional
	32-bit values that must be stored after the command in a very specific
	order. For more information about BRDC_OP commands, see their
	individual desciption.

	Once a list is created, it needs to be placed into a slot and then
	issued. See BRDC_Slot_Create for more information.

Returns:

See Also:
	BRDC_Open,
	BRDC_List_GetStartAddress_isr,
	BRDC_Slot_Create,
	BRDC_List_Destroy,
	BRDC_List_GetMaxEntries_isr,
	BRDC_REGISTER,
	BRDC_OP_NOP,
	BRDC_OP_IMM_TO_REG,
	BRDC_OP_VAR_TO_REG,
	BRDC_OP_REG_TO_VAR,
	BRDC_OP_IMM_TO_VAR,
	BRDC_OP_IMMS_TO_REG,
	BRDC_OP_BLOCK_IMMS_TO_REGS,
	BRDC_OP_REG_TO_REG_BLOCK,
	BRDC_OP_REGS_TO_REGS,
	BRDC_OP_REG_TO_REGS,
	BRDC_OP_REGS_TO_REG,
	BRDC_OP_VAR_AND_VAR_TO_VAR,
	BRDC_OP_VAR_AND_IMM_TO_VAR,
	BRDC_OP_VAR_OR_VAR_TO_VAR,
	BRDC_OP_VAR_OR_IMM_TO_VAR,
	BRDC_OP_VAR_XOR_VAR_TO_VAR,
	BRDC_OP_VAR_XOR_IMM_TO_VAR,
	BRDC_OP_NOT_VAR_TO_VAR,
	BRDC_OP_VAR_ROR_TO_VAR
****************************************************************************/
BERR_Code BRDC_List_Create
	( BRDC_Handle                      hRdc,          /* [in] The handle to the RDC. */
	  uint32_t                         ulMaxEntries,  /* [in] The size of the list to allocate. */
	  BRDC_List_Handle                *phList );      /* [out] The allocated list. */

/***************************************************************************
Summary:
	Destroys a list.

Description:
	This command will fail if the list is currently assigned to a
	slot.

Returns:

See Also:
	BRDC_List_Create,
	BRDC_Slot_SetList_isr
****************************************************************************/
BERR_Code BRDC_List_Destroy
	( BRDC_List_Handle                 hList );  /* [in] The list to destroy */

/***************************************************************************
Summary:
	Returns the address of an allocated list.

Description:
	Once a list is created, the user must fill it with appropriate entries
	before it can be used. This function gives the user a pointer to the
	allocated list memory so that it can be modified.

Returns:
	The address of the list.

See Also:
	BRDC_List_Create
	BRDC_List_GetStartCachedAddress_isr
****************************************************************************/
uint32_t *BRDC_List_GetStartAddress_isr
	( BRDC_List_Handle                 hList );  /* [in] The list to use. */


#define BRDC_List_GetStartCachedAddress_isr(hList) \
	BRDC_List_GetStartAddress_isr(hList)

/***************************************************************************
Summary:
	Sets the list count.

Description:
	When a list is filled with commands an data. The user must set the
	number of entries that are in the list before assigning it to a slot.

	If the number of entries exceeds the maximum number of entries
	specified when the list was created, an error will result.

Returns:

See Also:
	BRDC_List_Create
	BRDC_Slot_SetList_isr
	BRDC_List_GetNumEntries_isr
	BRDC_List_GetMaxEntries_isr
	BRDC_List_GetNumEntries_isr
	BRDC_List_SetNumEntries_isr
****************************************************************************/
BERR_Code BRDC_List_SetNumEntries_isr
	( BRDC_List_Handle                 hList, /* [in] List to set. */
	  uint32_t                         ulNumEntries ); /* [in] The number of valid entries in a list. */

/***************************************************************************
Summary:
	Gets the number of entries in the list

Description:

Returns:

See Also:
	BRDC_List_Create
	BRDC_Slot_SetList_isr
	BRDC_List_GetMaxEntries_isr
	BRDC_List_SetNumEntries_isr
	BRDC_List_SetNumEntries_isr
	BRDC_List_GetNumEntries_isr
****************************************************************************/
BERR_Code BRDC_List_GetNumEntries_isr
	( BRDC_List_Handle                 hList, /* [in] List to get the number of entries from. */
	  uint32_t                        *pulNumEntries ); /* [out] The number of valid entries in the list */

/***************************************************************************
Summary:
	Gets the maximum number of entries in the list

Description:

Returns:

See Also:
	BRDC_List_Create
	BRDC_Slot_SetList_isr
	BRDC_List_SetNumEntries_isr
****************************************************************************/
BERR_Code BRDC_List_GetMaxEntries_isr
	( BRDC_List_Handle                 hList,     /* [in] List to get the number
	                                                of entries from. */
	  uint32_t                        *pulMaxEntries ); /* [out] The maximum number of entries in the list */

/***************************************************************************
Summary:
	Get the status of list execution

Description:
	Get the status of the list if it last executed with last slot assignment.
	Need to call BRDC_Slot_UpdateLastRulStatus_isr with the last Slot
	assignment.

Returns:
	true if the list was executed last time setting to a slot. false otherwise.

See Also:
	BRDC_Slot_SetList_isr
	BRDC_Slot_UpdateLastRulStatus_isr
****************************************************************************/
bool BRDC_List_GetLastExecStatus_isr
	( BRDC_List_Handle                 hList );  /* [in] List to set. */

/***************************************************************************
Summary:
	Creates a slot.

Description:
	Allocates a hardware slot used to store lists. When allocated this
	prevents any other module from using the same slot.

	Since this slot is allocated, all slots that are needed by a module
	should be created when the using module is first opened.

Returns:

See Also:
	BRDC_Open
****************************************************************************/
BERR_Code BRDC_Slot_Create
	( BRDC_Handle                      hRdc,     /* [in] The RDC handle. */
	  BRDC_Slot_Handle                *phSlot ); /* [out] The returned slot. */

/***************************************************************************
Summary:
	Destroys the slot.

Description:
	If the slot is still active in hardware, this function will first
	deactivate the slot before freeing the data structure. Because of
	this added check, one should first destroy the slot then destroy
	any lists contained within the slot.

Returns:

See Also:
	BRDC_Slot_Create
****************************************************************************/
BERR_Code BRDC_Slot_Destroy
	( BRDC_Slot_Handle                 hSlot ); /* [in] The list to destroy.*/

/***************************************************************************
Summary:
	Gets the ID of a specific slot.

Description:
	This function returns the ID of the slot which can be used to determine
	the exact set of registers used by this slot. One use of this information
	is to trigger a slot directly within another active list using register
	write commands.

See Also:
	BRDC_Slot_Create
****************************************************************************/
BERR_Code BRDC_Slot_GetId
	( BRDC_Slot_Handle                 hSlot,     /* [in] The slot to get the ID from */
	  BRDC_SlotId                     *pSlotId ); /* [out] Returned slot ID.*/

/***************************************************************************
Summary:
	Returns the list handle stored in a specific slot.

Description:
	The list assigned to a current slot is returned. This will be NULL
	if no list handle was assigned.

Returns:

See Also:
	BRDC_Slot_SetList_isr
	BRDC_Slot_SetCachedList_isr
****************************************************************************/
BERR_Code BRDC_Slot_GetList_isr
	( BRDC_Slot_Handle                 hSlot,   /* [in] The slot to hold the list */
	  BRDC_List_Handle                *phList );/* [out] List to store. */

/***************************************************************************
Summary:
	Stores a list into a specific slot. This function updates the RUL
	address and RUL count for the slot at isr.

Description:
	In order to activate a list, it must first be put into an allocated
	slot. This function does this task.

	A list may be assigned to more than one slot.

Returns:

See Also:
	BRDC_Slot_Create
****************************************************************************/
BERR_Code BRDC_Slot_SetList_isr
	( BRDC_Slot_Handle                 hSlot,   /* [in] The slot to hold the list */
	  BRDC_List_Handle                 hList ); /* [in] List to store. */


/***************************************************************************
Summary:
	Stores a list into a specific slot. This function updates the RUL
	address and RUL count for the slot at isr.

Description:
	In order to activate a list, it must first be put into an allocated
	slot. This function does this task.

	A list may be assigned to more than one slot.

Returns:

See Also:
	BRDC_Slot_Create
	BRDC_List_GetCachedAddress_isr
****************************************************************************/

#define BRDC_Slot_SetCachedList_isr(hSlot, hList) \
	BRDC_Slot_SetList_isr(hSlot, hList)

/***************************************************************************
Summary:
	Stores a list into dual slots This function updates the RUL
	address and RUL count for the slots at isr.

Description:
	In order to activate a list, it must first be put into an allocated
	slot. This function does this task.

	A list may be assigned to more than one slot.

Returns:

See Also:
	BRDC_Slot_Create
****************************************************************************/
BERR_Code BRDC_Slot_SetListDual_isr
	( BRDC_Slot_Handle                 hSlot1,   /* [in] The 1st slot to hold the list */
	  BRDC_Slot_Handle                 hSlot2,   /* [in] The 2nd slot to hold the list */
	  BRDC_List_Handle                 hList );  /* [in] List to store. */

#define BRDC_Slot_SetCachedListDual_isr(hSlot1, hSlot2, hList) \
	BRDC_Slot_SetListDual_isr(hSlot1, hSlot2, hList)


/***************************************************************************
Summary:
	Activates a slot on a trigger at isr.

Description:
	The slot is given to hardware and is allowed to fire on a specific
	trigger.

	If bRecurring is true, the slot will be activated each time
	the trigger fires. If bRecurring is false, the slot will fire only the
	first time the trigger fires.

	A level 2 interrupt will fire when the slot has completed. See level 2
	interrupt on how to service the interrupt appropriately.

	While the trigger to be used is specified in the RDC register set, the
	configuration of those triggers (when they fire) is configured in
	the software module that controls the source of the trigger. Those
	modules must be active and running before those triggers can fire.

	If you want to execute a slot immediately without a trigger call
	BRDC_Slot_Execute_isr instead.

Returns:

See Also:
	BRDC_Slot_Create,
****************************************************************************/
BERR_Code BRDC_Slot_ExecuteOnTrigger_isr
	( BRDC_Slot_Handle                 hSlot,        /* [in] The slot to activate. */
	  BRDC_Trigger                     eRDCTrigger,  /* [in] The trigger used to fire the slot. */
	  bool                             bRecurring ); /* [in] Whether to allow multiple firings
	                                                    of the trigger to execute the slot
	                                                    repeatedly. */

/***************************************************************************
Summary:
	Executes a slot manually at isr.

Description:
	Slot is executed once and immediately.

	If you want to execute a slot on a trigger, use
	BRDC_Slot_ExecuteOnTrigger_isr instead.

Returns:

See Also:
	BRDC_Slot_Create,
	BRDC_Slot_ExecuteOnTrigger_isr
****************************************************************************/
BERR_Code BRDC_Slot_Execute_isr
	( BRDC_Slot_Handle                 hSlot );  /* [in] The slot to execute. */

/***************************************************************************
Summary:
	Disables a slots execution at isr.

Description:
	Prevents a slot from further execution.

	This function is only necessary when a slot has been executed
	with BRDC_Slot_ExecuteOnTrigger_isr in recurring mode.

Returns:

See Also:
	BRDC_Slot_Create,
	BRDC_Slot_ExecuteOnTrigger_isr
****************************************************************************/
BERR_Code BRDC_Slot_Disable_isr
	( BRDC_Slot_Handle                 hSlot );  /* [in] The slot to disable.*/

/***************************************************************************
Summary:
	Get the interrupt id of this slot.

Description:
	This function return the interrupt id of this slot.  Use this id to
	create callback.  The interrupt will fire when the slot execution
	is done.

Returns:

See Also:
****************************************************************************/
BINT_Id BRDC_Slot_GetIntId
	( const BRDC_Slot_Handle           hSlot );  /* [in] The slot to get the int id.*/

/***************************************************************************
Summary:
	Update the status of last executed RUL.

Description:
	This function update the status of last assigned RUL has been executed
	or not.  In addition it will store that information in a given list.

Returns:
	true - if slot was last executed with the last list assignment.
	false - the last list assignment did not execute.

See Also:
	BRDC_List_GetLastExecStatus_isr
****************************************************************************/
bool BRDC_Slot_UpdateLastRulStatus_isr
	( BRDC_Slot_Handle                 hSlot,  /* [in] The slot to update the status. */
	  BRDC_List_Handle                 hList,   /* [in] The list to update the status. */
	  bool                             bEnableTracking ); /*[in] Enable tracking for this slot */

/***************************************************************************
 * This function program RUL to config the length of the RUL;
 */
BERR_Code BRDC_Slot_RulConfigRulSize_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                 hList,
	  uint32_t                         ulCount );

/******************* Chained RUL programming *******************************/
/***************************************************************************
 * This function program RUL to config the slave slot trigger;
 */
BERR_Code BRDC_Slot_RulConfigSlaveTrigger_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_Slot_Handle                 hSlotSlave,
	  BRDC_List_Handle                 hList,
	  BRDC_Trigger                     eRDCTrigger,
	  bool                             bRecurring );

/***************************************************************************
 * This function program previous RUL to config the length of the next RUL;
 */
BERR_Code BRDC_Slot_RulConfigCount_isr
	( BRDC_Slot_Handle                 hSlot,
	  uint32_t                         ulCount );

/***************************************************************************
 * This function build RUL to program slot descriptor to point to the
 * next slave RUL address;
 */
uint32_t BRDC_Slot_RulSetNextAddr_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                 hList );

/***************************************************************************
 * This function build the master RUL to program the slave slot to point to
 * the head of the slave RULs
 */
BERR_Code BRDC_List_RulSetSlaveListHead_isr
	( BRDC_List_Handle                 hList,
	  BRDC_Slot_Handle                 hSlotSlave,
	  BRDC_List_Handle                 hListSlave );

/***************************************************************************
 * This function build the tail of the chained slave-RULs:
 * it simply disable the slave slot itself;
 */
BERR_Code BRDC_Slot_RulConfigSlaveListTail_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                 hList );

/***************************************************************************
 * This function flushes the cached list;
 */
BERR_Code BRDC_Slot_FlushCachedList_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_List_Handle                 hList );

/***************************************************************************
 * This function returns the timer snapshot in microseconds for the slot 
 */
uint32_t BRDC_Slot_GetTimerSnapshot_isr
	( BRDC_Slot_Handle                 hSlot );

/***************************************************************************
 * This function returns the max value for RDC timer. This is the value the 
 * RDC timer will reach before wrapping back to zero, and is used to perform 
 * timer wrap calculationsThe value returned is in microseconds.
 */
uint32_t BRDC_GetTimerMaxValue
	( BRDC_Handle                      hRdc );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BRDC_H__ */

/* end of file */
