/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: blna_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 3/26/12 5:15p $
 *
 * Module Description:
 * This module is the driver for LNA.
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/lna/blna_priv.h $
 * 
 * Hydra_Software_Devel/7   3/26/12 5:15p agin
 * SWNOOS-524:  Add Setting of LNA boost mode.
 * 
 * Hydra_Software_Devel/6   9/30/11 3:31p atanugul
 * SW7425-1208: revert to version 4
 * 
 * Hydra_Software_Devel/5   9/23/11 10:25a atanugul
 * SW7425-1208: Add BLNA_ReadRegister() and BLNA_WriteRegister() API
 * 
 * Hydra_Software_Devel/4   11/29/10 11:11a agin
 * SW7405-5021:  Add support for loopthrough enable/disable.
 * 
 * Hydra_Software_Devel/3   11/24/10 8:41p agin
 * SW7405-5021:  Add loop through feature.
 * 
 * Hydra_Software_Devel/2   11/22/10 12:36p mward
 * SW7125-729: Enable/Disable Power Saver Functions for maximum power-
 * down.
 * 
 * Hydra_Software_Devel/SW7125-729/1   11/10/10 5:49p mward
 * SW7125-729: Enable/Disable Power Saver Functions for maximum power-
 * down.
 * 
 * Hydra_Software_Devel/1   10/24/06 2:04p dlwin
 * PR24087: Support BCM3431 lna in reference design.
 * 
 * Refsw_Devel_3560/1   9/6/06 8:59p jerryy
 * PR24087: Support BCM3431 lna in reference design.
 * 
 ***************************************************************************/
#ifndef BLNA_PRIV_H__
#define BLNA_PRIV_H__


#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************
Summary:
	This function enable the automatic gain control feature of LNA.

Description:
	This function is responsible for enabling the automatic gain control
	feature of LNA.  When enabling this feature, the manual gain control
	feature will be disabled.
	
Returns:
	TODO:

See Also:
	BLNA_EnableManualGainControl(), BLNA_GetStatus()

****************************************************************************/
typedef BERR_Code (*BLNA_EnableAutomaticGainControlFunc)(
	void *hDevImp,					/* [in] Device handle */
	unsigned int agcOutputLevel,		/* [in] AGC Output Level, in 2/10 dB */
	int agcDeltaVal						/* [in] Delta value, specifics window, in 2/10 dB */
	);
typedef BERR_Code (*BLNA_EnableManualGainControlFunc)(
	void *hDevImp,					/* [in] Device handle */
	unsigned int gain					/* [in] Gain, in 2/10 dB */
	);

/***************************************************************************
Summary:
	This function sets the InBand port mode.

Description:
	This function is responsible for setting the InBand port's mode
	of operation.
	
Returns:
	TODO:

See Also:
	BLNA_GetStatus()

****************************************************************************/
typedef BERR_Code (*BLNA_SetInBandModeFunc)(
	void *hDevImp,					/* [in] Device handle */
	unsigned int portNbr,				/* [in] Port number, 0-2 */
	BLNA_InBandMode mode				/* [in] InBand mode */
	);

/***************************************************************************
Summary:
	This function sets the OutOfBand port mode.

Description:
	This function is responsible for setting the OutOfBand port's mode
	of operation.
	
Returns:
	TODO:

See Also:
	BLNA_GetStatus()

****************************************************************************/
typedef BERR_Code (*BLNA_SetOutOfBandModeFunc)(
	void *hDevImp,					/* [in] Device handle */
	unsigned int portNbr,				/* [in] Port number, 0-1 */
	BLNA_OutOfBandMode mode				/* [in] OutOfBand mode */
	);

/***************************************************************************
Summary:
	This function enables the external driver.

Description:
	This function is responsible for enabling the external driver on LNA.
	
Returns:
	TODO:

See Also:
	BLNA_GetStatus()

****************************************************************************/
typedef BERR_Code (*BLNA_EnableExternalDriverFunc)(
	void *hDevImp,					/* [in] Device handle */
	unsigned int extNbr					/* [in] Extneral driver */
	);

/***************************************************************************
Summary:
	This function disables the external driver.

Description:
	This function is responsible for disabling the external driver on LNA.
	
Returns:
	TODO:

See Also:
	BLNA_GetStatus()

****************************************************************************/
typedef BERR_Code (*BLNA_DisableExternalDriverFunc)(
	void *hDevImp,					/* [in] Device handle */
	unsigned int extNbr					/* [in] Extneral driver */
	);

