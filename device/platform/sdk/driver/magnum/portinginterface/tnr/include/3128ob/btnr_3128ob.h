/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3128ob.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/16/12 12:02p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/3128ob/btnr_3128ob.h $
 * 
 * Hydra_Software_Devel/2   5/16/12 12:02p atanugul
 * SW3128-164: Add Out Of Band Inpur Mode Selection
 * 
 * Hydra_Software_Devel/1   4/11/11 5:44p atanugul
 * SW3128-2: Add AOB TNR support for 3128
 * 
 ****************************************************************************/
#ifndef BTNR_3128OB_H__
#define BTNR_3128OB_H__

#include "bchp.h"
#include "breg_i2c.h"
#include "bhab.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BTNR_OOB_RF_INPUT_MODE 0x24
/***************************************************************************
Summary:
    Enumeration for RF Input mode

Description:
    This enumeration defines the AOB RF Input mode

See Also:

****************************************************************************/
typedef enum BTNR_3128Ob_OutOfBandInputMode
{
    BTNR_3128Ob_OutOfBandInputMode_eOutOfBandAdc,   /* OOB ADC connected to OOB Demod */
    BTNR_3128Ob_OutOfBandInputMode_eWideBandAdc,    /* WB ADC connected to OOB Demod */
    BTNR_3128Ob_OutOfBandInputMode_eLast    
} BTNR_3128Ob_OutOfBandInputMode;

typedef struct
{
	unsigned short i2cAddr;			/* 7bit I2C address of Bcm3128 */	
	unsigned ifFreq;
    BTNR_3128Ob_OutOfBandInputMode inputMode;   /* RF input mode for AOB */
} BTNR_3128Ob_Settings;

/***************************************************************************
Summary:
	This function returns the default settings for Bcm3128 Tuner module.

Description:
	This function is responsible for returns the default setting for 
	BTNR module. The returning default setting should be when
	opening the device.

Returns:
	TODO:

See Also:
	BTNR_3128_Open()

****************************************************************************/
BERR_Code BTNR_3128Ob_GetDefaultSettings(
	BTNR_3128Ob_Settings *pDefSettings	/* [out] Returns default setting */
	);


/***************************************************************************
Summary:
	This function opens Bcm3128 Tuner module.

Description:
	This function is responsible for opening Bcm3128 BTNR module. When BTNR is
	opened, it will create a module handle and configure the module based
	on the default settings. Once the device is opened, it must be closed
	before it can be opened again.

Returns:
	TODO:

See Also:
	BTNR_Close(), BTNR_3128_GetDefaultSettings()

****************************************************************************/
BERR_Code BTNR_3128Ob_Open(
	BTNR_Handle *phDev,					/* [out] Returns handle */
	BTNR_3128Ob_Settings *pSettings,
	BHAB_Handle hHab
	);

#ifdef __cplusplus
}
#endif
 
#endif



