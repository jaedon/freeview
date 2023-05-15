/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3418.c $
 * $brcm_Revision: Hydra_Software_Devel/28 $
 * $brcm_Date: 6/22/10 6:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/3418/btnr_3418.c $
 * 
 * Hydra_Software_Devel/28   6/22/10 6:12p vishk
 * SW7420-839: Add support to program AGC gain to 31xx TNR PI
 * 
 * Hydra_Software_Devel/27   8/25/09 9:42a vishk
 * SW7405-2841: Bcm31xx: Add Power Management support for TNR
 * 
 * Hydra_Software_Devel/26   8/24/09 5:58p vishk
 * SW7405-2841: Bcm31xx: Add Power Management support for TNR
 * 
 * Hydra_Software_Devel/25   9/20/07 3:52p vishk
 * PR 35027: TNR-Coverity (CID 3102): USE_AFTER_FREE
 * 
 * Hydra_Software_Devel/24   7/24/07 11:32a vishk
 * PR 32983: To port tuner PI to use fixed point implementation
 * 
 * Hydra_Software_Devel/23   2/2/07 5:01p dlwin
 * PR 27583: Fixed a compiler error (typo).
 * 
 * Hydra_Software_Devel/22   2/2/07 1:58p dlwin
 * PR 27583: 3418 Tuner: Reduce tuning time by using shadow registers
 * 
 * Hydra_Software_Devel/customer_OCAP_SUPPORT/3   2/2/07 1:12p ayxiang
 * modification for robustness
 * 
 * Hydra_Software_Devel/customer_OCAP_SUPPORT/2   2/1/07 4:38p ayxiang
 * tuner 3418 optimization, put shadow into 3418 handle
 * 
 * Hydra_Software_Devel/customer_OCAP_SUPPORT/1   1/25/07 5:41p ayxiang
 * customer optimize tuning speed
 * 
 * Hydra_Software_Devel/21   6/7/05 8:36a dlwin
 * PR 14682: Added additional functions for Docsis use.  Also added
 * function to return additional information about the tuner.
 * 
 * Hydra_Software_Devel/20   4/5/05 2:23p dlwin
 * PR 14696: Fixed warning for unused variables
 * 
 * Hydra_Software_Devel/19   4/1/05 5:26p dlwin
 * PR 14682: Added semi-private functions for QamCm to use to AGC
 * readings.  This required for Docsis integration.
 * 
 * Hydra_Software_Devel/18   1/28/04 7:56a dlwin
 * PR 9500: Inserted BERR_TRACE() marco around error code.
 * 
 * Hydra_Software_Devel/17   1/22/04 4:57p dlwin
 * PR 9448: Fixed the problem of not tuning to frequencies below one MHz.
 * 
 * Hydra_Software_Devel/16   12/30/03 3:10p dlwin
 * PR 9155: Added support for configuring the Xtal Clock Freq. for Bcm3418
 * tuner.
 * 
 * Hydra_Software_Devel/15   12/12/03 3:15p dlwin
 * PR 8973: Return error number when BKNI_Malloc() fails,  instead of
 * asserting.
 * 
 * Hydra_Software_Devel/14   12/2/03 9:26a dlwin
 * Removed check for hChip handle.
 * 
 * Hydra_Software_Devel/13   11/3/03 5:36p dlwin
 * Fixed problem discovered by Vicent, regarding the setting for
 * Analog/Digital settings.
 * 
 * Hydra_Software_Devel/12   10/29/03 8:21p dlwin
 * Updated to latest Bcm3418 tuner code from H/W folks.
 * 
 * Hydra_Software_Devel/11   10/21/03 6:19p dlwin
 * Removed all BDBG_ENTER(__FUNCTION__) and BDBG_LEAVE(__FUNCTION__) with
 * macro usage of function, instead of __FUNCTION__.
 * 
 * Hydra_Software_Devel/10   10/20/03 8:53p dlwin
 * Initial memory from BKNI_Malloc() to zero.
 * 
 * Hydra_Software_Devel/9   10/8/03 11:18a dlwin
 * Converted I2C address to use 7bit instead of 8bit.
 * 
 * Hydra_Software_Devel/8   10/7/03 8:53a dlwin
 * Removed the need to call a create generic tuner  handle.
 * 
 * Hydra_Software_Devel/7   10/6/03 3:59p dlwin
 * Updated to latest 3418 port from H/W.
 * 
 * Hydra_Software_Devel/6   9/18/03 2:41p dlwin
 * Removed an warning from '-Wall'.
 * 
 * Hydra_Software_Devel/5   9/14/03 2:22p dlwin
 * Replaced TRUE with true and FALSE with flase
 * 
 * Hydra_Software_Devel/4   9/9/03 8:44a dlwin
 * Updated API function parameter comment for DocJet.
 * 
 * Hydra_Software_Devel/3   9/3/03 2:16p dlwin
 * Added to use default values from include file.
 * 
 * Hydra_Software_Devel/2   8/29/03 7:10a dlwin
 * Added debug code, clearing out 'magicId' when the device is closed.
 * 
 * Hydra_Software_Devel/1   8/26/03 2:20p dlwin
 * Initial version
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "btnr_priv.h"
#include "btnr_3418.h"
#include "btnr_3418_smarttune.h"
#include "btnr_3418_regio.h"

