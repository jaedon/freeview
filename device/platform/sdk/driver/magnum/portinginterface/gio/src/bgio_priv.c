/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgio_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/50 $
 * $brcm_Date: 4/24/12 3:02p $
 *
 * Module Description:
 *
 *
 * Revision History:
 * $brcm_Log: /magnum/portinginterface/gio/7435/bgio_priv.c $
 * 
 * Hydra_Software_Devel/50   4/24/12 3:02p tdo
 * SW7584-10: Add PI/gio support for 7584
 *
 * Hydra_Software_Devel/49   6/16/11 5:59p tdo
 * SWDTV-7592: add BDBG_OBJECT_ASSERT for BGIO.
 *
 * Hydra_Software_Devel/48   6/16/11 11:26a tdo
 * SWDTV-7292: Re-org GIO code
 *
 * Hydra_Software_Devel/47   6/7/11 1:22p tdo
 * SWDTV-7292: Add Magnum PI GIO to 35233
 *
 * Hydra_Software_Devel/SWDTV-7288/1   5/26/11 5:16p franli
 * SWDTV-7288:Correct the GPIO control set for 35233 chip
 *
 * Hydra_Software_Devel/46   5/11/11 11:20a tdo
 * SW7231-139: [BGIO] It is not possible to control GPIO[149:122]
 *
 * Hydra_Software_Devel/45   4/8/11 9:40a tdo
 * SWBLURAY-23686: Add GIO PortingInterface support for Blast (7640) chip
 *
 * Hydra_Software_Devel/44   3/21/11 3:47p jhaberf
 * SW35330-13: Added support for 35233 DTV chip
 *
 * Hydra_Software_Devel/43   12/6/10 4:09p jhaberf
 * SW35230-1: Added 35125 to the build
 *
 * Hydra_Software_Devel/42   12/2/10 2:38p tdo
 * SWBLURAY-23686: Add GIO PortingInterface support for Blast (7640) chip
 *
 * Hydra_Software_Devel/41   11/17/10 9:03a tdo
 * SW7231-11: Add GIO PI support for 7231/7344/7346
 *
 * Hydra_Software_Devel/40   11/1/10 5:01p xhuang
 * SW7552-4: Add 7552 support
 *
 * Hydra_Software_Devel/39   8/30/10 3:18p tdo
 * SW7425-22: Add GIO PI support for 7425
 *
 * Hydra_Software_Devel/38   8/12/10 1:00p tdo
 * SW7358-6: Add support for 7358
 *
 * Hydra_Software_Devel/SW7358-6/1   8/12/10 2:48p xhuang
 * SW7358-6: Add support for 7358 in gio
 *
 * Hydra_Software_Devel/37   6/22/10 11:40a vanessah
 * SW7422-12:  To support appframework. Missing files added:
 * magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
 * do list: 1. in framework_board.c, more initialization to be done.  2.
 * More registers mapping, like clock generation as well as
 * BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
 *
 * Hydra_Software_Devel/36   1/5/10 1:02p jhaberf
 * SW35230-1: Check in of file on behalf of Srinivasa M.P. Reddy in order
 * to get 35230 RAP PI compiling
 *
 * Hydra_Software_Devel/35   11/19/09 11:14a tdo
 * SW7468-23: Create GIO PI for 7468
 *
 * Hydra_Software_Devel/34   11/18/09 11:36p tdo
 * SW7408-22: Add GIO PI support for 7408
 *
 * Hydra_Software_Devel/33   9/1/09 2:37p yuxiaz
 * SW7550-28: Add GIO pinmux support for 7125.
 *
 * Hydra_Software_Devel/32   9/1/09 11:11a tdo
 * SW7550-28: Add GIO pinmux support for 7550
 *
 * Hydra_Software_Devel/31   8/27/09 6:30p tdo
 * SW7630-15: Bringup of portinginterface "gio" for Grain (7630) and 7342
 *
 * Hydra_Software_Devel/30   6/11/09 9:30p tdo
 * PR55763: Port Magnum gio module to 97340 chipset
 *
 * Hydra_Software_Devel/29   4/27/09 11:07a jhaberf
 * PR53796: Updating gio build to support BCM35130 DTV chip.
 *
 * Hydra_Software_Devel/28   3/23/09 10:01p tdo
 * PR52975: BGIO PI support for 7635 "Dune" chip
 *
 * Hydra_Software_Devel/27   1/27/09 8:37p tdo
 * PR51627: add VDC 7336 PI support
 *
 * Hydra_Software_Devel/26   12/3/08 11:01a tdo
 * PR49907: incorrect pin mux setting for 7601 GIO
 *
 * Hydra_Software_Devel/25   9/16/08 12:15p tdo
 * PR46945: Fix bug to program sgpio pin when there is no Ext gio pins
 *
 * Hydra_Software_Devel/24   9/11/08 11:27a tdo
 * PR36114: Fix Ext Hi pin to unused for 7325
 *
 * Hydra_Software_Devel/23   9/10/08 7:50p tdo
 * PR46763: Add GIO PI support for 7420
 *
 * Hydra_Software_Devel/22   9/10/08 3:46p tdo
 * PR41941: Basic GIO PI support for 7335B0
 *
 * Hydra_Software_Devel/21   7/7/08 6:06p tdo
 * PR44530: BGIO support for 7601
 *
 * Hydra_Software_Devel/20   4/8/08 5:48p tdo
 * PR41205: Add _isr functions to avoid deadlock
 *
 * Hydra_Software_Devel/19   3/5/08 12:32p tdo
 * PR39459: Basic GIO PI support for 3556
 *
 * Hydra_Software_Devel/18   2/27/08 11:55a tdo
 * PR34956: Re-organize GPIO pin  mux and add 3548 support.
 *
 * Hydra_Software_Devel/17   2/14/08 6:25p pntruong
 * PR34956: Added stub to compile for 3548.
 *
 * Hydra_Software_Devel/16   11/20/07 10:53p tdo
 * PR36883: Add gio PI suppport for 7335
 *
 * Hydra_Software_Devel/15   10/15/07 2:29p yuxiaz
 * PR36114: Added GIO support for 7325.
 *
 * Hydra_Software_Devel/14   9/13/07 4:12p syang
 * PR 30391, PR 32351: clean up OpenDrain pin data set records when it is
 * destroied or changed to diff type
 *
 * Hydra_Software_Devel/13   9/12/07 6:34p syang
 * PR 30391, PR 32351: guard reg read and modify by kni critical section
 *
 * Hydra_Software_Devel/12   9/12/07 5:59p syang
 * PR 30391, PR 32351: BGIO only init for the pin created by BGIO to avoid
 * overriding configures by other sw entity; BGIO read from HW reg (no more
 * sw buffering);
 *
 * Hydra_Software_Devel/11   5/18/07 3:20p syang
 * PR 31356: add gpio PI support for 7440 B0
 *
 * Hydra_Software_Devel/10   5/18/07 10:48a yuxiaz
 * PR30839: Added 7405 support in GIO.
 *
 * Hydra_Software_Devel/9   12/29/06 11:31a syang
 * PR 25750: add 7403 and 7400 B0 support
 *
 * Hydra_Software_Devel/8   7/21/06 11:25a syang
 * PR 22789: added support for gio control set *_EXT_HI and more pins with
 * control set *_EXT, added suuport for 7118, 3563 and 7440
 *
 * Hydra_Software_Devel/7   3/23/06 2:27p syang
 * PR 19670: added support for 7438 A0
 *
 * Hydra_Software_Devel/6   4/20/05 12:56p syang
 * PR 14421: be able to compile for 3560 now
 *
 * Hydra_Software_Devel/5   5/24/04 5:07p jasonh
 * PR 11189: Merge down from B0 to main-line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   5/7/04 5:02p syang
 * PR 10097: added RST register setting to block interrupt firing when it
 * is meant to be disabled
 *
 * Hydra_Software_Devel/4   4/12/04 5:21p syang
 * PR 10564: multiply BGIO_P_NUM_LOW_REGS by 4 in BGIO_P_CalcPinRegAndBit
 *
 * Hydra_Software_Devel/3   3/15/04 6:29p syang
 * PR 10097: fixed a problems related to reg index and offset
 *
 * Hydra_Software_Devel/2   2/24/04 7:20p syang
 * PR 9785: more api function implementations are added
 *
 * Hydra_Software_Devel/1   2/20/04 11:23a syang
 * PR 9785: init version
 *
 ***************************************************************************/

