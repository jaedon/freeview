/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brdc_private.c $
 * $brcm_Revision: Hydra_Software_Devel/33 $
 * $brcm_Date: 8/10/12 1:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/rdc/7038/brdc_private.c $
 * 
 * Hydra_Software_Devel/33   8/10/12 1:50p yuxiaz
 * SW7425-3626: Clean up RDC: Rename BRDC_List_GetStartCachedAddress_is to
 * BRDC_List_GetStartAddress_is, BRDC_Slot_SetCachedList_isr to
 * BRDC_Slot_SetList_isr, BRDC_Slot_SetCachedListDual_isr to
 * BRDC_Slot_SetListDual_isr.
 * 
 * Hydra_Software_Devel/32   8/9/12 9:57a yuxiaz
 * SW7425-3626: Replace uncache memory access with cache memory access.
 * 
 * Hydra_Software_Devel/31   11/1/11 9:55a pntruong
 * SW7435-23: Initial rdc support for 7435.
 *
 * Hydra_Software_Devel/30   12/15/10 12:33p erickson
 * SW7420-941: add missing bstd.h
 *
 * Hydra_Software_Devel/29   6/22/10 3:03p pntruong
 * SW7422-12: Fixed build errors.  Corrected naming to follow previous
 * naming convention.
 *
 * Hydra_Software_Devel/28   6/22/10 11:40a vanessah
 * SW7422-12:  To support appframework. Missing files added:
 * magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
 * do list: 1. in framework_board.c, more initialization to be done.  2.
 * More registers mapping, like clock generation as well as
 * BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
 *
 * Hydra_Software_Devel/27   1/22/09 1:05p pntruong
 * PR51344: Change RDC delay from 600 to 1000.
 *
 * Hydra_Software_Devel/26   1/6/07 12:22a pntruong
 * PR26943: Update documentation in RDMA - cannot access registers outside
 * of BVN.
 *
 * Hydra_Software_Devel/25   8/15/06 6:37p pntruong
 * PR23177:  Also need to map the unknown trigger for trigger_select.
 *
 * Hydra_Software_Devel/24   8/14/06 7:52p pntruong
 * PR23177: RDC module bringup.
 *
 * Hydra_Software_Devel/23   8/7/06 3:29p pntruong
 * PR23177: RDC module bringup.
 *
 * Hydra_Software_Devel/22   1/3/06 3:01p yuxiaz
 * PR17593: Removed unused RDC task-context code. Use the _isr version of
 * functions for the non-_isr version.
 *
 * Hydra_Software_Devel/21   11/23/04 8:53p pntruong
 * PR13076, PR11749: Video jitter under heavy system load.  Added RUL
 * execution check to reduce number of programmed registers.
 *
 * Hydra_Software_Devel/20   11/9/04 2:35p yuxiaz
 * PR13108: Use BKNI_Delay(1) to respond as quick as possible. Fixed
 * comment.
 *
 * Hydra_Software_Devel/19   10/28/04 4:07p yuxiaz
 * PR13108: Remove BKNI_Sleep from critical section and _isr functions.
 * Clean up _isr and non_isr functions.
 *
 * Hydra_Software_Devel/18   4/30/04 3:32p hongtaoz
 * PR8761: fixed C++ compile error.
 *
 * Hydra_Software_Devel/17   3/16/04 2:52p yuxiaz
 * PR 10095: Added code to enable RDC acess of  regsiters outside BVN.
 * This code is disabled for A0.
 *
 * Hydra_Software_Devel/16   1/14/04 4:41p yuxiaz
 * PR 9076: Change isr functions to _isr.
 *
 * Hydra_Software_Devel/15   12/31/03 11:09a yuxiaz
 * PR 9142: fixed compile warning with "-W" option.
 *
 * Hydra_Software_Devel/14   12/22/03 11:54a jasonh
 * PR 8861: Set reset to initialize registers to known state. Fixed forced
 * execute to set trigger/repeat appropriately. Added new private
 * routines to acquire/release semaphore and dump debugging slot
 * information.
 *
 * Hydra_Software_Devel/13   10/30/03 2:45p yuxiaz
 * Remove bInterrupt from BRDC_Slot_Execute and
 * BRDC_Slot_ExecuteOnTrigger.
 *
 * Hydra_Software_Devel/12   10/27/03 3:13p yuxiaz
 * Added soft reset.
 *
 * Hydra_Software_Devel/11   10/23/03 10:43a yuxiaz
 * Don't clear repeat for BRDC_Slot_Execute.
 *
 * Hydra_Software_Devel/10   10/17/03 8:56a yuxiaz
 * Fixed BDBG_ENTER and BDBG_LEAVE.
 *
 * Hydra_Software_Devel/9   9/24/03 10:28a yuxiaz
 * Convert virtual address to physical address offset.
 *
 * Hydra_Software_Devel/8   9/23/03 2:25p yuxiaz
 * Fixed count setting in RDC_desc_x_config.
 *
 * Hydra_Software_Devel/7   9/8/03 9:35a yuxiaz
 * Change unsigned int to uint32_t.
 *
 * Hydra_Software_Devel/6   9/2/03 2:55p yuxiaz
 * Added BRDC_Slot_GetId, moved BRDC_SlotId to brdc.h.
 *
 * Hydra_Software_Devel/5   7/25/03 12:12p yuxiaz
 * Change BRDC_Trigger to use defines in RDB. Misc clean up.
 *
 * Hydra_Software_Devel/4   7/17/03 8:50a yuxiaz
 * Added debug message.
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

