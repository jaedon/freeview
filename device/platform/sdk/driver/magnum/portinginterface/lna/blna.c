/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: blna.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 3/26/12 5:15p $
 *
 * Module Description:
 * This module is the driver for LNA.
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/lna/blna.c $
 * 
 * Hydra_Software_Devel/8   3/26/12 5:15p agin
 * SWNOOS-524:  Add Setting of LNA boost mode.
 * 
 * Hydra_Software_Devel/7   9/30/11 3:29p atanugul
 * SW7425-1208: revert to version 5
 * 
 * Hydra_Software_Devel/6   9/23/11 10:25a atanugul
 * SW7425-1208: Add BLNA_ReadRegister() and BLNA_WriteRegister() API
 * 
 * Hydra_Software_Devel/5   3/11/11 3:54p mward
 * SW7125-828:  Default error code to BERR_NOT_SUPPORTED, so that
 * unimplemented functions with NULL pointers will indicate properly.
 * 
 * Hydra_Software_Devel/SW7125-828/1   2/14/11 12:04p mward
 * SW7125-828:  Default error code to BERR_NOT_SUPPORTED, so that
 * unimplemented functions with NULL pointers will indicate properly.
 * 
 * Hydra_Software_Devel/4   11/24/10 8:40p agin
 * SW7405-5021:  Add loop through feature.
 * 
 * Hydra_Software_Devel/3   11/22/10 12:35p mward
 * SW7125-729: Enable/Disable Power Saver Functions for maximum power-
 * down.
 * 
 * Hydra_Software_Devel/SW7125-729/1   11/10/10 5:48p mward
 * SW7125-729: Enable/Disable Power Saver Functions for maximum power-
 * down.
 * 
 * Hydra_Software_Devel/2   12/4/06 10:28a erickson
 * PR24374: fix warning
 * 
 * Hydra_Software_Devel/1   10/24/06 2:01p dlwin
 * PR24087: Support BCM3431 lna in reference design.
 * 
 * Refsw_Devel_3560/1   9/6/06 8:59p jerryy
 * PR24087: Support BCM3431 lna in reference design.
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "blna.h"
#include "blna_priv.h"

BDBG_MODULE(blna);

#define	CHK_RETCODE( rc, func )				\
	do {										\
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
{										\
	goto done;							\
}										\
	} while(0)


#define	DEV_MAGIC_ID			((BERR_LNA_ID<<16) | 0xFACE)


/*******************************************************************************
*
*	Private Module Handles
*
*******************************************************************************/



/*******************************************************************************
*
*	Default Module Settings
*
*******************************************************************************/



/*******************************************************************************
*
*	Private Module Data
*
*******************************************************************************/



/*******************************************************************************
*
*	Private Module Functions
*
*******************************************************************************/



/*******************************************************************************
*
*	Public Module Functions
*
*******************************************************************************/




BERR_Code BLNA_EnableAutomaticGainControl(
										  BLNA_Handle hDev,					/* [in] Device handle */
										  unsigned int agcOutputLevel,		/* [in] AGC Output Level, in 2/10 dB */
										  int agcDeltaVal						/* [in] Delta value, specifics window, in 2/10 dB */
										  )
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	
	BDBG_ENTER(BLNA_EnableAutomaticGainControl);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	
	if( hDev->pEnableAutomaticGainControl != NULL )
	{
		CHK_RETCODE( retCode, hDev->pEnableAutomaticGainControl( hDev->hDevImpl, agcOutputLevel, agcDeltaVal ) );
	}
	
done:
	BDBG_LEAVE(BLNA_EnableAutomaticGainControl);
	return( retCode );
}

BERR_Code BLNA_EnableManualGainControl(
									   BLNA_Handle hDev,					/* [in] Device handle */
									   unsigned int gain					/* [in] Gain, in 2/10 dB */
									   )
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	BDBG_ENTER(BLNA_EnableManualGainControl);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	
	if( hDev->pEnableManualGainControl != NULL )
	{
		CHK_RETCODE( retCode, hDev->pEnableManualGainControl( hDev->hDevImpl, gain ) );
	}
	
done:
	BDBG_LEAVE(BLNA_EnableManualGainControl);
	return( retCode );
}

BERR_Code BLNA_SetInBandMode(
							 BLNA_Handle hDev,					/* [in] Device handle */
							 unsigned int portNbr,				/* [in] Port number, 0-2 */
							 BLNA_InBandMode mode				/* [in] InBand mode */
							 )
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	BDBG_ENTER(BLNA_SetInBandMode);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	if( hDev->pSetInBandMode != NULL )
	{
		CHK_RETCODE( retCode, hDev->pSetInBandMode( hDev->hDevImpl, portNbr,  mode) );
	}
done:
	BDBG_LEAVE(BLNA_SetInBandMode);
	return( retCode );
	
}

BERR_Code BLNA_SetOutOfBandMode(
								BLNA_Handle hDev,					/* [in] Device handle */
								unsigned int portNbr,				/* [in] Port number, 0-1 */
								BLNA_OutOfBandMode mode				/* [in] OutOfBand mode */
								)
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	BDBG_ENTER(BLNA_SetOutOfBandMode);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	
	if( hDev->pSetOutOfBandMode != NULL )
	{
		CHK_RETCODE( retCode, hDev->pSetOutOfBandMode( hDev->hDevImpl, portNbr,  mode) );
	}
	
done:
	BDBG_LEAVE(BLNA_SetOutOfBandMode);
	return( retCode );
}