BDBG_MODULE(btnr_3418);

#define	DEV_MAGIC_ID			((BERR_TNR_ID<<16) | 0xFACE)

#define	CHK_RETCODE( rc, func )			\
do {										\
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										\
		goto done;							\
	}										\
} while(0)


/*******************************************************************************
*
*	Private Module Handles
*
*******************************************************************************/

typedef struct BTNR_P_3418_Handle		*BTNR_3418_Handle;

typedef struct BTNR_P_3418_Handle
{
	uint32_t magicId;					/* Used to check if structure is corrupt */

	BTNR_P_3418_Settings settings;
} BTNR_P_3418_Handle;



/*******************************************************************************
*
*	Default Module Settings
*
*******************************************************************************/
static const BTNR_3418_Settings defDevSettings =
{
	BTNR_3418_SETTINGS_IFFREQ,
	BTNR_3418_SETTINGS_XTALFREQ,
	BTNR_3418_SETTINGS_I2CADDR,
	BTNR_3418_SETTINGS_ENABLEAGGAIN
};



/*******************************************************************************
*
*	Private Module Data
*
*******************************************************************************/
static BERR_Code BTNR_3418_Close(
	BTNR_Handle hDev					/* [in] Device handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BTNR_3418_Handle btnr_3418_handle;
	BTNR_P_3418_Settings btnr_3418_settings;


	BDBG_ENTER(BTNR_3418_Close);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	/* verify the handle is good before using it */
	btnr_3418_handle = (BTNR_3418_Handle) hDev->hDevImpl;
	btnr_3418_settings = btnr_3418_handle ->settings;

	hDev->magicId = 0x00;		/* clear it to catch inproper use */
	BKNI_Free((void *) btnr_3418_settings.register_shadow);
	BKNI_Free( (void *) hDev->hDevImpl );
	BKNI_Free( (void *) hDev );

	BDBG_LEAVE(BTNR_3418_Close);
	return( retCode );
}

static BERR_Code BTNR_3418_SetRfFreq(
	BTNR_3418_Handle hDev,				/* [in] Device handle */
	uint32_t rfFreq,					/* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode			/* [in] Requested tuner mode */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BTNR_P_3418_Settings *pTnrImplData;

	BDBG_ENTER(BTNR_3418_SetRfFreq);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	
	pTnrImplData = &hDev->settings;
	if( pTnrImplData->isInitialized == false )
	{
		/* If the Bcm3418 has never initialized, initialize the H/W */
		BTNR_P_3418_initialize( pTnrImplData );
	}

	if( tunerMode == BTNR_TunerMode_eDigital)
		BTNR_P_3418_initDigital( pTnrImplData );
	else if( tunerMode == BTNR_TunerMode_eAnalog)
		BTNR_P_3418_initAnalog( pTnrImplData );
	
	BTNR_P_3418_tune( pTnrImplData, rfFreq );

	pTnrImplData->rfFreq = rfFreq;
	pTnrImplData->tunerMode = tunerMode;

	BDBG_LEAVE(BTNR_3418_SetRfFreq);
	return( retCode );
}