#include "bstd.h"                /* standard types */
#include "brdc_private.h"
#include "bchp_fmisc.h"
#include "bkni.h"
#include "bchp_sun_gisb_arb.h"

BDBG_MODULE(BRDC);

/* Internal constant */
#define BRDC_P_SEMAPHORE_ACQUIRE_DELAY           (1000)

/***************************************************************************
Summary:
	Reset RDC

Description:

Input:
	hRdc - The RDC handle.

Output:

Returns:

****************************************************************************/
BERR_Code BRDC_P_SoftReset
(
	BRDC_Handle   hRdc
)
{
	BERR_Code err = BERR_SUCCESS;
	uint32_t  ulReg;
	int       i;
	uint32_t  ulRegAddr;
	uint32_t  ulRegConfig;
	uint32_t ulTrigSelect;

	BDBG_ENTER(BRDC_P_SoftReset);

#ifdef BCHP_FMISC_SW_INIT
	/* Write a 1 to the reset bit.*/
	ulReg  = BRDC_P_Read32(hRdc, BCHP_FMISC_SW_INIT);
	ulReg |= BCHP_FIELD_DATA(FMISC_SW_INIT, RDC, 1);
	BRDC_P_Write32(hRdc, BCHP_FMISC_SW_INIT, ulReg);

	/* Write a 0 to reset. */
	ulReg &= ~BCHP_FIELD_DATA(FMISC_SW_INIT, RDC, 1);
	BRDC_P_Write32(hRdc, BCHP_FMISC_SW_INIT, ulReg);
#else
	/* Write a 1 to the reset bit.*/
	ulReg  = BRDC_P_Read32(hRdc, BCHP_FMISC_SOFT_RESET);
	ulReg |= BCHP_FIELD_DATA(FMISC_SOFT_RESET, RDC, 1);
	BRDC_P_Write32(hRdc, BCHP_FMISC_SOFT_RESET, ulReg);

	/* Write a 0 to reset. */
	ulReg &= ~BCHP_FIELD_DATA(FMISC_SOFT_RESET, RDC, 1);
	BRDC_P_Write32(hRdc, BCHP_FMISC_SOFT_RESET, ulReg);
#endif

	/******************
	 * Set known good values for all registers
	 */
#if (!BRDC_P_SUPPORT_SEGMENTED_RUL)
	ulReg = BCHP_FIELD_DATA(RDC_config, same_trigger, 0);
#else
	ulReg = BCHP_FIELD_DATA(RDC_config, trig_arbitration_mode, 0); /* 0 - convention ; 1 - segmented mode */
#endif
	BRDC_P_Write32(hRdc, BCHP_RDC_config, ulReg);

	/* Get trigger select value. */
	ulTrigSelect = hRdc->aTrigInfo[BRDC_Trigger_UNKNOWN].ulTrigVal;

	/* setup known values for descriptors */
	ulRegAddr = BCHP_FIELD_DATA(RDC_desc_0_addr, addr, 0x0);
	ulRegConfig =
		BCHP_FIELD_DATA(RDC_desc_0_config, count,           0x0          ) |
		BCHP_FIELD_DATA(RDC_desc_0_config, trigger_select,  ulTrigSelect ) |
		BCHP_FIELD_DATA(RDC_desc_0_config, repeat,          0            ) |
		BCHP_FIELD_DATA(RDC_desc_0_config, enable,          0            ) |
		BCHP_FIELD_DATA(RDC_desc_0_config, done,            1           ) |
		BCHP_FIELD_DATA(RDC_desc_0_config, error,           1            ) |
		BCHP_FIELD_DATA(RDC_desc_0_config, dropped_trigger, 1            );

	/* set all descriptors */
	for (i=0; i<32; ++i)
	{
		/* acquire semaphore */
		BKNI_EnterCriticalSection();
		err = BERR_TRACE(BRDC_P_AcquireSemaphore_isr(hRdc, (BRDC_SlotId)i));
		if (err != BERR_SUCCESS)
		{
			/* error */
			BKNI_LeaveCriticalSection();
			goto done;
		}

		/* write address and config */
		BRDC_P_Write32(hRdc, BCHP_RDC_desc_0_addr + 16 * i,   ulRegAddr);
		BRDC_P_Write32(hRdc, BCHP_RDC_desc_0_config + 16 * i, ulRegConfig);

		/* release semaphore */
		BRDC_P_ReleaseSemaphore_isr(hRdc, (BRDC_SlotId)i);
		BKNI_LeaveCriticalSection();
	}

	/* PR 10095:
	 * RDC can only be used to program BVN registers by default (set at bootup).
	 * Need to enable it for B0 if need to access IFD registers. */
	/* Unmask RDC so it can access registers outside BVN, such as IFD regs. */
	BKNI_EnterCriticalSection();
	ulReg  = BREG_Read32_isr(hRdc->hReg, BCHP_SUN_GISB_ARB_REQ_MASK);
#ifdef BCHP_SUN_GISB_ARB_REQ_MASK_rdc_MASK
	ulReg &= ~BCHP_SUN_GISB_ARB_REQ_MASK_rdc_MASK;
#endif
#ifdef BCHP_SUN_GISB_ARB_REQ_MASK_req_mask_5_MASK
	ulReg &= ~BCHP_SUN_GISB_ARB_REQ_MASK_req_mask_5_MASK;
#endif
	BREG_Write32_isr(hRdc->hReg, BCHP_SUN_GISB_ARB_REQ_MASK, ulReg);
	BKNI_LeaveCriticalSection();

done:
	BDBG_LEAVE(BRDC_P_SoftReset);
	return err;
}


