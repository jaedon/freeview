/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgio_pin_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/84 $
 * $brcm_Date: 7/2/12 11:16a $
 *
 * Module Description:
 *
 *
 * Revision History:
 * $brcm_Log: /magnum/portinginterface/gio/7425/bgio_pin_priv.c $
 * 
 * Hydra_Software_Devel/84   7/2/12 11:16a tdo
 * SW7425-3355: BGIO implementation does not appear to work for level
 * interrupts
 *
 * Hydra_Software_Devel/83   12/5/11 5:02p tdo
 * SW7552-90: Set AON GPIO as push-pull type through bgio PI
 *
 * Hydra_Software_Devel/82   10/24/11 5:21p tdo
 * SW7552-90: Set AON GPIO as push-pull type through bgio PI
 *
 * Hydra_Software_Devel/81   6/16/11 5:58p tdo
 * SWDTV-7592: add BDBG_OBJECT_ASSERT for BGIO.
 *
 * Hydra_Software_Devel/80   6/16/11 11:26a tdo
 * SWDTV-7292: Re-org GIO code
 *
 * Hydra_Software_Devel/79   6/13/11 2:38p tdo
 * SWDTV-7292: Add Magnum PI GIO to 35233
 *
 * Hydra_Software_Devel/78   6/7/11 1:21p tdo
 * SWDTV-7292: Add Magnum PI GIO to 35233
 *
 * Hydra_Software_Devel/77   6/1/11 4:03p syang
 * SW7401-4472: change interrupt config order as requested.
 *
 * Hydra_Software_Devel/76   5/2/11 5:05p franli
 * SWDTV-6889:Add board specific configure in vdc test
 *
 * Hydra_Software_Devel/75   4/13/11 6:54p tdo
 * SW7425-112: Add GIO support for 7425 B0 chip
 *
 * Hydra_Software_Devel/75   4/13/11 6:15p tdo
 * SW7425-112: Add GIO support for 7425 B0 chip
 *
 * Hydra_Software_Devel/74   3/29/11 12:02p pblanco
 * SW35125-43: Fix compiler warning for 35125.
 *
 * Hydra_Software_Devel/73   3/21/11 5:43p jhaberf
 * SWDTV-6095: Added dummy placeholder for 35233 DTV chip
 *
 * Hydra_Software_Devel/72   3/21/11 3:47p jhaberf
 * SW35330-13: Added support for 35233 DTV chip
 *
 * Hydra_Software_Devel/71   12/23/10 1:23p zhang
 * SW35125-43: Added 35125 defines and fixed pin creation bug.
 *
 * Hydra_Software_Devel/70   12/6/10 4:08p jhaberf
 * SW35230-1: Added 35125 to the build
 *
 * Hydra_Software_Devel/69   12/2/10 2:38p tdo
 * SWBLURAY-23686: Add GIO PortingInterface support for Blast (7640) chip
 *
 * Hydra_Software_Devel/68   11/23/10 11:29a jrubio
 * SW7344-9: update 7346
 *
 * Hydra_Software_Devel/67   11/17/10 9:02a tdo
 * SW7231-11: Add GIO PI support for 7231/7344/7346
 *
 * Hydra_Software_Devel/66   11/3/10 6:14p xhuang
 * SW7358-3: sync with RDB change
 *
 * Hydra_Software_Devel/65   11/1/10 5:00p xhuang
 * SW7552-4: Add 7552 support
 *
 * Hydra_Software_Devel/64   9/8/10 4:33p xhuang
 * SW7358-3: update according to RDB change
 *
 * Hydra_Software_Devel/63   8/30/10 3:23p tdo
 * SW7425-22: Add GIO PI support for 7425
 *
 * Hydra_Software_Devel/62   8/30/10 3:18p tdo
 * SW7425-22: Add GIO PI support for 7425
 *
 * Hydra_Software_Devel/61   8/12/10 12:59p tdo
 * SW7358-6: Add support for 7358
 *
 * Hydra_Software_Devel/SW7358-6/1   8/12/10 2:48p xhuang
 * SW7358-6: Add support for 7358 in gio
 *
 * Hydra_Software_Devel/60   7/28/10 10:23p tdo
 * SWBLURAY-21288: Add support for real gpio pins (not pin muxed) for
 * Quick (7631) chip
 *
 * Hydra_Software_Devel/59   6/29/10 3:23p tdo
 * SWBLURAY-21288: Need GIO PI updated for Quick (7631) chip
 *
 * Hydra_Software_Devel/58   6/24/10 7:07p vanessah
 * SW7422-12:  Naming convention problem for the registers.
 *
 * Hydra_Software_Devel/56   6/22/10 11:40a vanessah
 * SW7422-12:  To support appframework. Missing files added:
 * magnum\portinginterface\pwr rockford\appframework\src\board\97422  To
 * do list: 1. in framework_board.c, more initialization to be done.  2.
 * More registers mapping, like clock generation as well as
 * BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL etc
 *
 * Hydra_Software_Devel/55   1/5/10 1:02p jhaberf
 * SW35230-1: Check in of file on behalf of Srinivasa M.P. Reddy in order
 * to get 35230 RAP PI compiling
 *
 * Hydra_Software_Devel/54   11/19/09 11:14a tdo
 * SW7468-23: Create GIO PI for 7468
 *
 * Hydra_Software_Devel/53   11/18/09 11:35p tdo
 * SW7408-22: Add GIO PI support for 7408
 *
 * Hydra_Software_Devel/52   9/1/09 2:37p yuxiaz
 * SW7550-28: Add GIO pinmux support for 7125.
 *
 * Hydra_Software_Devel/51   9/1/09 11:11a tdo
 * SW7550-28: Add GIO pinmux support for 7550
 *
 * Hydra_Software_Devel/50   8/28/09 3:07p yuxiaz
 * SW7550-28: Add GIO pinmux support for 7550.
 *
 * Hydra_Software_Devel/49   8/27/09 6:29p tdo
 * SW7630-15: Bringup of portinginterface "gio" for Grain (7630) and 7342
 *
 * Hydra_Software_Devel/48   6/17/09 4:34p tdo
 * PR55763: Fix gpio_52 for 7340
 *
 * Hydra_Software_Devel/47   6/11/09 9:30p tdo
 * PR55763: Port Magnum gio module to 97340 chipset
 *
 * Hydra_Software_Devel/46   4/27/09 11:07a jhaberf
 * PR53796: Updating gio build to support BCM35130 DTV chip.
 *
 * Hydra_Software_Devel/45   3/24/09 4:11p tdo
 * PR52975: Fix compiling error
 *
 * Hydra_Software_Devel/44   3/23/09 10:01p tdo
 * PR52975: BGIO PI support for 7635 "Dune" chip
 *
 * Hydra_Software_Devel/43   1/27/09 8:37p tdo
 * PR51627: add VDC 7336 PI support
 *
 * Hydra_Software_Devel/42   9/10/08 7:50p tdo
 * PR46763: Add GIO PI support for 7420
 *
 * Hydra_Software_Devel/41   9/9/08 2:01p tdo
 * PR46701: Add support for GIO for 3548B0
 *
 * Hydra_Software_Devel/40   7/7/08 6:09p tdo
 * PR44530: BGIO support for 7601
 *
 * Hydra_Software_Devel/39   7/7/08 6:06p tdo
 * PR44530: BGIO support for 7601
 *
 * Hydra_Software_Devel/38   6/30/08 2:06p tdo
 * PR44360: Remove FORWARD_NULL Coverity Defect
 *
 * Hydra_Software_Devel/37   6/10/08 1:17p tdo
 * PR41941: Basic GIO PI support for 7335B0
 *
 * Hydra_Software_Devel/36   4/30/08 6:59p tdo
 * PR34956: gpio pin name change for 7325
 *
 * Hydra_Software_Devel/35   4/8/08 5:47p tdo
 * PR41205: Add _isr functions to avoid deadlock
 *
 * Hydra_Software_Devel/34   3/5/08 12:31p tdo
 * PR39459: Basic GIO PI support for 3556
 *
 * Hydra_Software_Devel/33   2/27/08 11:54a tdo
 * PR34956: Re-organize GPIO pin  mux and add 3548 support.
 *
 * Hydra_Software_Devel/32   2/14/08 6:25p pntruong
 * PR34956: Added stub to compile for 3548.
 *
 * Hydra_Software_Devel/31   11/20/07 10:53p tdo
 * PR36883: Add gio PI suppport for 7335
 *
 * Hydra_Software_Devel/30   10/15/07 2:29p yuxiaz
 * PR36114: Added GIO support for 7325.
 *
 * Hydra_Software_Devel/29   9/13/07 4:12p syang
 * PR 30391, PR 32351: clean up OpenDrain pin data set records when it is
 * destroied or changed to diff type
 *
 * Hydra_Software_Devel/28   9/12/07 6:35p syang
 * PR 30391, PR 32351: guard reg read and modify by kni critical section
 *
 * Hydra_Software_Devel/27   9/12/07 5:58p syang
 * PR 30391, PR 32351: BGIO only init for the pin created by BGIO to avoid
 * overriding configures by other sw entity; BGIO read from HW reg (no more
 * sw buffering);
 *
 * Hydra_Software_Devel/26   5/18/07 3:25p syang
 * PR 31356: add gpio PI support for 7440 B0
 *
 * Hydra_Software_Devel/25   5/18/07 10:47a yuxiaz
 * PR30839: Added 7405 support in GIO.
 *
 * Hydra_Software_Devel/24   12/29/06 11:31a syang
 * PR 25750: add 7403 and 7400 B0 support
 *
 * Hydra_Software_Devel/23   10/4/06 3:14p syang
 * PR 23355: fixed a typo in 3563 pin mux entry
 *
 * Hydra_Software_Devel/22   10/4/06 12:01p syang
 * PR 23536: update pin mux in sun_top_ctrl for 7440
 *
 * Hydra_Software_Devel/21   7/21/06 11:27a syang
 * PR 22789: added support for gio control set *_EXT_HI and more pins with
 * control set *_EXT, added suuport for 7118, 3563 and 7440
 *
 * Hydra_Software_Devel/20   6/21/06 6:41p syang
 * PR 16058: added 7401 B0 support
 *
 * Hydra_Software_Devel/19   3/23/06 2:27p syang
 * PR 19670: added support for 7438 A0
 *
 * Hydra_Software_Devel/18   2/3/06 5:01p syang
 * PR 19425: added 7400 support
 *
 * Hydra_Software_Devel/17   7/14/05 5:05p syang
 * PR 16058: added 7401 support
 *
 * Hydra_Software_Devel/16   6/24/05 10:08a syang
 * PR 14720: added C1 support
 *
 * Hydra_Software_Devel/15   5/13/05 12:59p syang
 * PR 14720: added 7038 B2 and C1 support
 *
 * Hydra_Software_Devel/14   4/20/05 12:55p syang
 * PR 14421: be able to compile for 3560 now
 *
 * Hydra_Software_Devel/13   3/18/05 6:30p syang
 * PR 14421: updated SUN_TOP_CTRL_* registers for 3560
 *
 * Hydra_Software_Devel/12   3/16/05 12:31p syang
 * PR 14421: added support for 3560
 *
 * Hydra_Software_Devel/11   2/1/05 3:49p jasonh
 * PR 14009: Fixed chip revision detection.
 *
 * Hydra_Software_Devel/10   1/25/05 4:50p syang
 * PR 1344: fixed the mis-using of BVDC_P macro
 *
 * Hydra_Software_Devel/9   1/25/05 10:14a syang
 * PR 1344: corrected the gpio pin order in s_aPinMux[]
 *
 * Hydra_Software_Devel/8   1/24/05 7:27p syang
 * PR 1344: added C0 support
 *
 * Hydra_Software_Devel/7   7/27/04 11:21a syang
 * PR 10548: changed to use BCHP_MASK and BCHP_SHIFT
 *
 * Hydra_Software_Devel/6   5/24/04 5:07p jasonh
 * PR 11189: Merge down from B0 to main-line
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/3   5/7/04 5:02p syang
 * PR 10097: added RST register setting to block interrupt firing when it
 * is meant to be disabled
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/2   4/28/04 11:10a syang
 * PR 10097: write 1 and then 0 to interrupt clear bit
 *
 * Hydra_Software_Devel/Refsw_Devel_7038_B0/1   4/20/04 2:28p syang
 * PR 10687: updated sun_top_ctrl registers to B0
 *
 * Hydra_Software_Devel/4   3/15/04 6:28p syang
 * PR 10097: fixed a comparing problem in BGIO_P_Pin_SetType
 *
 * Hydra_Software_Devel/3   2/24/04 7:20p syang
 * PR 9785: more api function implementations are added
 *
 * Hydra_Software_Devel/2   2/20/04 4:53p syang
 * PR 9785: check in before clearcase upgrade world wise
 *
 * Hydra_Software_Devel/1   2/20/04 11:23a syang
 * PR 9785: init version
 *
 ***************************************************************************/

