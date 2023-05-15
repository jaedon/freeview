/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bled.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 7/20/12 6:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/led/7405/bled.c $
 * 
 * Hydra_Software_Devel/7   7/20/12 6:42p jkim
 * SWNOOS-541: Add support for HW driven HH:MM clock LED display.
 * 
 * Hydra_Software_Devel/6   1/14/11 7:57p brianlee
 * SW7422-197: Make sure to zero out upper bits of LDK_CONTROL during
 * initialization.
 * 
 * Hydra_Software_Devel/5   6/5/06 4:28p erickson
 * PR21941: cleaned up minor warnings
 * 
 * Hydra_Software_Devel/4   5/27/04 5:15p brianlee
 * PR11238: Fixed code to work with 7038 B0.
 * 
 * Hydra_Software_Devel/3   11/4/03 6:59p brianlee
 * Get rid of enter/leave macros.
 * 
 * Hydra_Software_Devel/2   9/30/03 11:21a brianlee
 * Fixed a warning.
 * 
 * Hydra_Software_Devel/1   9/23/03 10:17a brianlee
 * Initial version.  This module includes stubs for now since 7038 A0 does
 * not contain LED module.
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bled.h"
#include "bchp_ldk.h"

#if (HLCD_SUPPORT==1)
#include "bchp_aon_ctrl.h"
#endif

BDBG_MODULE(bled);

#define	DEV_MAGIC_ID			((BERR_LED_ID<<16) | 0xFACE)

#define	BLED_CHK_RETCODE( rc, func )		\
do {										\
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										\
		goto done;							\
	}										\
} while(0)

/* Default values */
#define LED_DEFAULT_PRESCALE_HI		0x00
#define LED_DEFAULT_PRESCALE_LO		0x55
#define LED_DEFAULT_DUTYCYCLE_OFF	0x01
#define LED_DEFAULT_DUTYCYCLE_ON	0xAA
#define LED_DEFAULT_DEBOUNCE		0x40

/*******************************************************************************
*
*	Private Module Handles
*
*******************************************************************************/

typedef struct BLED_P_Handle
{
	uint32_t		magicId;					/* Used to check if structure is corrupt */
	BCHP_Handle 	hChip;
	BREG_Handle		hRegister;
} BLED_P_Handle;

/*******************************************************************************
*
*	Default Module Settings
*
*******************************************************************************/
static const BLED_Settings defLedSettings = 
{
	100									/* percent brightness */
};

/*******************************************************************************
*
*	Public Module Functions
*
*******************************************************************************/
BERR_Code BLED_Open(
	BLED_Handle *pLed,					/* [output] Returns handle */
	BCHP_Handle hChip,					/* Chip handle */
	BREG_Handle hRegister,				/* Register handle */
	const BLED_Settings *pDefSettings	/* Default settings */
	)
{
	BERR_Code 		retCode = BERR_SUCCESS;
 	BLED_Handle 	hDev;
 	uint32_t		lval;

	/* Sanity check on the handles we've been given. */
	BDBG_ASSERT( hChip );
	BDBG_ASSERT( hRegister );

	/* Alloc memory from the system heap */
	hDev = (BLED_Handle) BKNI_Malloc( sizeof( BLED_P_Handle ) );
	if( hDev == NULL )
	{
		*pLed = NULL;
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(("BLED_Open: BKNI_malloc() failed\n"));
		goto done;
	}

	hDev->magicId	= DEV_MAGIC_ID;
	hDev->hChip		= hChip;
	hDev->hRegister = hRegister;
	*pLed = hDev;

	/* Reset LED/Keypad core */
	lval = BREG_Read32 (hDev->hRegister, BCHP_LDK_CONTROL);
	lval |= BCHP_LDK_CONTROL_swr_MASK;
	lval &= 0x0f;						/* only keep lower 4 bits */
	BREG_Write32 (hDev->hRegister, BCHP_LDK_CONTROL, lval);
	
	lval &= ~BCHP_LDK_CONTROL_swr_MASK;
	lval |=  BCHP_LDK_CONTROL_ver_MASK;
	BREG_Write32 (hDev->hRegister, BCHP_LDK_CONTROL, lval);

	/* Set up LED */
	BREG_Write32 (hDev->hRegister, BCHP_LDK_PRESCHI, LED_DEFAULT_PRESCALE_HI);
	BREG_Write32 (hDev->hRegister, BCHP_LDK_PRESCLO, LED_DEFAULT_PRESCALE_LO);

	BLED_AdjustBrightness (hDev, pDefSettings->percentBrightness);

done:
	return( retCode );
}