#include "bgio_priv.h"
#include "bchp_gio.h"
#include "bkni.h"

BDBG_MODULE(BGIO);


/***************************************************************************
 *
 * Utility functions
 *
 ***************************************************************************/
/*--------------------------------------------------------------------------
 * To be called by BGIO_P_WritePinRegBit and BGIO_P_ReadPinRegBit to calculate the
 * register offset relative to BGIO_P_REG_BASE and the bit offset
 * relative to bit 0, based on pin ID.
 */
BERR_Code BGIO_P_CalcPinRegAndBit(
	BGIO_PinId            ePinId,
	uint32_t              ulRegLow,        /* corresponding reg_low */
	uint32_t *            pulRegOffset,
	uint32_t *            pulBitOffset )
{
	const BGIO_P_PinSet  *pGioPinSet = BGIO_P_GetPinMapping();

	/* assert para from our private code */
	BDBG_ASSERT( NULL != pGioPinSet );
	BDBG_ASSERT( BGIO_PinId_eInvalid > ePinId );
	BDBG_ASSERT( (BGIO_P_REG_BASE <= ulRegLow) &&
				 (ulRegLow <= BGIO_P_REG_LOW_TOP ) );
	BDBG_ASSERT( NULL != pulRegOffset );
	BDBG_ASSERT( NULL != pulBitOffset );

	/* calc register and bit offset */
	if(pGioPinSet->eSetLo != BGIO_PinId_eInvalid &&
	   ePinId < pGioPinSet->eSetHi && ePinId < BGIO_PinId_eSgpio00)
	{
		/* _LO */
		*pulRegOffset = ulRegLow - BGIO_P_REG_BASE;
		*pulBitOffset = ePinId - pGioPinSet->eSetLo;
		if(pGioPinSet->ulSetSgio == 0)
			*pulBitOffset += pGioPinSet->ulSetSgioShift;
	}
	else if(pGioPinSet->eSetHi != BGIO_PinId_eInvalid &&
	        ePinId < pGioPinSet->eSetExt && ePinId < BGIO_PinId_eSgpio00)
	{
		/* _HI */
		*pulRegOffset = (BGIO_P_NUM_LOW_REGS * 4 * 1 + ulRegLow) - BGIO_P_REG_BASE;
		*pulBitOffset = ePinId - pGioPinSet->eSetHi;
		if(pGioPinSet->ulSetSgio == 1)
			*pulBitOffset += pGioPinSet->ulSetSgioShift;
	}
	else if(pGioPinSet->eSetExt != BGIO_PinId_eInvalid &&
	        ePinId < pGioPinSet->eSetExtHi && ePinId < BGIO_PinId_eSgpio00)
	{
		/* _EXT */
		*pulRegOffset = (BGIO_P_NUM_LOW_REGS * 4 * 2 + ulRegLow) - BGIO_P_REG_BASE;
		*pulBitOffset = (ePinId - pGioPinSet->eSetExt);
		if(pGioPinSet->ulSetSgio == 2)
			*pulBitOffset += pGioPinSet->ulSetSgioShift;
	}
	else if(pGioPinSet->eSetExtHi != BGIO_PinId_eInvalid &&
	        ePinId < pGioPinSet->eSetExt2 && ePinId < BGIO_PinId_eSgpio00)
	{
		/* _EXT_HI */
		*pulRegOffset = (BGIO_P_NUM_LOW_REGS * 4 * 3 + ulRegLow) - BGIO_P_REG_BASE;
		*pulBitOffset = ePinId - pGioPinSet->eSetExtHi;
		if(pGioPinSet->ulSetSgio == 3)
			*pulBitOffset += pGioPinSet->ulSetSgioShift;
	}
	else if(pGioPinSet->eSetExt2 != BGIO_PinId_eInvalid && ePinId < BGIO_PinId_eSgpio00)
	{
		/* _EXT2 */
		*pulRegOffset = (BGIO_P_NUM_LOW_REGS * 4 * 4 + ulRegLow) - BGIO_P_REG_BASE;
		*pulBitOffset = ePinId - pGioPinSet->eSetExt2;
		if(pGioPinSet->ulSetSgio == 4)
			*pulBitOffset += pGioPinSet->ulSetSgioShift;
	}
	else if(ePinId < BGIO_PinId_eAgpio00)
	{
		/* special gpio pins */
		*pulRegOffset = (BGIO_P_NUM_LOW_REGS * 4 * pGioPinSet->ulSetSgio + ulRegLow) - BGIO_P_REG_BASE;
		*pulBitOffset = ePinId - BGIO_PinId_eSgpio00;
	}
	else if(ePinId < BGIO_PinId_eAsgpio00)
	{
		/* Aon GPIO */
		*pulRegOffset = ulRegLow - BGIO_P_REG_BASE;
		*pulBitOffset = ePinId - BGIO_PinId_eAgpio00;
	}
	else
	{
		/* Aon SGPIO */
		*pulRegOffset = (BGIO_P_NUM_LOW_REGS * 4 * 1 + ulRegLow) - BGIO_P_REG_BASE;
		*pulBitOffset = ePinId - BGIO_PinId_eAsgpio00;
	}

	return BERR_SUCCESS;
}