#include "bgio_pin_priv.h"
#include "bgio_priv.h"
#include "berr.h"
#include "bkni.h"
#include "bchp_gio.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_common.h"
#ifdef BCHP_AON_PIN_CTRL_REG_START
#include "bchp_aon_pin_ctrl.h"
#endif
#include "bkni.h"

BDBG_MODULE(BGIO);
BDBG_OBJECT_ID(BGIO_PIN);

/***************************************************************************
 *
 * Utility functions
 *
 ***************************************************************************/
static BERR_Code BGIO_P_Pin_ClearOpenDrainSet(
	BGIO_Handle           hGpio,
	BGIO_PinId            ePinId )
{
	BERR_Code eResult = BERR_SUCCESS;
	uint32_t  ulRegOffset, ulBitOffset;
	uint32_t  ulRegIndex = 0;

	BDBG_OBJECT_ASSERT(hGpio, BGIO);
	BDBG_ASSERT( BGIO_PinId_eInvalid > ePinId );

	/* read the HW register and modify it for this setting */
	eResult = BGIO_P_CalcPinRegAndBit( ePinId, BCHP_GIO_DATA_LO,
									   &ulRegOffset, &ulBitOffset );
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	if (BERR_SUCCESS == eResult )
	{
		ulRegIndex = ulRegOffset / 4;
		hGpio->aulOpenDrainSet[ulRegIndex] &= (~ BGIO_P_BIT_MASK(ulBitOffset));
	}

	return eResult;
}


