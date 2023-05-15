/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brdc_private.h $
 * $brcm_Revision: Hydra_Software_Devel/35 $
 * $brcm_Date: 11/1/11 9:56a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/rdc/7038/brdc_private.h $
 * 
 * Hydra_Software_Devel/35   11/1/11 9:56a pntruong
 * SW7435-23: Initial rdc support for 7435.
 *
 * Hydra_Software_Devel/34   2/9/11 3:34p pntruong
 * SW7420-1456: Initial standby power management that used chp's pm
 * functionalities.  Additional standardize coding style and uses of dbg
 * object.
 *
 * Hydra_Software_Devel/33   3/18/10 11:36a yuxiaz
 * SW7405-3954: Remove dependence of brdc.h in bavc.h
 *
 * Hydra_Software_Devel/31   3/24/09 5:29p albertl
 * PR52513: Moved RDC debug globals into hList structure.
 *
 * Hydra_Software_Devel/30   2/28/08 5:03p jessem
 * PR 38623: Added RDMA blockout support.
 *
 * Hydra_Software_Devel/29   10/4/07 11:52a pntruong
 * PR34714: Update RDC trigger definitions, and removed warnings and
 * global vars.
 *
 * Hydra_Software_Devel/28   9/13/07 4:20p pntruong
 * PR 28750 : Added RDC hooks for RUL capture for splash screen.
 *
 * Hydra_Software_Devel/28   9/13/07 4:14p pntruong
 * PR 28750 : Added RDC hooks for RUL capture for splash screen.
 *
 * Hydra_Software_Devel/27   12/21/06 1:07p hongtaoz
 * PR25668: h/w changed RDC scratch registers layout; bring up 7403;
 *
 * Hydra_Software_Devel/PR28750/1   9/7/07 1:46p shyam
 * PR 28750 : Change to RDC hooks as function pointers
 *
 * Hydra_Software_Devel/26   8/15/06 6:37p pntruong
 * PR23177:  Also need to map the unknown trigger for trigger_select.
 *
 * Hydra_Software_Devel/25   8/14/06 7:53p pntruong
 * PR23177: RDC module bringup.
 *
 * Hydra_Software_Devel/24   8/7/06 3:29p pntruong
 * PR23177: RDC module bringup.
 *
 * Hydra_Software_Devel/23   6/16/06 3:26p albertl
 * PR20276:  Removed multiple calls to BMEM address conversion functions.
 * Converted address now stored in context.
 *
 * Hydra_Software_Devel/22   1/12/06 2:44p hongtaoz
 * PR18233: need to re-compute the RUL cached start address every time in
 * case user changes cache usage or mapping;
 *
 * Hydra_Software_Devel/21   1/12/06 1:31p hongtaoz
 * PR18233: added mosaic mode support;
 *
 * Hydra_Software_Devel/20   1/3/06 3:01p yuxiaz
 * PR17593: Removed unused RDC task-context code. Use the _isr version of
 * functions for the non-_isr version.
 *
 * Hydra_Software_Devel/19   6/13/05 3:14p pntruong
 * PR15862: Added rdc rul logging for debug.
 *
 * Hydra_Software_Devel/18   11/23/04 8:53p pntruong
 * PR13076, PR11749: Video jitter under heavy system load.  Added RUL
 * execution check to reduce number of programmed registers.
 *
 * Hydra_Software_Devel/17   10/28/04 4:07p yuxiaz
 * PR13108: Remove BKNI_Sleep from critical section and _isr functions.
 * Clean up _isr and non_isr functions.
 *
 * Hydra_Software_Devel/16   3/16/04 2:54p yuxiaz
 * PR 10095: Added code to enable RDC acess of  regsiters outside BVN.
 * This code is disabled for A0.
 *
 * Hydra_Software_Devel/15   1/14/04 4:41p yuxiaz
 * PR 9076: Change isr functions to _isr.
 *
 * Hydra_Software_Devel/14   1/8/04 9:31a jasonh
 * PR 9201: Added typedef to structures.
 *
 * Hydra_Software_Devel/13   12/22/03 11:51a jasonh
 * PR 8861: Added new semaphore and debugging prototypes.
 *
 * Hydra_Software_Devel/12   10/30/03 2:46p yuxiaz
 * Remove bInterrupt from BRDC_Slot_Execute and
 * BRDC_Slot_ExecuteOnTrigger.
 *
 * Hydra_Software_Devel/11   10/27/03 3:13p yuxiaz
 * Added soft reset.
 *
 * Hydra_Software_Devel/10   9/8/03 9:36a yuxiaz
 * Change unsigned int to uint32_t.
 *
 * Hydra_Software_Devel/9   9/2/03 2:55p yuxiaz
 * Added BRDC_Slot_GetId, moved BRDC_SlotId to brdc.h.
 *
 * Hydra_Software_Devel/8   8/14/03 8:58a yuxiaz
 * Clean up include files.
 *
 * Hydra_Software_Devel/7   8/12/03 1:48p yuxiaz
 * Use double link list to keep track of slots a specific list assigned
 * to.
 *
 * Hydra_Software_Devel/6   7/25/03 12:12p yuxiaz
 * Change BRDC_Trigger to use defines in RDB. Misc clean up.
 *
 * Hydra_Software_Devel/5   7/17/03 8:50a yuxiaz
 * Added debug message.
 *
 * Hydra_Software_Devel/4   7/7/03 2:51p yuxiaz
 * Added opcode. Misc clean up.
 *
 * Hydra_Software_Devel/3   7/2/03 10:19a yuxiaz
 * Fixed register write.
 *
 * Hydra_Software_Devel/2   6/30/03 1:15p yuxiaz
 * Added BRDC_Trigger.
 *
 * Hydra_Software_Devel/1   6/27/03 2:55p yuxiaz
 * Initial version.
 *
 ***************************************************************************/