static BERR_Code BTNR_3418_GetRfFreq(
	BTNR_3418_Handle hDev,				/* [in] Device handle */
	uint32_t *rfFreq,					/* [output] Returns tuner freq., in Hertz */
	BTNR_TunerMode *tunerMode			/* [output] Returns tuner mode */
    )
{
	BERR_Code retCode = BERR_SUCCESS;
	BTNR_P_3418_Settings *pTnrImplData;


	BDBG_ENTER(BTNR_3418_GetRfFreq);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	pTnrImplData = &hDev->settings;

	*rfFreq = pTnrImplData->rfFreq;
	*tunerMode = pTnrImplData->tunerMode;

	BDBG_LEAVE(BTNR_3418_GetRfFreq);
	return( retCode );
}

static BERR_Code BTNR_3418_GetAgcRegVal(
	BTNR_3418_Handle hDev,				/* [in] Device handle */
	uint32_t regOffset,					/* [in] AGC register offset */
	uint32_t *agcVal					/* [out] output value */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
	BTNR_P_3418_Settings *pTnrImplData;
	uint8_t bVal;


	BDBG_ENTER(BTNR_3418_GetAgcRegVal);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	pTnrImplData = &hDev->settings;

	BTNR_P_3418_ReadReg( pTnrImplData, regOffset, &bVal );
	*agcVal = bVal;

	BDBG_LEAVE(BTNR_3418_GetAgcRegVal);
	return( retCode );
}

static BERR_Code BTNR_3418_GetInfo(
	BTNR_3418_Handle hDev,				/* [in] Device handle */
	BTNR_TunerInfo *tnrInfo				/* [out] Tuner information */
	)
{	
	BTNR_P_3418_Settings *pTnrImplData;
	BERR_Code retCode = BERR_SUCCESS;

	BDBG_ENTER(BTNR_3418_GetInfo);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	pTnrImplData = &hDev->settings;
	tnrInfo->tunerMaker = BRCM_TUNER_MAKER_ID;
	tnrInfo->tunerId = 3418;
	tnrInfo->tunerMajorVer = pTnrImplData->iRevLetter;
	tnrInfo->tunerMinorVer = pTnrImplData->iRevNumber;

	BDBG_LEAVE(BTNR_3418_GetInfo);
	return( retCode );
}

/*******************************************************************************
*
*	Public Module Functions
*
*******************************************************************************/

BERR_Code BTNR_3418_Open(
	BTNR_Handle *phDev,					/* [output] Returns handle */
	BCHP_Handle hChip,					/* [in] Chip handle */
	BREG_I2C_Handle hI2CReg,			/* [in] I2C Register handle */
	const BTNR_3418_Settings *pDefSettings /* [in] Default settings */
	)
{
	BERR_Code retCode = BERR_SUCCESS;
 	BTNR_3418_Handle h3418Dev;
	BTNR_P_3418_Settings *pTnrImplData;
	BTNR_Handle hDev;
	unsigned char * pShadow;


	BDBG_ENTER(BTNR_3418_Open);
	BDBG_ASSERT( hI2CReg );
	BSTD_UNUSED(hChip);

	hDev = NULL;
	/* Alloc memory from the system heap */
	h3418Dev = (BTNR_3418_Handle) BKNI_Malloc( sizeof( BTNR_P_3418_Handle ) );
	if( h3418Dev == NULL )
	{
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(("BTNR_3418_Open: BKNI_malloc() failed\n"));
		*phDev = NULL;
		goto done;
	}
	BKNI_Memset( h3418Dev, 0x00, sizeof( BTNR_P_3418_Handle ) );

	hDev = (BTNR_Handle) BKNI_Malloc( sizeof( BTNR_P_Handle ) );
	if( hDev == NULL )
	{
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(("BTNR_3418_Open: BKNI_malloc() failed\n"));
		BKNI_Free( h3418Dev );
		*phDev = NULL;
		goto done;
	}
	BKNI_Memset( hDev, 0x00, sizeof( BTNR_P_Handle ) );

	pShadow = (unsigned char *) BKNI_Malloc( (sizeof(unsigned char) * BTNR_P_3418_GetRegNumber()) );
	if( pShadow == NULL )
	{
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(("BTNR_3418_Open: BKNI_malloc() failed\n"));
		BKNI_Free( h3418Dev );
		BKNI_Free( hDev );
		*phDev = NULL;
		goto done;
	}
	BKNI_Memset( pShadow, 0x00, (sizeof(unsigned char) * BTNR_P_3418_GetRegNumber()));

	h3418Dev->magicId = DEV_MAGIC_ID;
	pTnrImplData = &h3418Dev->settings;
	pTnrImplData->register_shadow = pShadow;
	pTnrImplData->hI2CReg = hI2CReg;
	pTnrImplData->ifFreq = pDefSettings->ifFreq;
	pTnrImplData->xtalFreq = pDefSettings->xtalFreq;
	pTnrImplData->i2cAddr = pDefSettings->i2cAddr;
	if( (pDefSettings->i2cAddr & 0x80) )
	{
		BDBG_ERR(("I2C Address must be 7 bit format"));
		BDBG_ASSERT( !(pDefSettings->i2cAddr & 0x80) );
	}
	pTnrImplData->enableAgcGain = pDefSettings->enableAgcGain;

	pTnrImplData->rfFreq = 0;
	pTnrImplData->tunerMode = BTNR_TunerMode_eDigital;
	pTnrImplData->isInitialized = false;

	hDev->hDevImpl = (void *) h3418Dev;
	hDev->magicId = DEV_MAGIC_ID;
	hDev->pSetRfFreq = (BTNR_SetRfFreqFunc) BTNR_3418_SetRfFreq;
	hDev->pGetRfFreq = (BTNR_GetRfFreqFunc) BTNR_3418_GetRfFreq;
	hDev->pGetAgcRegVal = (BTNR_GetAgcRegValFunc) BTNR_3418_GetAgcRegVal;
	hDev->pSetAgcRegVal = (BTNR_SetAgcRegValFunc) NULL;
	hDev->pGetInfo = (BTNR_GetInfoFunc) BTNR_3418_GetInfo;
	hDev->pClose = (BTNR_CloseFunc) BTNR_3418_Close;
	hDev->pGetPowerSaver = (BTNR_GetPowerSaverFunc)NULL;
	hDev->pSetPowerSaver = (BTNR_SetPowerSaverFunc)NULL;
	
	*phDev = hDev;
	
done:
	BDBG_LEAVE(BTNR_3418_Open);
	return( retCode );
}