BERR_Code BLNA_EnableExternalDriver(
									BLNA_Handle hDev,					/* [in] Device handle */
									unsigned int extNbr					/* [in] Extneral driver */
									)
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	BDBG_ENTER(BLNA_EnableExternalDriver);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	if( hDev->pEnableExternalDriver != NULL )
	{
		CHK_RETCODE( retCode, hDev->pEnableExternalDriver( hDev->hDevImpl, extNbr) );
	}
	
done:
	BDBG_LEAVE(BLNA_EnableExternalDriver);
	return( retCode );
}

BERR_Code BLNA_DisableExternalDriver(
									 BLNA_Handle hDev,					/* [in] Device handle */
									 unsigned int extNbr					/* [in] Extneral driver */
									 )
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	BDBG_ENTER(BLNA_DisableExternalDriver);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	if( hDev->pDisableExternalDriver != NULL )
	{
		CHK_RETCODE( retCode, hDev->pDisableExternalDriver( hDev->hDevImpl, extNbr) );
	}
	
done:
	BDBG_LEAVE(BLNA_DisableExternalDriver);
	return( retCode );
}

BERR_Code BLNA_GetStatus(
						 BLNA_Handle hDev,					/* [in] Device handle */
						 BLNA_Status *pStatus				/* [out] Returns status */
						 )
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	BDBG_ENTER(BLNA_GetStatus);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	if( pStatus == NULL )
	{
		retCode = BERR_INVALID_PARAMETER;
		goto done;
	}
	if( hDev->pGetStatus != NULL )
	{
		CHK_RETCODE( retCode, hDev->pGetStatus( hDev->hDevImpl, pStatus) );
	}
done:
	BDBG_LEAVE(BLNA_GetStatus);
	return( retCode );
}

BERR_Code BLNA_GetLnaAgcRegVal(
							   BLNA_Handle hDev,					/* [in] Device handle */
							   uint32_t regOffset,					/* [in] AGC register offset */
							   uint32_t *agcVal					/* [out] output value */
							   )
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	
	BDBG_ENTER(BLNA_GetLnaAgcRegVal);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	if( agcVal == NULL )
	{
		retCode = BERR_INVALID_PARAMETER;
		goto done;
	}
	if( hDev->pGetLnaAgcRegVal != NULL )
	{
		CHK_RETCODE( retCode, hDev->pGetLnaAgcRegVal( hDev->hDevImpl, regOffset, agcVal) );
	}
done:
	BDBG_LEAVE(BLNA_GetLnaAgcRegVal);
	return( retCode );
}


BERR_Code BLNA_Close(
					 BLNA_Handle hDev					/* [in] Device handle */
					 )
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	
	
	BDBG_ENTER(BLNA_Close);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	
	if( hDev->pClose != NULL )
	{
		CHK_RETCODE( retCode, hDev->pClose( hDev ) );
	}
	
done:
	BDBG_LEAVE(BLNA_Close);
	return( retCode );
}

BERR_Code BLNA_EnablePowerSaver(
								BLNA_Handle hDev					/* [in] Device handle */
								)
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	BDBG_ENTER(BLNA_EnablePowerSaver);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	if( hDev->pEnablePowerSaver != NULL )
	{
		CHK_RETCODE( retCode, hDev->pEnablePowerSaver( hDev->hDevImpl ) );
	}
	
done:
	BDBG_LEAVE(BLNA_EnablePowerSaver);
	return( retCode );
}

BERR_Code BLNA_DisablePowerSaver(
								BLNA_Handle hDev					/* [in] Device handle */
								)
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	BDBG_ENTER(BLNA_DisablePowerSaver);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	if( hDev->pDisablePowerSaver != NULL )
	{
		CHK_RETCODE( retCode, hDev->pDisablePowerSaver( hDev->hDevImpl ) );
	}
	
done:
	BDBG_LEAVE(BLNA_DisablePowerSaver);
	return( retCode );
}

BERR_Code BLNA_EnableLoopThrough(
								BLNA_Handle hDev					/* [in] Device handle */
								)
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	BDBG_ENTER(BLNA_EnableLoopThrough);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	if( hDev->pEnableLoopThrough != NULL )
	{
		CHK_RETCODE( retCode, hDev->pEnableLoopThrough( hDev->hDevImpl ) );
	}
	
done:
	BDBG_LEAVE(BLNA_EnableLoopThrough);
	return( retCode );
}

BERR_Code BLNA_DisableLoopThrough(
								BLNA_Handle hDev					/* [in] Device handle */
								)
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	BDBG_ENTER(BLNA_DisableLoopThrough);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	if( hDev->pDisableLoopThrough != NULL )
	{
		CHK_RETCODE( retCode, hDev->pDisableLoopThrough( hDev->hDevImpl ) );
	}
	
done:
	BDBG_LEAVE(BLNA_DisableLoopThrough);
	return( retCode );
}

BERR_Code BLNA_SetBoostMode(
                            BLNA_Handle hDev,					/* [in] Device handle */
                            BLNA_BoostMode boostMode
						    )
{
	BERR_Code retCode = BERR_NOT_SUPPORTED;
	BDBG_ENTER(BLNA_SetBoostMode);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );
	if( hDev->pDisableLoopThrough != NULL )
	{
		CHK_RETCODE( retCode, hDev->pSetBoostMode( hDev->hDevImpl, boostMode ) );
	}
	
done:
	BDBG_LEAVE(BLNA_SetBoostMode);
	return( retCode );
}