/***************************************************************************
Summary:
	Private function to get next available slot

Description:

Input:
	hRdc - The RDC handle.

Output:
	pSlotID - The returned slot ID.

Returns:

****************************************************************************/
BERR_Code BRDC_Slot_P_GetNextSlot
(
	BRDC_Handle   hRdc,
	BRDC_SlotId  *pSlotId
)
{
	BERR_Code    err = BERR_SUCCESS;
	int          eSlotId;

	BDBG_ENTER(BRDC_Slot_P_GetNextSlot);

	for( eSlotId = BRDC_SlotId_eSlot0; eSlotId < BRDC_SlotId_eSlotMAX; eSlotId++ )
	{
		if( !hRdc->bSlotUsed[eSlotId] )
		{
			*pSlotId = (BRDC_SlotId) eSlotId;
			goto done;
		}
	}

	/* Can't find any slot available */
	err = BERR_TRACE(BRDC_SLOT_ERR_ALL_USED);

done:
	BDBG_LEAVE(BRDC_Slot_P_GetNextSlot);
	return err;
}

/***************************************************************************
Summary:
	Private function to fill in hardware registers for DMA

	This function assumes DMA is already locked if necessary.

Description:

Input:
	hSlot - The slot to activate.
	ui32_trigger - The trigger used to fire the slot.
	bRecurring - Whether to allow multiple firings of the trigger to execute
	             the slot repeatedly.

Output:
	pSlotID - The returned slot ID.

Returns:

****************************************************************************/
BERR_Code BRDC_Slot_P_Write_Registers_isr
(
	BRDC_Slot_Handle hSlot,
	BRDC_Trigger     eRDCTrigger,
	bool             bRecurring,
	bool             ExecuteOnTrigger
)
{
	uint32_t ulRegVal, ulTrigSelect;

	BDBG_ENTER(BRDC_Slot_P_Write_Registers_isr);

	/* Set RDC_desc_x_addr */
	BRDC_Slot_P_Write32(hSlot, BCHP_RDC_desc_0_addr + hSlot->ulRegOffset, 
		hSlot->hList->ulAddrOffset);

	/* Set RDC_desc_x_config */
	ulRegVal = BRDC_Slot_P_Read32(hSlot, BCHP_RDC_desc_0_config + hSlot->ulRegOffset);

	/* Get trigger select value. */
	ulTrigSelect = hSlot->hRdc->aTrigInfo[eRDCTrigger].ulTrigVal;

	if( ExecuteOnTrigger )
	{
		ulRegVal &= ~(
			BCHP_MASK(RDC_desc_0_config, enable         ) |
			BCHP_MASK(RDC_desc_0_config, repeat         ) |
			BCHP_MASK(RDC_desc_0_config, trigger_select ) |
			BCHP_MASK(RDC_desc_0_config, count          ));

		ulRegVal |= (
			BCHP_FIELD_DATA(RDC_desc_0_config, enable,         1                          ) |
			BCHP_FIELD_DATA(RDC_desc_0_config, repeat,         bRecurring                 ) |
			BCHP_FIELD_DATA(RDC_desc_0_config, trigger_select, ulTrigSelect               ) |
			BCHP_FIELD_DATA(RDC_desc_0_config, count,          hSlot->hList->ulEntries -1));

		BRDC_Slot_P_Write32(hSlot, BCHP_RDC_desc_0_config + hSlot->ulRegOffset, ulRegVal);
	}
	else
	{
		/* previously not enabled? */
		if (!BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_config, enable))
		{
			/* we are forcing a descriptor that doesn't have a trigger
			   so we should set the trigger to an undefined value (so
			   we can later turn on the enable) and turn off the repeat so this
			   slot is executed only once */
			/* Get trigger select value. */
			ulTrigSelect = hSlot->hRdc->aTrigInfo[BRDC_Trigger_UNKNOWN].ulTrigVal;

			ulRegVal &= ~(
				BCHP_MASK(RDC_desc_0_config, trigger_select ) |
				BCHP_MASK(RDC_desc_0_config, repeat         ));
			ulRegVal |= (
				BCHP_FIELD_DATA(RDC_desc_0_config, trigger_select, ulTrigSelect) |
				BCHP_FIELD_DATA(RDC_desc_0_config, repeat,         0));
		}

		/* enable descriptor and update count */
		ulRegVal &= ~(
			BCHP_MASK(RDC_desc_0_config, enable ) |
			BCHP_MASK(RDC_desc_0_config, count  ));

		ulRegVal |= (
			BCHP_FIELD_DATA(RDC_desc_0_config, enable, 1                          ) |
			BCHP_FIELD_DATA(RDC_desc_0_config, count,  hSlot->hList->ulEntries - 1));

		BRDC_Slot_P_Write32(hSlot, BCHP_RDC_desc_0_config + hSlot->ulRegOffset, ulRegVal);

		/* Set RDC_desc_x_immediate */
		ulRegVal = BCHP_FIELD_DATA(RDC_desc_0_immediate, trigger, 1 );
		BRDC_Slot_P_Write32(hSlot, BCHP_RDC_desc_0_immediate + hSlot->ulRegOffset, ulRegVal);
	}

	BDBG_LEAVE(BRDC_Slot_P_Write_Registers_isr);
	return BERR_SUCCESS;

}