/***************************************************************************
 *
 * API support functions
 *
 ***************************************************************************/


#define BGIO_P_PIN_MUX_SEL_GPIO     0
/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_Create(
	BGIO_Handle           hGpio,
	BGIO_PinId            ePinId,
	BGIO_Pin_Handle *     phPin )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BGIO_P_Pin_Context *  pPin = NULL;
	const BGIO_P_PinMux *  pPinMux;
	BREG_Handle  hRegister;
	uint32_t ulRegValue;

	if ( NULL != phPin )
		*phPin = NULL;

	BDBG_OBJECT_ASSERT(hGpio, BGIO);
	pPinMux = BGIO_P_GetPinMux(ePinId);
	if ((NULL == phPin) ||
		(BGIO_PinId_eInvalid <= ePinId) ||
		(pPinMux->ulReg == BGIO_P_NULL_REG) )
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* GPIO pin can not share among two apps */
	if ( NULL != BGIO_P_GetPinHandle(hGpio, ePinId) )
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* allocate pin gio sub-module's context */
	pPin = (BGIO_P_Pin_Context *)BKNI_Malloc( sizeof(BGIO_P_Pin_Context) );
	if ( NULL == pPin )
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BKNI_Memset((void*)pPin, 0x0, sizeof(BGIO_P_Pin_Context));
	BDBG_OBJECT_SET(pPin, BGIO_PIN);

	if(pPinMux->ulReg != BGIO_P_GIO_REG)
	{
		/* set pin mux to make the pin work as GPIO pin */
		hRegister = BGIO_P_GetRegisterHandle( hGpio );
		BKNI_EnterCriticalSection();
		ulRegValue = BREG_Read32(hRegister, pPinMux->ulReg) &
			~ pPinMux->ulBitMask;
		ulRegValue |= pPinMux->ulValue;
		BREG_Write32( hRegister, pPinMux->ulReg, ulRegValue );
		BKNI_LeaveCriticalSection();
	}

	/* init pin sub-module's main context */
	BGIO_P_PIN_SET_BLACK_MAGIC( pPin );
	pPin->hGpio = hGpio;
	pPin->ePinId = ePinId;

	eResult = BGIO_P_Pin_SetType( pPin, BGIO_PinType_eInput, false );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	eResult = BGIO_P_Pin_SetIntrMode( pPin, BGIO_IntrMode_eDisabled, false );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	eResult = BGIO_P_Pin_ClearIntrStatus( pPin, false );
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	/* connect pin gio sub-module to gio module's main context */
	eResult = BGIO_P_AddPinToList( hGpio, pPin );

	*phPin = pPin;
	return eResult;
}

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_Destroy(
	BGIO_Pin_Handle       hPin )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BDBG_OBJECT_ASSERT(hPin, BGIO_PIN);

	/* block the pin's driving and interrupt,
	 * important for other pins' Pin_SetValue later */
	eResult = BGIO_P_Pin_SetType( hPin, BGIO_PinType_eInput, false );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	eResult = BGIO_P_Pin_SetIntrMode( hPin, BGIO_IntrMode_eDisabled, false );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	eResult = BGIO_P_Pin_ClearOpenDrainSet(hPin->hGpio, hPin->ePinId);
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	/* remove pin handle from the pin list in gio module's main context */
	eResult = BGIO_P_RemovePinFromList( hPin->hGpio, hPin );
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	BDBG_OBJECT_DESTROY(hPin, BGIO_PIN);
	BKNI_Free((void*)hPin);
	return eResult;
}

