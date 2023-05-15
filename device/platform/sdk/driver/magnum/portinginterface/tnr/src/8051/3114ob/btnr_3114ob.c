/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3114ob.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 7/9/10 10:25a $
 *
 * Module Description: $
 *
 * Revision History: $
 *
 * $brcm_Log: /magnum/portinginterface/tnr/3114ob/btnr_3114ob.c $
 * 
 * Hydra_Software_Devel/5   7/9/10 10:25a VISHK
 * SW7420-839: Add support to program AGC gain to 31xx TNR PI
 * 
 * Hydra_Software_Devel/4   8/25/09 3:07p vishk
 * SW7405-2841: Bcm31xx: Add Power Management support for TNR
 * 
 * Hydra_Software_Devel/3   8/25/09 2:44p vishk
 * SW7405-2841: Bcm31xx: Add Power Management support for TNR
 * 
 * Hydra_Software_Devel/2   6/29/09 1:30p vishk
 * PR 56235: Add support to set IfFreq in AOB PI for 3117
 * 
 * Hydra_Software_Devel/1   3/11/09 4:21p vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 *  
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "btnr_priv.h"
#include "btnr_3114ob.h"
#include "btnr_31xxob_priv.h"

BDBG_MODULE(btnr_3114ob);

#define	DEV_MAGIC_ID					((BERR_TNR_ID<<16) | 0xFACE)
#define	BTNR_31XXOb_SETTINGS_I2CADDR	(0x66)			/* 7bit addr */
#define	BTNR_31XXOb_IF_FREQ	            44000000			/* 7bit addr */

/*******************************************************************************
*
*	Public Module Functions
*
*******************************************************************************/

BERR_Code BTNR_3114Ob_Open(
	BTNR_Handle *phDev,					/* [output] Returns handle */
	BTNR_3114Ob_Settings *pSettings, /* [Input] settings structure */
	BHAB_Handle hHab                  /* [Input] Hab Handle */   
	)
{
	BERR_Code retCode = BERR_SUCCESS;
 	BTNR_31xxOb_Handle h3114ObDev;
	BTNR_P_31xxOb_Settings *pTnrImplData;
	BTNR_Handle hDev;


	BDBG_ENTER(BTNR_3114Ob_Open);
	BDBG_ASSERT( hHab );
	
	hDev = NULL;
	/* Alloc memory from the system heap */
	h3114ObDev = (BTNR_31xxOb_Handle) BKNI_Malloc( sizeof( BTNR_P_31xxOb_Handle ) );
	if( h3114ObDev == NULL )
	{
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(("BTNR_3114_Open: BKNI_malloc() failed\n"));
		goto done;
	}
	BKNI_Memset( h3114ObDev, 0x00, sizeof( BTNR_P_31xxOb_Handle ) );

	hDev = (BTNR_Handle) BKNI_Malloc( sizeof( BTNR_P_Handle ) );
	if( hDev == NULL )
	{
		retCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		BDBG_ERR(("BTNR_3114Ob_Open: BKNI_malloc() failed\n"));
		BKNI_Free( h3114ObDev );
		goto done;
	}
	BKNI_Memset( hDev, 0x00, sizeof( BTNR_P_Handle ) );

	h3114ObDev->magicId = DEV_MAGIC_ID;
	pTnrImplData = &h3114ObDev->settings;
	pTnrImplData->rfFreq = 0;
	pTnrImplData->tunerMode = BTNR_TunerMode_eDigital;
	pTnrImplData->ifFreq = pSettings->ifFreq;

	h3114ObDev->hHab = hHab;
	hDev->hDevImpl = (void *) h3114ObDev;
	hDev->magicId = DEV_MAGIC_ID;
	hDev->pSetRfFreq = (BTNR_SetRfFreqFunc) BTNR_31xxOb_SetRfFreq;
	hDev->pGetRfFreq = (BTNR_GetRfFreqFunc) BTNR_31xxOb_GetRfFreq;
	hDev->pGetAgcRegVal = (BTNR_GetAgcRegValFunc) BTNR_P_31xxOb_GetAgcRegVal;
	hDev->pSetAgcRegVal = (BTNR_SetAgcRegValFunc) BTNR_31xxOb_SetAgcRegVal;
	hDev->pGetInfo = (BTNR_GetInfoFunc) BTNR_31xxOb_GetInfo;
	hDev->pClose = (BTNR_CloseFunc) BTNR_31xxOb_Close;
	hDev->pGetPowerSaver = (BTNR_GetPowerSaverFunc) BTNR_31xxOb_GetPowerSaver;
	hDev->pSetPowerSaver = (BTNR_SetPowerSaverFunc) BTNR_31xxOb_SetPowerSaver;


done:
	*phDev = hDev;
	BDBG_LEAVE(BTNR_3114Ob_Open);
	return( retCode );
}

BERR_Code BTNR_3114Ob_GetDefaultSettings(
	BTNR_3114Ob_Settings *pDefSettings	/* [out] Returns default setting */
	)
{
	BDBG_ASSERT(NULL != pDefSettings);

    BKNI_Memset(pDefSettings, 0, sizeof(*pDefSettings));
	pDefSettings->i2cAddr = BTNR_31XXOb_SETTINGS_I2CADDR;
	pDefSettings->ifFreq = BTNR_31XXOb_IF_FREQ;

	return BERR_SUCCESS;
}