/***************************************************************************
Summary:
	Isr function to acquire semaphore from slot.

Description:

Input:
	hSlot - The slot to acquire semaphore from.
	ulRegOffset - Offset to the slot's registers.

Output:

Returns:

****************************************************************************/
BERR_Code BRDC_P_AcquireSemaphore_isr
(
	BRDC_Handle hRdc,
	BRDC_SlotId eSlotId
)
{
	int      iDMABusy = 0;
	bool     bDMABusy;
	uint32_t ulRegVal, ulRegOffset;

	/* calculate offset for this slot */
	ulRegOffset = 16 * (eSlotId - BRDC_SlotId_eSlot0);

	/* If DMA is not busy, this read will acquire the semaphore */
	ulRegVal = BRDC_P_Read32(hRdc, BCHP_RDC_desc_0_lock + ulRegOffset);

	/* All RDC_desc_x_lock bit definitions are same */
	bDMABusy = (bool)BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_lock, semaphore);

	/* Wait to get semaphore to lock DMA */
	while( bDMABusy )
	{
		/* PR13108: This is the very rare case that we can't acquire
		 * semaphore for the slot.
		 * The common belief is that a RUL execution time should be maxed by
		 * 1/2000th of a second. Therefore the max delay caused by a loss of
		 * semaphore should be the same plus some delta just in case.
		 * In this case, choose total dealy = 1/2000 sec + 100 us (delta) =
		 * 600 us, for BRDC_P_SEMAPHORE_ACQUIRE_DELAY tries. */
		BKNI_Delay(1);

		if (BRDC_P_SEMAPHORE_ACQUIRE_DELAY == ++iDMABusy)
		{
			/* could not acquire semaphore within a reasonable amount of time */
			BDBG_ERR(( "Cannot acquire semaphore" ));
			BRDC_P_DumpSlot(hRdc, eSlotId);
			return BERR_TRACE(BERR_TIMEOUT);
		}

		/* If DMA is not busy, this read will acquire the semaphore */
		ulRegVal = BRDC_P_Read32(hRdc, BCHP_RDC_desc_0_lock + ulRegOffset);

		/* All RDC_desc_x_lock bit definitions are same */
		bDMABusy = (bool)BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_lock, semaphore);

	}

	/* semaphore acquired */
	return BERR_SUCCESS;
}

