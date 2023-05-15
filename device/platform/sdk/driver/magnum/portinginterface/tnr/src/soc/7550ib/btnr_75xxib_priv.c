/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_75xxib_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/7/12 10:11a $
 *
 * Module Description:
 *
 * Revision History:  $
 *
 * $brcm_Log: /magnum/portinginterface/tnr/src/soc/7550ib/btnr_75xxib_priv.c $
 * 
 * Hydra_Software_Devel/1   9/7/12 10:11a farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 * structure
 * 
 * Hydra_Software_Devel/16   12/28/09 3:36p farshidf
 * SW7550-38: compile fix
 * 
 * Hydra_Software_Devel/15   12/28/09 2:46p farshidf
 * SW7550-38: be able to tune with BBS with Nexus
 * 
 * Hydra_Software_Devel/14   11/30/09 12:56p farshidf
 * SW7550-38: call the rigth free function
 * 
 * Hydra_Software_Devel/13   11/23/09 1:45p farshidf
 * SW7550-38: nexus compile fix
 * 
 * Hydra_Software_Devel/12   11/19/09 5:21p farshidf
 * SW7550-38:compile fix
 * 
 * Hydra_Software_Devel/11   11/19/09 4:27p farshidf
 * SW7550-38: check-in latest branch
 * 
 * Hydra_Software_Devel/THD_7550_40_6/1   11/18/09 9:29p jputnam
 * Functional for terrestrial
 * 
 * Hydra_Software_Devel/7   11/13/09 1:42p farshidf
 * SW7550-38: make Nexus compatible
 * 
 * Hydra_Software_Devel/6   11/13/09 11:28a farshidf
 * SW7550-38: update the BBS interface
 * 
 * Hydra_Software_Devel/5   11/12/09 10:04p farshidf
 * SW7550-38: BBS code changes required
 * 
 * Hydra_Software_Devel/4   11/12/09 8:42p farshidf
 * SW7550-38: bbs control function
 * 
 * Hydra_Software_Devel/3   11/12/09 6:57p farshidf
 * SW7550-40: update the tuner files
 * 
 * Hydra_Software_Devel/2   11/10/09 7:11p farshidf
 * 7550-38: add the tuner code
 * 
 * Hydra_Software_Devel/1   11/5/09 10:23a farshidf
 * SW7550-38: add the 7550 tuner code
 *  
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btnr.h"
#include "btnr_priv.h"
#include "btnr_75xxib_priv.h"

BDBG_MODULE(btnr_75xxib_priv);

#define DEV_MAGIC_ID                    ((BERR_TNR_ID<<16) | 0xFACE)

#define CHK_RETCODE( rc, func )         \
do {                                        \
    if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
    {                                       \
        goto done;                          \
    }                                       \
} while(0)

/*******************************************************************************
*
*   Private Module Data
*
*******************************************************************************/
BERR_Code BTNR_75xx_Close(
    BTNR_Handle hDev                    /* [in] Device handle */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_75xx_Handle btnr_75xx_handle;
    BTNR_P_75xx_Settings btnr_75xx_settings;
	BTNR_P_75xx_Handle *p7550;

    BDBG_ENTER(BTNR_75xx_Close);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    /* verify the handle is good before using it */
    btnr_75xx_handle = (BTNR_75xx_Handle) hDev->hDevImpl;
    btnr_75xx_settings = btnr_75xx_handle ->settings;
	p7550 = (BTNR_P_75xx_Handle *)(hDev->hDevImpl);

    hDev->magicId = 0x00;       /* clear it to catch inproper use */
	BTMR_DestroyTimer(p7550->hTimer);
	BKNI_DestroyEvent(p7550->hInterruptEvent);
	BMEM_Free( p7550->hHeap, p7550->pTnrModeData );
    BMEM_Free( p7550->hHeap, p7550->pTnrStatus );
    BKNI_Free( (void *) hDev->hDevImpl );
    BKNI_Free( (void *) hDev );



    BDBG_LEAVE(BTNR_75xx_Close);
    return( retCode );
}