/***************************************************************************
 *
 */
#define  BGIO_P_PUSH_PULL           BGIO_PinValue_e0
#define  BGIO_P_OPEN_DRAIN          BGIO_PinValue_e1
#define  BGIO_P_OUTPUT              BGIO_PinValue_e0
#define  BGIO_P_INPUT_ONLY          BGIO_PinValue_e1

BERR_Code BGIO_P_Pin_SetType(
	BGIO_Pin_Handle       hPin,
	BGIO_PinType          ePinType,
	bool                  bInIsr )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BGIO_PinValue  eValIoDir, eValOdEn;

	BDBG_OBJECT_ASSERT(hPin, BGIO_PIN);
	if (BGIO_PinType_eInvalid <= ePinType)
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		return eResult;
	}

	/* calc reg values */
	switch ( ePinType )
	{
	case BGIO_PinType_eInput:
		eValIoDir = BGIO_P_INPUT_ONLY;
		eValOdEn = BGIO_P_PUSH_PULL;
		break;
	case BGIO_PinType_ePushPull:
		/* special GPIO pins can not work as push-pull type */
		if((BGIO_PinId_eSgpio00 <= hPin->ePinId && hPin->ePinId < BGIO_PinId_eAgpio00) ||
		   (BGIO_PinId_eAsgpio00 <= hPin->ePinId))
		{
			eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
			return eResult;
		}
		eValIoDir = BGIO_P_OUTPUT;
		eValOdEn = BGIO_P_PUSH_PULL;
		break;
	case BGIO_PinType_eOpenDrain:
		eValIoDir = BGIO_P_OUTPUT;
		eValOdEn = BGIO_P_OPEN_DRAIN;
		break;
	default:
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		return eResult;
	}
	hPin->ePinType = ePinType;

	/* modify hGpio's register value records for this pin and write HW reg */
	eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
									 BCHP_GIO_IODIR_LO, eValIoDir, bInIsr );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
									 BCHP_GIO_ODEN_LO, eValOdEn, bInIsr );
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	/* TODO: init value for push-pull XXX ??? */
	if ( BGIO_PinType_eOpenDrain == ePinType )
		eResult = BGIO_P_Pin_SetValue( hPin, BGIO_PinValue_e1, bInIsr ); /* release */
	else
		eResult = BGIO_P_Pin_ClearOpenDrainSet(hPin->hGpio, hPin->ePinId);
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	return eResult;
}

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_GetValue(
	BGIO_Pin_Handle       hPin,
	BGIO_PinValue *       pePinValue )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BGIO_PinValue         ePinValue = BGIO_PinValue_eInvalid;

	BDBG_OBJECT_ASSERT(hPin, BGIO_PIN);
	if (NULL == pePinValue)
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		return eResult;
	}

	/* read the pin value from HW reg */
	eResult = BGIO_P_ReadPinRegBit( hPin->hGpio, hPin->ePinId, BCHP_GIO_DATA_LO,
									&ePinValue );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	*pePinValue = ePinValue;
	return eResult;
}

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_SetValue(
	BGIO_Pin_Handle       hPin,
	BGIO_PinValue         ePinValue,
	bool                  bInIsr )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_OBJECT_ASSERT(hPin, BGIO_PIN);
	if (BGIO_PinValue_eInvalid <= ePinValue)
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		return eResult;
	}

	/* modify hGpio's register value records for this pin and write HW reg */
	eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
									 BCHP_GIO_DATA_LO, ePinValue, bInIsr );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	return eResult;
}