#ifndef BRDC__PRIVATE_H__
#define BRDC__PRIVATE_H__

#include "blst_list.h"           /* Link list support */
#include "brdc.h"
#include "brdc_dbg.h"

#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
 * Defines
 ***************************************************************************/

#define BRDC_P_Read32(hRdc, reg)                BREG_Read32(hRdc->hReg, reg)
#define BRDC_P_Write32(hRdc, reg, data)         BREG_Write32(hRdc->hReg, reg, data)

#define BRDC_Slot_P_Read32(hSlot, reg)          BRDC_P_Read32(hSlot->hRdc, reg)
#define BRDC_Slot_P_Write32(hSlot, reg, data)   BRDC_P_Write32(hSlot->hRdc, reg, data)

/* Number of rul to capture. */
#define BRDC_P_MAX_COUNT                        (1024*1024)

/* Null int id */
#define BRDC_P_NULL_BINTID                      ((BINT_Id)(-1))

/* Macro to make a RDC trigger.*/
#define BRDC_P_TRIGGER(trigger)                  BCHP_RDC_desc_0_config_trigger_select_##trigger

/* Unkown trigger value. */
#define BRDC_P_TRIGGER_UNKNOWN_VAL              (0x7f)

/* RDC general purpose registers address*/
#define BRDC_P_NO_TRACKING_ADDR                  UINT32_C(-1)

/***************************************************************************
Summary:
	Scratch Registers wrapper

Description:
	The following moduals have reserved scratch registers:
		RDC: the first 27 or 32 scratch registers are reserved internally for
		     slot execution tracking;
		VDC: the last 3 scratch registers are reserved for VDC format switch,
			 which are BRDC_SCRATCH_REG_END ~ (BRDC_SCRATCH_REG_END-2);
****************************************************************************/
#define BRDC_P_NUM_OF_SLOTS            (32)
#define BRDC_P_MAX_NUM_OF_DISPLAYS     (3)

#if BCHP_RDC_scratch_i_ARRAY_BASE /* 7403 and beyond */
	/* VBI: the last 4 scratch reigsters are reserved to coordinate programming
	 * of VBI encoder control registers between BVBI and BVDC. (see bavc.h
	 * for details) */
	#define BRDC_P_NUM_OF_SCRATCH_FOR_VBI  (4)

	/* the first 32 scratch registers are reserved for RDC slot execution tracking */
	#define BRDC_P_SCRATCH_REG_START   (0)
	#define BRDC_P_SCRATCH_REG_END     (BCHP_RDC_scratch_i_ARRAY_END - BRDC_P_NUM_OF_SCRATCH_FOR_VBI)

	#define BRDC_P_SCRATCH_REG_ADDR(varx) \
		(BCHP_RDC_scratch_i_ARRAY_BASE + ((varx) * sizeof(uint32_t)))

	/* track all slots */
	#define BRDC_P_TRACK_REG_ADDR(x) BRDC_P_SCRATCH_REG_ADDR(x)

	/* BRDC_P_SCRATCH_FIRST_AVAILABLE needs to exclude RDC internally reserved
	   slot-tracking registers;
	   we also need to reserve at least 3 scratch registers for VDC display format
	   switch usage; */
	#define BRDC_P_SCRATCH_FIRST_AVAILABLE  (BRDC_P_NUM_OF_SLOTS)