/***************************************************************************
Summary:
	Release semaphore to hardware

Description:

Input:
	hSlot - The slot to release semaphore.
	ulRegOffset - Offset to the slot's registers.

Output:

Returns:

****************************************************************************/
void BRDC_P_ReleaseSemaphore_isr
(
	BRDC_Handle hRdc,
	BRDC_SlotId eSlotId
)
{
	uint32_t ulRegVal;
	uint32_t ulRegOffset;

	/* calculate offset for this slot */
	ulRegOffset = 16 * (eSlotId - BRDC_SlotId_eSlot0);

	/* Release semaphore. Write 1 to clear. */
	ulRegVal = BCHP_MASK(RDC_desc_0_lock, semaphore);
	BRDC_P_Write32(hRdc, BCHP_RDC_desc_0_lock + ulRegOffset, ulRegVal);
}

void BRDC_P_DumpSlot
(
	BRDC_Handle hRdc,
	BRDC_SlotId eSlotId
)
{
	uint32_t  ulRegOffset;
	uint32_t  ulRegVal;
	uint32_t  ulAddr, *pulAddr;
	void     *pvAddr, *pvCachedAddr;
	uint32_t  ulCount, ulIndex;

	/* determine offset of registers for this slot */
	ulRegOffset = 16 * (eSlotId - BRDC_SlotId_eSlot0);

	/* header */
	BDBG_MSG(("-------------------------------\n"));

	/* read and display address register */
	ulRegVal = BRDC_P_Read32(hRdc, BCHP_RDC_desc_0_addr + ulRegOffset);
	ulAddr = BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_addr, addr);
	BDBG_MSG(("RDC_desc_%d_addr\n"
		"\taddr: 0x%08x\n",
		eSlotId,
		ulAddr ));

	/* read and display config register */
	ulRegVal = BRDC_P_Read32(hRdc, BCHP_RDC_desc_0_config + ulRegOffset);
	ulCount = BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_config, count);
	BDBG_MSG(("RDC_desc_%d_config\n"
		"\tcount:           %d\n"
		"\ttrigger_select:  %d\n"
		"\trepeat:          %d\n"
		"\tenable:          %d\n"
		"\tdone:            %d\n"
		"\tbusy:            %d\n"
		"\terror:           %d\n"
		"\tdropped_trigger: %d\n"
		"\tlock_rd:         %d\n",
		eSlotId,
		ulCount,
		BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_config, trigger_select),
		BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_config, repeat),
		BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_config, enable),
		BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_config, done),
		BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_config, busy),
		BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_config, error),
		BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_config, dropped_trigger),
		BCHP_GET_FIELD_DATA(ulRegVal, RDC_desc_0_config, lock_rd)));

	/* contents of RUL */
	BDBG_MSG(("RUL contents\n"));
	BMEM_ConvertOffsetToAddress(hRdc->hMem, ulAddr, &pvAddr);

	/* Use cached address */
	BMEM_ConvertAddressToCached(hRdc->hMem, pvAddr, &pvCachedAddr);
	BMEM_FlushCache_isr(hRdc->hMem, pvCachedAddr, ulCount);

	pulAddr = (uint32_t *)pvCachedAddr;
	for (ulIndex=0; ulIndex<=ulCount; ++ulIndex)
	{
		BDBG_MSG(("0x%08x\n", *(pulAddr++)));
	}
}

/* end of file */