/***************************************************************************
Summary:
	This function gets the status of LNA.

Description:
	This function is responsible for getting the complete status for
	LNA module.
	
Returns:
	TODO:

See Also:

****************************************************************************/
typedef BERR_Code (*BLNA_GetStatusFunc)(
	void *hDevImp,					/* [in] Device handle */
	BLNA_Status *pStatus				/* [out] Returns status */
	);

/***************************************************************************
Summary:
	This function reads the LNA's AGC value.

Description:
	This semi-private function is responsible for reading the
	LNA's AGC register value.  This function is specially written for
	Docsis use.
	
Returns:
	TODO:

See Also:

****************************************************************************/
typedef BERR_Code (*BLNA_GetLnaAgcRegValFunc)(
	void *hDevImp,					/* [in] Device handle */
	uint32_t regOffset,					/* [in] AGC register offset */
	uint32_t *agcVal					/* [out] output value */
	);

/***************************************************************************
Summary:
	This function powers down the LNA.

Description:
	This function is responsible for putting the LNA into lowest-power mode.
	
Returns:
	TODO:

See Also:
	BLNA_DisableExternalDriverFunc()
    BLNA_SetInBandModeFunc()
    BLNA_SetOutOfBandModeFunc()
****************************************************************************/
typedef BERR_Code (*BLNA_EnablePowerSaverFunc)(
	void *hDevImp					/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This function powers up the LNA.

Description:
	This function is responsible for taking the LNA out of lowest-power mode.
	
Returns:
	TODO:

See Also:
	BLNA_EnableExternalDriverFunc()
    BLNA_SetInBandModeFunc()
    BLNA_SetOutOfBandModeFunc()
****************************************************************************/
typedef BERR_Code (*BLNA_DisablePowerSaverFunc)(
	void *hDevImp					/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This enables loop through in the LNA.

Description:
	
Returns:
	TODO:

See Also:
	BLNA_DisableExternalDriverFunc()
    BLNA_SetInBandModeFunc()
    BLNA_SetOutOfBandModeFunc()
****************************************************************************/
typedef BERR_Code (*BLNA_EnableLoopThroughFunc)(
	void *hDevImp					/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This function disables loop through in the LNA.

Description:
	
Returns:
	TODO:

See Also:
	BLNA_EnableExternalDriverFunc()
    BLNA_SetInBandModeFunc()
    BLNA_SetOutOfBandModeFunc()
****************************************************************************/
typedef BERR_Code (*BLNA_DisableLoopThroughFunc)(
	void *hDevImp					/* [in] Device handle */
	);

/***************************************************************************
Summary:
	This function sets the boost mode of the LNA.

Description:
	
Returns:
	TODO:

See Also:
****************************************************************************/
typedef BERR_Code (*BLNA_SetBoostModeFunc)(
	void *hDevImp,					/* [in] Device handle */
	BLNA_BoostMode boostMode
	);

typedef BERR_Code (*BLNA_CloseFunc)(
	BLNA_Handle hLna					/* [in] Device handle */
	);


/*******************************************************************************
*
*	Private Module Handles
*
*******************************************************************************/
typedef struct BLNA_P_Handle
{
	void *hDevImpl;						/* Device Implementation handle */
	uint32_t magicId;					/* Used to check if structure is corrupt */
	BLNA_EnableAutomaticGainControlFunc pEnableAutomaticGainControl; 		/* ptr to get tuned frequency function */
	BLNA_EnableManualGainControlFunc pEnableManualGainControl;
	BLNA_SetInBandModeFunc pSetInBandMode;
	BLNA_SetOutOfBandModeFunc pSetOutOfBandMode;
	BLNA_EnableExternalDriverFunc pEnableExternalDriver;
	BLNA_DisableExternalDriverFunc pDisableExternalDriver;
	BLNA_GetStatusFunc pGetStatus;
	BLNA_GetLnaAgcRegValFunc pGetLnaAgcRegVal;
	BLNA_CloseFunc pClose; 		/* ptr to tune tuner function */
    BLNA_EnablePowerSaverFunc pEnablePowerSaver;
    BLNA_DisablePowerSaverFunc pDisablePowerSaver;
    BLNA_EnableLoopThroughFunc pEnableLoopThrough;
    BLNA_DisableLoopThroughFunc pDisableLoopThrough;
    BLNA_SetBoostModeFunc pSetBoostMode;
} BLNA_P_Handle;



#ifdef __cplusplus
}
#endif
 
#endif