#else /* 3563 and previous chipsets */
	/* the first 27 or 32 scratch registers are reserved for RDC slot execution
	 * tracking */
	#define BRDC_P_SCRATCH_REG_START        (BRDC_Variable_Max)
	#define BRDC_P_SCRATCH_REG_END          (BCHP_RDC_data_i_ARRAY_END)

	#define BRDC_P_SCRATCH_REG_ADDR(varx) \
		(BCHP_RDC_data_i_ARRAY_BASE + ((varx) * sizeof(uint32_t)))

	/* BRDC_P_SCRATCH_FIRST_AVAILABLE needs to exclude RDC internally reserved
	   slot-tracking registers;
	   we also need to reserve at least 3 scratch registers for VDC display format
	   switch usage; */
	#if ((BRDC_P_SCRATCH_REG_END) >= (BRDC_P_SCRATCH_REG_START+BRDC_P_NUM_OF_SLOTS+BRDC_P_MAX_NUM_OF_DISPLAYS)) /* 3563 */
	#define BRDC_P_TRACK_REG_ADDR(x) BRDC_P_SCRATCH_REG_ADDR(x)
	#define BRDC_P_SCRATCH_FIRST_AVAILABLE  (BRDC_P_SCRATCH_REG_START+BRDC_P_NUM_OF_SLOTS)
	#else /* previous chipsets: 7038/7438/7118/7401/7400A0/3560 */
	#define BRDC_P_TRACK_REG_ADDR(x) \
		(((x) > BRDC_P_SCRATCH_REG_END-BRDC_P_MAX_NUM_OF_DISPLAYS)? \
		BRDC_P_NO_TRACKING_ADDR : BRDC_P_SCRATCH_REG_ADDR(x))
	#define BRDC_P_SCRATCH_FIRST_AVAILABLE  (BRDC_P_SCRATCH_REG_END-BRDC_P_MAX_NUM_OF_DISPLAYS+1)
	#endif
#endif

#if (BRDC_P_SCRATCH_FIRST_AVAILABLE+BRDC_P_MAX_NUM_OF_DISPLAYS-1)>BRDC_P_SCRATCH_REG_END
#error "RDC doesn't have enough scratch registers! Please adjust MACRO above!"
#endif

#ifdef BCHP_RDC_desc_0_config_segmented_MASK
#define BRDC_P_SUPPORT_SEGMENTED_RUL       (1)
#else
#define BRDC_P_SUPPORT_SEGMENTED_RUL       (0)
#endif

/***************************************************************************
 * Data Structure
 ***************************************************************************/

/***************************************************************************
 * BRDC_P_Slot_Head
 *      Head of the double Link List for slot
 ***************************************************************************/
typedef struct BRDC_P_Slot_Head BRDC_P_Slot_Head;
BLST_D_HEAD(BRDC_P_Slot_Head, BRDC_P_Slot_Handle);

/***************************************************************************
 * BRDC_P_Handle
 ***************************************************************************/