/*--------------------------------------------------------------------------
 * To be called to write the GPIO pin's bit into one register
 */
BERR_Code BGIO_P_WritePinRegBit(
	BGIO_Handle           hGpio,
	BGIO_PinId            ePinId,
	BGIO_PinType          ePinType,
	uint32_t              ulRegLow,
	BGIO_PinValue         ePinValue,
	bool                  bInIsr )
{
	BERR_Code eResult = BERR_SUCCESS;
	uint32_t  ulRegOffset, ulBitOffset;
	uint32_t  ulRegValue;
	uint32_t  ulRegIndex = 0;
	uint32_t  ulOpenDrainSet = 0;
	uint32_t  ulRegBase = (ePinId < BGIO_PinId_eAgpio00) ? BGIO_P_REG_BASE : BGIO_P_AON_BASE;

	/* check input para */
	BDBG_OBJECT_ASSERT(hGpio, BGIO);
	BDBG_ASSERT( BGIO_PinId_eInvalid > ePinId );
	BDBG_ASSERT( (BGIO_P_REG_BASE <= ulRegLow) &&
				 (ulRegLow <= BGIO_P_REG_LOW_TOP ) );
	BDBG_ASSERT( BGIO_PinValue_eInvalid > ePinValue );

	/* read the HW register and modify it for this setting */
	eResult = BGIO_P_CalcPinRegAndBit( ePinId, ulRegLow,
									   &ulRegOffset, &ulBitOffset );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	if(!bInIsr)
	{
		BKNI_EnterCriticalSection();
	}
	ulRegValue = BREG_Read32( hGpio->hRegister, ulRegBase + ulRegOffset );

	/* for the data of other pins of open drain type, we can not write 0 only if HW
	 * reading returns 0, since it might be due to that some other device is pulling
	 * down the bus */
	if (BCHP_GIO_DATA_LO == ulRegLow)
	{
		ulRegIndex = ulRegOffset / 4;
		ulOpenDrainSet = hGpio->aulOpenDrainSet[ulRegIndex];
		ulRegValue = ulRegValue | ulOpenDrainSet;
	}

	/* set new value to the bit */
	ulRegValue = ulRegValue & (~ BGIO_P_BIT_MASK(ulBitOffset));
	ulRegValue = (ePinValue << ulBitOffset) | ulRegValue;

	/* write to HW */
	BREG_Write32( hGpio->hRegister, ulRegBase + ulRegOffset, ulRegValue );
	if(!bInIsr)
	{
		BKNI_LeaveCriticalSection();
	}

	BDBG_MSG(("Write: RegAddr=0x%08x, RegValue=0x%08x", ulRegBase + ulRegOffset, ulRegValue));

	/* record open drain pin data set for future modification to this register by some other pin */
	if ((BCHP_GIO_DATA_LO == ulRegLow) && (BGIO_PinType_eOpenDrain == ePinType))
	{
		hGpio->aulOpenDrainSet[ulRegIndex] =
			(ulOpenDrainSet & (~ BGIO_P_BIT_MASK(ulBitOffset))) | (ePinValue << ulBitOffset);
	}

	return BERR_TRACE(eResult);
}