/***************************************************************************
 *
 */
#define  BGIO_P_INTR_RESET          BGIO_PinValue_e0  /* reset to work normal */
#define  BGIO_P_INTR_CLEAR          BGIO_PinValue_e1  /* clear and block */
#define  BGIO_P_INTR_DISABLE        BGIO_PinValue_e0
#define  BGIO_P_INTR_ENABLE         BGIO_PinValue_e1
#define  BGIO_P_INTR_FALL_EDGE      BGIO_PinValue_e0
#define  BGIO_P_INTR_RISE_EDGE      BGIO_PinValue_e1
#define  BGIO_P_INTR_ONE_EDGE       BGIO_PinValue_e0
#define  BGIO_P_INTR_BOTH_EDGE      BGIO_PinValue_e1
#if (BCHP_CHIP != 7038) && (BCHP_CHIP != 7438)
#define  BGIO_P_INTR_EDGE           BGIO_PinValue_e0
#define  BGIO_P_INTR_LEVEL          BGIO_PinValue_e1
#define  BGIO_P_INTR_0              BGIO_PinValue_e0
#define  BGIO_P_INTR_1              BGIO_PinValue_e1
#endif

BERR_Code BGIO_P_Pin_SetIntrMode(
	BGIO_Pin_Handle       hPin,
	BGIO_IntrMode         eIntrMode,
	bool                  bInIsr )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BGIO_PinValue  eIntrEna, eEdgeSel, eEdgeInsen;