typedef struct BRDC_P_Handle
{
	BDBG_OBJECT(BRDC_RDC)

	BREG_Handle                        hReg;             /* Register module handle */
	BCHP_Handle                        hChp;             /* Chip module handle */
	BMEM_Handle                        hMem;             /* Memory module handle */
	BRDC_Settings                      stRdcSettings;    /* Global RDC Settings */
	const BRDC_TrigInfo               *aTrigInfo;        /* Contain this chip trigger information. */

	/* Scratch registers use flags */
	bool                               abScratchRegUsed[BRDC_P_SCRATCH_REG_END - BRDC_P_SCRATCH_FIRST_AVAILABLE + 1];
	bool                               bSlotUsed[BRDC_SlotId_eSlotMAX];
	BRDC_Slot_Handle                   apSlot[BRDC_SlotId_eSlotMAX];

#ifdef BRDC_USE_CAPTURE_BUFFER
	BRDC_DBG_CaptureBuffer             captureBuffer;
#endif

	/* RDC blockout */
	BRDC_List_Handle                   hRdcBlockOutList; /* prealloced RUL list for RDC block out */
	BRDC_BlockOut                      astBlockOut[BRDC_MAX_RDC_BLOCKOUT_LIST_COUNT];
	bool                               bRdcBlockOutEnabled;
} BRDC_P_Handle;


/***************************************************************************
 * BRDC_P_Slot_Handle
 ***************************************************************************/
typedef struct BRDC_P_Slot_Handle
{
	BDBG_OBJECT(BRDC_SLT)

	BRDC_Handle        hRdc;         /* Parent handle */
	BRDC_List_Handle   hList;        /* RUL list handle */

	bool               bRecurring;   /* Is it recurring? */
	BRDC_SlotId        eSlotId;      /* RDMA descriptor Id */
	BRDC_Trigger       eRDCTrigger;  /* Trigger */
	BINT_Id            SlotIntId;    /* L2 interrupt id of this slot. */
	uint32_t           ulRegOffset;  /* Byte offset from slot 0 */

	/* Keep track if a list has been executed when assigned to a slot. */
	bool               bTrackExecution;   /* Enable keep track of execution. */
	uint32_t           ulTrackCount;      /* SW updates this. */
	uint32_t           ulTrackRegAddr;    /* HW updates this. */

	/* store the desc_config setting */
	uint32_t          *pulRulConfigPrevVal; /* point to the previous RUL's config value */
	uint32_t          *pulRulConfigVal; /* point to the current RUL's config value */

	BLST_D_ENTRY(BRDC_P_Slot_Handle)  link;         /* doubly-linked list support */

} BRDC_P_Slot_Handle;


/***************************************************************************
 * BRDC_P_List_Handle
 ***************************************************************************/
typedef struct BRDC_P_List_Handle
{
	BDBG_OBJECT(BRDC_LST)

	BRDC_Handle         hRdc;              /* Parent handle */
	uint32_t           *pulRULAddr;        /* RUL address */
	uint32_t           *pulRULCacheAddr;   /* RUL cached address */
	uint32_t            ulAddrOffset;      /* Device offset address */
	uint32_t            ulEntries;         /* Number of entries in slot */
	uint32_t            ulMaxEntries;      /* Max number of entries */
	bool                bLastExecuted;     /* Check if last list assignment executed. */

	uint32_t            ulNumSlotAssigned; /* Number of slots the list assigned to */
	BRDC_P_Slot_Head   *pSlotAssigned;     /* Double link list to keep track of which slots the list assigned to */

	/* for RDC list debugging */
	BRDC_DBG_ListEntry  eNextEntry;
	uint32_t           *pulCurListAddr;
	uint32_t            ulNumEntries;
	uint32_t            ulCurrCommand;
	int                 iCommandIndex;
	int                 iDataCount;

} BRDC_P_List_Handle;


/***************************************************************************
 * Functions
 ***************************************************************************/
BERR_Code BRDC_P_SoftReset
	( BRDC_Handle                      hRdc );

BERR_Code BRDC_Slot_P_GetNextSlot
	( BRDC_Handle                      hRdc,
	  BRDC_SlotId                     *pSlotId );

BERR_Code BRDC_Slot_P_Write_Registers_isr
	( BRDC_Slot_Handle                 hSlot,
	  BRDC_Trigger                     eRDCTrigger,
	  bool                             bRecurring,
	  bool                             bExecuteOnTrigger );

BERR_Code BRDC_P_AcquireSemaphore_isr
	( BRDC_Handle                      hRdc,
	  BRDC_SlotId                      eSlotId );

void BRDC_P_ReleaseSemaphore_isr
	( BRDC_Handle                      hRdc,
	  BRDC_SlotId                      eSlotId );

void BRDC_P_DumpSlot
	( BRDC_Handle                      hRdc,
	  BRDC_SlotId                      eSlotId );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BRDC__PRIVATE_H__ */

/* end of file */