BERR_Code BTNR_75xx_SetRfFreq(
    BTNR_75xx_Handle hDev,                /* [in] Device handle */
    uint32_t rfFreq,                    /* [in] Requested tuner freq., in Hertz */
    BTNR_TunerMode tunerMode            /* [in] Requested tuner mode */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
	BSTD_UNUSED(tunerMode);
	BSTD_UNUSED(rfFreq);
    BDBG_ENTER(BTNR_75xx_SetRfFreq);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BDBG_LEAVE(BTNR_75xx_SetRfFreq);
    return( retCode );
}
            
BERR_Code BTNR_75xx_GetRfFreq(
    BTNR_75xx_Handle hDev,                /* [in] Device handle */
    uint32_t *rfFreq,                   /* [output] Returns tuner freq., in Hertz */
    BTNR_TunerMode *tunerMode           /* [output] Returns tuner mode */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_P_75xx_Settings *pTnrImplData;


    BDBG_ENTER(BTNR_75xx_GetRfFreq);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    pTnrImplData = &hDev->settings;

    *rfFreq = pTnrImplData->rfFreq;
    *tunerMode = pTnrImplData->tunerMode;

    BDBG_LEAVE(BTNR_75xx_GetRfFreq);
    return( retCode );
}

BERR_Code BTNR_P_75xx_GetAgcRegVal(
    BTNR_75xx_Handle hDev,                /* [in] Device handle */
    uint32_t regOffset,                 /* [in] AGC register offset */
    uint32_t *agcVal                    /* [out] output value */
    )
{
    BERR_Code retCode = BERR_SUCCESS;
    BTNR_P_75xx_Settings *pTnrImplData;


    BDBG_ENTER(BTNR_P_75xx_GetAgcRegVal);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    BSTD_UNUSED(regOffset);

    pTnrImplData = &hDev->settings;

    BDBG_ERR(("Not Implemented on this tuner"));
    *agcVal = 0;

    BDBG_LEAVE(BTNR_P_75xx_GetAgcRegVal);
    return( retCode );
}

BERR_Code BTNR_75xx_GetInfo(
    BTNR_75xx_Handle hDev,                /* [in] Device handle */
    BTNR_TunerInfo *tnrInfo             /* [out] Tuner information */
    )
{   
    BTNR_P_75xx_Settings *pTnrImplData;
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_75xx_GetInfo);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    pTnrImplData = &hDev->settings;
    tnrInfo->tunerMaker = BRCM_TUNER_MAKER_ID;
    tnrInfo->tunerId = 0x7550;
    tnrInfo->tunerMajorVer = pTnrImplData->iRevLetter;
    tnrInfo->tunerMinorVer = pTnrImplData->iRevNumber;

    BDBG_LEAVE(BTNR_75xx_GetInfo);
    return( retCode );
}

BERR_Code BTNR_75xx_GetPowerSaver(
	BTNR_75xx_Handle hDev,					/* [in] Device handle */
	BTNR_PowerSaverSettings *pwrSettings 		/* [in] Power saver settings. */
	)
{   
    BTNR_P_75xx_Settings *pTnrImplData;
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_75xx_GetPowerSaver);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    pTnrImplData = &hDev->settings;

	pwrSettings->enable = pTnrImplData->powerSaver;
	
    BDBG_LEAVE(BTNR_75xx_GetPowerSaver);
    return( retCode );
}

BERR_Code BTNR_75xx_SetPowerSaver(
	BTNR_75xx_Handle hDev,					/* [in] Device handle */
	BTNR_PowerSaverSettings *pwrSettings /* [in] Power saver settings. */
	)
{   
    BTNR_P_75xx_Settings *pTnrImplData;
    BERR_Code retCode = BERR_SUCCESS;

    BDBG_ENTER(BTNR_75xx_SetPowerSaver);
    BDBG_ASSERT( hDev );
    BDBG_ASSERT( hDev->magicId == DEV_MAGIC_ID );

    pTnrImplData = &hDev->settings;
	pTnrImplData->powerSaver = pwrSettings->enable;
	BTNR_7550_P_PowerDn_TNR_Core0(hDev);
	
    BDBG_LEAVE(BTNR_75xx_SetPowerSaver);
    return( retCode );
}



 
