/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_ob_3x7x.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/7/12 11:11a $
 *
 * Module Description:
 *
 * Revision History:  $
 *
 * $brcm_Log: /magnum/portinginterface/tnr/include/3x7xob/btnr_ob_3x7x.h $
 * 
 * Hydra_Software_Devel/1   9/7/12 11:11a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/1   9/7/12 10:18a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/3   3/12/12 1:07p farshidf
 * SW3461-1: add the AOB to tuner callback
 * 
 * Hydra_Software_Devel/2   1/11/12 3:44p farshidf
 * SW7552-176: fix the header file
 * 
 * Hydra_Software_Devel/1   12/8/11 11:28p farshidf
 * SW7552-170: first version of OOB tuner for B0 verification
 * 
 ***************************************************************************/
#ifndef BTNR_OB_3x7x_H__
#define BTNR_OB_3x7x_H__

#include "bchp.h"
#include "btnr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    unsigned short i2cAddr;             /* 7bit I2C address of Bcm7550 */
	BTMR_Handle hTmr;
	BMEM_Heap_Handle hHeap;
} BTNR_Ob_3x7x_Settings;

/***************************************************************************
Summary:
    This function returns the default settings for Bcm3x7x Tuner module.

Description:
    This function is responsible for returns the default setting for 
    BTNR module. The returning default setting should be when
    opening the device.

Returns:
    TODO:

See Also:
    BTNR_Ob_3x7x_Open()

****************************************************************************/
BERR_Code BTNR_Ob_3x7x_GetDefaultSettings(
    BTNR_Ob_3x7x_Settings *pDefSettings  /* [out] Returns default setting */
    );


/***************************************************************************
Summary:
    This function opens Bcm3x7x Tuner module.

Description:
    This function is responsible for opening Bcm3x7x BTNR module. When BTNR is
    opened, it will create a module handle and configure the module based
    on the default settings. Once the device is opened, it must be closed
    before it can be opened again.

Returns:
    TODO:

See Also:
    BTNR_Ob_Close(), BTNR_Ob_3x7x_GetDefaultSettings()

****************************************************************************/

BERR_Code BTNR_Ob_3x7x_Open(BTNR_Handle *phDev, 
		BTNR_Ob_3x7x_Settings *pSettings, 
		BREG_Handle hRegister);


/***************************************************************************
Summary:
    Function called once the event is sent to upper layer

	BTNR_Ob_3x7x_ProcessInterruptEvent
****************************************************************************/
BERR_Code BTNR_Ob_3x7x_ProcessInterruptEvent(BTNR_Handle hDev);

/***************************************************************************
Summary:
    Function called by upper to get the inetrrupt handle

	BTNR_Ob_3x7x_GetInterruptEventHandle
****************************************************************************/
BERR_Code BTNR_Ob_3x7x_GetInterruptEventHandle(BTNR_Handle h, BKNI_EventHandle* hEvent);


/***************************************************************************
Summary:
	BTNR_Ob_3x7x_P_TimerFunc
****************************************************************************/
BERR_Code BTNR_Ob_3x7x_P_TimerFunc(void *myParam1, int myParam2);


typedef struct
{
	int32_t		            Total_Mix_After_ADC;    /*Sum of mixer frequencies after ADC on eRequestMode*/
	int16_t		            PreADC_Gain_x256db ;    /*Gain in db*256 before ADC on eRequestMode: set to 0x8000 if unknown*/
	int16_t		            PostADC_Gain_x256db;    /*Gain in db*256 after ADC on eRequestMode: set to 0x8000 if unknown*/
	int16_t		            External_Gain_x256db;   /*Gain in db*256 external to chip (like external LNA) on eRequestMode: set to 0x8000 if unknown*/
} BTNR_Ob_3x7x_RfStatus_t;

/******************************************************************************
  BTNR_3x7x_Ob_Get_RF_Status()
  callback from Demod to Tuner through above layer
 ******************************************************************************/
BERR_Code BTNR_Ob_3x7x_Get_RF_Status(BTNR_Handle hTnrDev, BTNR_Ob_3x7x_RfStatus_t *RfCallbackStatus);



#ifdef __cplusplus
}
#endif
 
#endif