/*--------------------------------------------------------------------------
 * To be called to write the GPIO pin's bit into one register
 */
BERR_Code BGIO_P_ReadPinRegBit(
	BGIO_Handle           hGpio,
	BGIO_PinId            ePinId,
	uint32_t              ulRegLow,
	BGIO_PinValue *       pePinValue )
{
	BERR_Code eResult = BERR_SUCCESS;
	uint32_t  ulRegOffset, ulBitOffset;
	uint32_t  ulRegValue;
	BGIO_PinValue  ePinValue;
	uint32_t  ulRegBase = (ePinId < BGIO_PinId_eAgpio00) ? BGIO_P_REG_BASE : BGIO_P_AON_BASE;

	/* check input para */
	BDBG_OBJECT_ASSERT(hGpio, BGIO);
	BDBG_ASSERT( BGIO_PinId_eInvalid > ePinId );
	BDBG_ASSERT( (BGIO_P_REG_BASE <= ulRegLow) &&
				 (ulRegLow <= BGIO_P_REG_LOW_TOP ) );
	BDBG_ASSERT( NULL != pePinValue );

	/* read the HW reg
	 * note: should not modify pGpio's records for this register based on the
	 * reading from HW, since it might be diff from user's last setting, such
	 * in as open dran case, and pGpio's records for this register will be
	 * used in BGIO_P_WritePinRegBit */
	eResult = BGIO_P_CalcPinRegAndBit( ePinId, ulRegLow,
									   &ulRegOffset, &ulBitOffset );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	ulRegValue = BREG_Read32( hGpio->hRegister, ulRegBase + ulRegOffset );
	ePinValue = (ulRegValue & BGIO_P_BIT_MASK(ulBitOffset)) >> ulBitOffset;

	BDBG_MSG(("Read: RegAddr=0x%08x, RegValue=0x%08x", ulRegBase + ulRegOffset, ulRegValue));

	*pePinValue = ePinValue;
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * To be called to add a pin handle into the pin list in BGIO's main
 * context
 */
BERR_Code BGIO_P_AddPinToList(
	BGIO_Handle           hGpio,
	BGIO_Pin_Handle       hPin )
{

	BDBG_OBJECT_ASSERT(hGpio, BGIO);
	BDBG_OBJECT_ASSERT(hPin, BGIO_PIN);

	/* add to the head */
	BLST_D_INSERT_HEAD(&hGpio->PinHead , hPin, Link);
	return BERR_SUCCESS;
}

/***************************************************************************
 * To be called to remove a pin handle from the pin list in BGIO's main
 * context
 */
BERR_Code BGIO_P_RemovePinFromList(
	BGIO_Handle           hGpio,
	BGIO_Pin_Handle       hPin )
{
	BDBG_OBJECT_ASSERT(hGpio, BGIO);
	BDBG_OBJECT_ASSERT(hPin, BGIO_PIN);
	BDBG_ASSERT( BGIO_P_GetPinHandle(hGpio, hPin->ePinId) );

	BLST_D_REMOVE(&hGpio->PinHead, hPin, Link);
	return BERR_SUCCESS;
}

/*--------------------------------------------------------------------------
 * To be called to get the pin handle for a PinId from the pin list in
 * BGIO's main context. NULL returned if it does not exist.
 */
BGIO_Pin_Handle BGIO_P_GetPinHandle(
	BGIO_Handle           hGpio,
	BGIO_PinId            ePinId )
{
	BGIO_P_Pin_Context *  pPin;

	BDBG_OBJECT_ASSERT(hGpio, BGIO);
	BDBG_ASSERT( BGIO_PinId_eInvalid > ePinId );

	/* check whether the pin is already being in use */
	pPin = BLST_D_FIRST(&hGpio->PinHead);
	while ( NULL != pPin )
	{
		if ( pPin->ePinId == ePinId )
		{
			return pPin;
		}
		pPin = BLST_D_NEXT(pPin, Link);
	}

	/* not found */
	return NULL;
}

/***************************************************************************
 * To be called to get the register handle
 */
BREG_Handle BGIO_P_GetRegisterHandle(
	BGIO_Handle           hGpio )
{

	BDBG_OBJECT_ASSERT(hGpio, BGIO);
	return hGpio->hRegister;
}

/* End of File */

