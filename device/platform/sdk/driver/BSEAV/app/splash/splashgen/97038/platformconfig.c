/***************************************************************************
 *     Copyright (c) 2005-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: platformconfig.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 5/14/07 6:57p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/splash/splashgen/97038/platformconfig.c $
 * 
 * 1   5/14/07 6:57p shyam
 * PR 30741 : Add reference support for generic portable splash
 * 
 * Hydra_Software_Devel/1   11/17/05 4:46p dkaufman
 * PR3481: Added file
 * 
 *
 ***************************************************************************/

#include "platformconfig.h"

#include "bchp_gio.h" /* For platform config */
#include "bchp_sun_top_ctrl.h"


BERR_Code ConfigPlatform(BREG_Handle hRegister)
{
	uint32_t	lval;
	
	/* Pin Mux */
	
	/*
	 * GPIO29 Controls Power for S-Video out
	 */
	lval = BREG_Read32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);

	lval &= ~(
				BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7,  gpio_29)  
			  );
	lval |= 
				BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_29, 0); /* 0: gpio_29 */

	BREG_Write32 (hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, lval);

	
	/* Gpio Config */
	
	/*
	 * GPIO29 Controls Power for S-Video out
	 */
	/* Set GPIO29 as output */
	lval = BREG_Read32 (hRegister, BCHP_GIO_IODIR_LO);
	lval &= ~(1 << 29);
	BREG_Write32 (hRegister, BCHP_GIO_IODIR_LO, lval);
	
	/* Set GPIO Output type */
	lval = BREG_Read32 (hRegister, BCHP_GIO_ODEN_LO);
	lval &= ~(1 << 29);
	lval |= (0 << 29); /* 1: open-drain output, 0: totem-pole output */
	BREG_Write32 (hRegister, BCHP_GIO_ODEN_LO, lval);
	
	/* Set GPIO Output level */
	lval = BREG_Read32 (hRegister, BCHP_GIO_DATA_LO);
	lval &= ~(1 << 29);
	lval |= (1 << 29); /* 1: high, 0: low */
	BREG_Write32 (hRegister, BCHP_GIO_DATA_LO, lval);
	
	
	return BERR_SUCCESS;
	
}