BERR_Code BTNR_3418_GetDefaultSettings(
	BTNR_3418_Settings *pDefSettings,	/* [output] Returns default setting */
	BCHP_Handle hChip					/* [in] Chip handle */
	)
{
	BERR_Code retCode = BERR_SUCCESS;

	
	BDBG_ENTER(BTNR_3418_GetDefaultSettings);
	BSTD_UNUSED(hChip);

	*pDefSettings = defDevSettings;

	BDBG_LEAVE(BTNR_3418_GetDefaultSettings);
	return( retCode );
}

/* Private functions for Docsis use: Do not for general use, therefore not documented */
void BTNR_3418_SetIf1AgcForceMode(
	BTNR_Handle hDev					/* [in] Device handle */
	)
{
	BTNR_P_3418_Settings *pTnrImplData;
	BTNR_3418_Handle h3418Dev;

	BDBG_ENTER(BTNR_3418_SetIf1AgcForceMode);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	h3418Dev = hDev->hDevImpl;
	pTnrImplData = &h3418Dev->settings;
	if( pTnrImplData->iRevLetter == 1 ) /* Only valid for 3418 B0 */
	{
		BTNR_P_3418_Freeze( pTnrImplData );
		BKNI_Sleep(100);
	}
	BDBG_LEAVE(BTNR_3418_SetIf1AgcForceMode);
}

/* Private functions for Docsis use: Do not for general use, therefore not documented */
void BTNR_3418_SetIf1AgcAutoMode(
	BTNR_Handle hDev					/* [in] Device handle */
	)
{
	BTNR_P_3418_Settings *pTnrImplData;
	BTNR_3418_Handle h3418Dev;

	BDBG_ENTER(BTNR_3418_SetIf1AgcAutoMode);
	BDBG_ASSERT( hDev );
	BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

	h3418Dev = hDev->hDevImpl;
	pTnrImplData = &h3418Dev->settings;
	if( pTnrImplData->iRevLetter == 1 ) /* Only valid for 3418 B0 */
	{
  		/* Set Auto mode */
        BTNR_P_3418_UnFreeze( pTnrImplData ); 
	}
	BDBG_LEAVE(BTNR_3418_SetIf1AgcAutoMode);
}