BERR_Code BLED_Close(
	BLED_Handle hDev					/* Device handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;


	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	BKNI_Free( (void *) hDev );

	return( retCode );
}

BERR_Code BLED_GetDefaultSettings(
	BLED_Settings *pDefSettings,		/* [output] Returns default setting */
	BCHP_Handle hChip					/* Chip handle */
)
{
	BERR_Code retCode = BERR_SUCCESS;

	BSTD_UNUSED(hChip);

	*pDefSettings = defLedSettings;

	return( retCode );
}

BERR_Code BLED_Write ( 
	BLED_Handle 		hLed, 			/* Device handle */
	uint8_t				digit,			/* digit to write to */
	uint8_t				value			/* value to write */
)
{
	uint32_t	offset;
	
	switch (digit)
	{
		case 1:
			offset = BCHP_LDK_DIGIT1;
			break;

		case 2:
			offset = BCHP_LDK_DIGIT2;
			break;
			
		case 3:
			offset = BCHP_LDK_DIGIT3;
			break;

		case 4:
			offset = BCHP_LDK_DIGIT4;
			break;
			
		default:
			return BERR_INVALID_PARAMETER;
	}
	
	BREG_Write32 (hLed->hRegister, offset, (uint32_t)value);

	return BERR_SUCCESS;
}

BERR_Code BLED_AdjustBrightness ( 
	BLED_Handle 		hLed,				/* Device handle */
	uint8_t				percentBrightness	/* percent of brightness */
)
{
	uint8_t				ucDutyCycleOn;
	uint8_t				ucDutyCycleOff;
	uint32_t			dutyCycleClks;
	uint32_t			valueOn;

	dutyCycleClks 	= LED_DEFAULT_DUTYCYCLE_ON + LED_DEFAULT_DUTYCYCLE_OFF;

	valueOn 		= dutyCycleClks * percentBrightness / 100;
	ucDutyCycleOn 	= (uint8_t)valueOn;
	ucDutyCycleOff 	= dutyCycleClks - ucDutyCycleOn;

	BREG_Write32 (hLed->hRegister, BCHP_LDK_DUTYOFF, (uint32_t)ucDutyCycleOff);
	BREG_Write32 (hLed->hRegister, BCHP_LDK_DUTYON,  (uint32_t)ucDutyCycleOn);

	return BERR_SUCCESS;
}

BERR_Code BLED_SetDiscreteLED ( 
	BLED_Handle 		hLed, 			/* Device handle */
	bool				on,				/* turn on or off */
	uint8_t				ledStatusBit	/* bit to turn on or off */
)
{
	uint32_t			lval;

	if (ledStatusBit > 7)
		return BERR_INVALID_PARAMETER;
		
	lval = BREG_Read32 (hLed->hRegister, BCHP_LDK_STATUS);
	if (on)
		lval &= ~(1 << ledStatusBit);
	else
		lval |= (1 << ledStatusBit);

	BREG_Write32 (hLed->hRegister, BCHP_LDK_STATUS, lval);

	return BERR_SUCCESS;
}