#if (BCHP_CHIP != 7038) && (BCHP_CHIP != 7438)
	BGIO_PinValue  eLvlEdge;
#endif

	BDBG_OBJECT_ASSERT(hPin, BGIO_PIN);
	if (BGIO_IntrMode_eInvalid <= eIntrMode)
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		return eResult;
	}

	/* calc reg values */
	eEdgeInsen = BGIO_P_INTR_ONE_EDGE;
#if (BCHP_CHIP != 7038) && (BCHP_CHIP != 7438)
	eLvlEdge = BGIO_P_INTR_EDGE;
#endif
	switch ( eIntrMode )
	{
	case BGIO_IntrMode_eDisabled:
		eIntrEna = BGIO_P_INTR_DISABLE;
		eEdgeSel = BGIO_P_INTR_FALL_EDGE;
		break;
	case BGIO_IntrMode_e0To1:
		eIntrEna = BGIO_P_INTR_ENABLE;
		eEdgeSel = BGIO_P_INTR_RISE_EDGE;
		break;
	case BGIO_IntrMode_e1To0:
		eIntrEna = BGIO_P_INTR_ENABLE;
		eEdgeSel = BGIO_P_INTR_FALL_EDGE;
		break;
	case BGIO_IntrMode_e0To1_Or_1To0:
		eIntrEna = BGIO_P_INTR_ENABLE;
		eEdgeSel = BGIO_P_INTR_FALL_EDGE;
		eEdgeInsen = BGIO_P_INTR_BOTH_EDGE;
		break;
#if (BCHP_CHIP != 7038) && (BCHP_CHIP != 7438)
	case BGIO_IntrMode_e0:
		eIntrEna = BGIO_P_INTR_ENABLE;
		eLvlEdge = BGIO_P_INTR_LEVEL;
		eEdgeSel = BGIO_P_INTR_0;
		break;
	case BGIO_IntrMode_e1:
		eIntrEna = BGIO_P_INTR_ENABLE;
		eLvlEdge = BGIO_P_INTR_LEVEL;
		eEdgeSel = BGIO_P_INTR_1;
		break;
#endif
	default:
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		return eResult;
	}
	hPin->eIntrMode = eIntrMode;

	/* modify hGpio's register value records for this pin and write HW reg */