#if (HLCD_SUPPORT==1)
BERR_Code BLED_StartHLCD ( 
	BLED_Handle 		hLed, 			/* Device handle */
	uint8_t				hour,			/* used to initialize the hour counter */
	uint8_t				min,			/* used	to initialize the minute counter */
	uint8_t				sec,			/* used to initialize the second counter */
	bool				hour_mode,		/* 1 count hours from 0 to 23. 0 count hours from 0 to 11 */
	bool				display_mode	/* 0: in 12-hour mode: display from 1 to 12; in 24-hour mode: display from 0 to 23 */
										/* 1: in 12-hour mode: display from 0 to 11; in 24-hour mode: display from 1 to 24 */
)
{
	uint32_t			lval;

    BDBG_ASSERT( hLed );

	if (hour_mode)
	{
		if (hour >= 24)
		{
			return BERR_INVALID_PARAMETER;
		}
	}
	else
	{
		if (hour >= 12)
		{
			return BERR_INVALID_PARAMETER;
		}
	}
	
	if (min >= 60)
	{
		return BERR_INVALID_PARAMETER;
	}

	if (sec >= 60)
	{
		return BERR_INVALID_PARAMETER;
	}
	lval = 0;
	lval |= (hour << BCHP_AON_CTRL_TIME_COUNTER_hour_counter_init_SHIFT);
	lval |= (min << BCHP_AON_CTRL_TIME_COUNTER_minute_counter_init_SHIFT);
	lval |= (sec << BCHP_AON_CTRL_TIME_COUNTER_second_counter_init_SHIFT);
	lval |= (hour_mode << BCHP_AON_CTRL_TIME_COUNTER_mode_12h_24h_init_SHIFT);
	lval |= (display_mode << BCHP_AON_CTRL_TIME_COUNTER_hour_display_mode_SHIFT);
	BREG_Write32 (hLed->hRegister, BCHP_AON_CTRL_TIME_COUNTER, lval);

	/* Write digit code to the register*/
	lval = 0;
	lval |= (LED_NUM0 << BCHP_AON_CTRL_LED_DIGIT_CODE_0_digit_code_0_SHIFT);
	lval |= (LED_NUM1 << BCHP_AON_CTRL_LED_DIGIT_CODE_0_digit_code_1_SHIFT);
	lval |= (LED_NUM2 << BCHP_AON_CTRL_LED_DIGIT_CODE_0_digit_code_2_SHIFT);
	lval |= (LED_NUM3 << BCHP_AON_CTRL_LED_DIGIT_CODE_0_digit_code_3_SHIFT);
	BREG_Write32 (hLed->hRegister, BCHP_AON_CTRL_LED_DIGIT_CODE_0, lval);
	lval = 0;
	lval |= (LED_NUM4 << BCHP_AON_CTRL_LED_DIGIT_CODE_1_digit_code_4_SHIFT);
	lval |= (LED_NUM5 << BCHP_AON_CTRL_LED_DIGIT_CODE_1_digit_code_5_SHIFT);
	lval |= (LED_NUM6 << BCHP_AON_CTRL_LED_DIGIT_CODE_1_digit_code_6_SHIFT);
	lval |= (LED_NUM7 << BCHP_AON_CTRL_LED_DIGIT_CODE_1_digit_code_7_SHIFT);
	BREG_Write32 (hLed->hRegister, BCHP_AON_CTRL_LED_DIGIT_CODE_1, lval);
	lval = 0;
	lval |= (LED_NUM8 << BCHP_AON_CTRL_LED_DIGIT_CODE_2_digit_code_8_SHIFT);
	lval |= (LED_NUM9 << BCHP_AON_CTRL_LED_DIGIT_CODE_2_digit_code_9_SHIFT);
	BREG_Write32 (hLed->hRegister, BCHP_AON_CTRL_LED_DIGIT_CODE_2, lval);

	/* 	program address offset of the hour/minute */
	lval = BREG_Read32 (hLed->hRegister, BCHP_AON_CTRL_LED_DIGIT_ADDR_OFFSET);
	lval &= ~BCHP_AON_CTRL_LED_DIGIT_ADDR_OFFSET_hour_msd_addr_offset_MASK;
	lval &= ~BCHP_AON_CTRL_LED_DIGIT_ADDR_OFFSET_hour_lsd_addr_offset_MASK;
	lval &= ~BCHP_AON_CTRL_LED_DIGIT_ADDR_OFFSET_minute_msd_addr_offset_MASK;
	lval &= ~BCHP_AON_CTRL_LED_DIGIT_ADDR_OFFSET_minute_lsd_addr_offset_MASK;
	lval |= (DIGIT1_OFFSET << BCHP_AON_CTRL_LED_DIGIT_ADDR_OFFSET_hour_msd_addr_offset_SHIFT);
	lval |= (DIGIT2_OFFSET << BCHP_AON_CTRL_LED_DIGIT_ADDR_OFFSET_hour_lsd_addr_offset_SHIFT);
	lval |= (DIGIT3_OFFSET << BCHP_AON_CTRL_LED_DIGIT_ADDR_OFFSET_minute_msd_addr_offset_SHIFT);
	lval |= (DIGIT4_OFFSET << BCHP_AON_CTRL_LED_DIGIT_ADDR_OFFSET_minute_lsd_addr_offset_SHIFT);
	BREG_Write32 (hLed->hRegister, BCHP_AON_CTRL_LED_DIGIT_ADDR_OFFSET, lval);

	/* enable hlcd by writing 0 and 1 to HLCD_CTRL*/
	BREG_Write32 (hLed->hRegister, BCHP_AON_CTRL_HLCD_CTRL, 0);
	BREG_Write32 (hLed->hRegister, BCHP_AON_CTRL_HLCD_CTRL, 1);

	return BERR_SUCCESS;
}


BERR_Code BLED_StopHLCD ( 
	BLED_Handle 		hLed 			/* Device handle */
)
{
	uint32_t		lval;
	uint8_t			cnt;
	BERR_Code 		retCode = BERR_SUCCESS;

    BDBG_ASSERT( hLed );

	/* Disable HLCD */
	BREG_Write32 (hLed->hRegister, BCHP_AON_CTRL_HLCD_CTRL, 0);

	/* wait until status becomes 0 */
      for(cnt = 0; cnt < 10; cnt++)
	{
		lval = BREG_Read32 (hLed->hRegister, BCHP_AON_CTRL_HLCD_CTRL);

		if ((lval & BCHP_AON_CTRL_HLCD_CTRL_hlcd_enable_status_MASK) == 0)
			break;
		/* wait 1 ms */
		BKNI_Delay(1000);
	}

	if(cnt == 10)
	{
		retCode = BERR_TIMEOUT;
	}

	return retCode;
}
#endif