#if (BCHP_CHIP == 7038) || (BCHP_CHIP==7438)
	/* Both MASK and RESET in RST are needed to really enable interrupt */
	eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
									 BCHP_GIO_RST_LO, BGIO_P_INTR_CLEAR, bInIsr );
	BDBG_ASSERT( BERR_SUCCESS == eResult ); /* clear and block for 7038 */
	if ( BGIO_IntrMode_eDisabled != eIntrMode )
	{
		eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
										 BCHP_GIO_RST_LO, BGIO_P_INTR_RESET, bInIsr );
		BDBG_ASSERT( BERR_SUCCESS == eResult ); /* reset to work normal */
	}
#else
	eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
									 BCHP_GIO_STAT_LO, BGIO_P_INTR_CLEAR, bInIsr );
	BDBG_ASSERT( BERR_SUCCESS == eResult ); /* clear and work normal */
	eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
									 BCHP_GIO_LEVEL_LO, eLvlEdge, bInIsr );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
#endif
	eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
									 BCHP_GIO_EC_LO, eEdgeSel, bInIsr );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
									 BCHP_GIO_EI_LO, eEdgeInsen, bInIsr );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
									 BCHP_GIO_MASK_LO, eIntrEna, bInIsr );
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	return eResult;
}

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_ClearIntrStatus(
	BGIO_Pin_Handle       hPin,
	bool                  bInIsr )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BDBG_OBJECT_ASSERT(hPin, BGIO_PIN);

#if (BCHP_CHIP == 7038) || (BCHP_CHIP==7438)
	/* modify hGpio's register value records for this pin and write HW reg
	 * needs to write a "1" and a "0" to the clear bit to clear the
	 * interrupt status and to set ready for new interrupt for the pin */
	eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
									 BCHP_GIO_RST_LO, BGIO_PinValue_e1, bInIsr );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
	eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
									 BCHP_GIO_RST_LO, BGIO_PinValue_e0, bInIsr );
	BDBG_ASSERT( BERR_SUCCESS == eResult );
#else
	if ( (BGIO_IntrMode_e0 != hPin->eIntrMode) &&
		 (BGIO_IntrMode_e1 != hPin->eIntrMode) )
	{
		eResult = BGIO_P_WritePinRegBit( hPin->hGpio, hPin->ePinId, hPin->ePinType,
										 BCHP_GIO_STAT_LO, BGIO_PinValue_e1, bInIsr );
	}
	else
	{
		BDBG_ERR(("Level trigered intr must be cleared by changing pin value"));
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
	}
#endif
	return eResult;
}

/***************************************************************************
 *
 */
BERR_Code BGIO_P_Pin_GetIntrStatus(
	BGIO_Pin_Handle       hPin,
	bool *                pbFire )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BGIO_PinValue  ePinValue = BGIO_PinValue_eInvalid;

	BDBG_OBJECT_ASSERT(hPin, BGIO_PIN);
	if (NULL == pbFire)
	{
		eResult = BERR_TRACE(BERR_INVALID_PARAMETER);
		return eResult;
	}

	/* read the pin value from HW reg */
	eResult = BGIO_P_ReadPinRegBit( hPin->hGpio, hPin->ePinId,
									BCHP_GIO_STAT_LO, &ePinValue );
	BDBG_ASSERT( BERR_SUCCESS == eResult );

	*pbFire = (BGIO_PinValue_e1 == ePinValue);
	return eResult;
}


/* End of File */

