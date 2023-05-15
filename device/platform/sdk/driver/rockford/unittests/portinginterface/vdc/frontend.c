/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: frontend.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date:
 *
 * Module Description:
 *
 * Revision History:
 *
 * 
 *
 ***************************************************************************/

#include <stdio.h>          /* for printf */
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "platform.h"
#include "framework.h"
#include "framework_board.h"

/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "bkni.h"           /* kernel interface */
#include "bkni_multi.h"     /* kernel interface */
#include "btst_kni.h"       /* to handle task */

/* format information */
#include "bfmt.h"

/* audio video common */
#include "bavc.h"

/* top level interrupts */
#include "int1.h"
#include "bint_plat.h"

/* test include */
#include "config.h"
#include "test_util.h"
#include "interactive.h"
#include "commands.h"

#include "bmem_debug.h"

#if (FRAMEWORK_DO_I2C == 1)
#include "bi2c.h"
#endif


BDBG_MODULE(vdc_test_frontend);


static BTNR_DTT76800_Settings tuner_settings;
static BVSB_Settings demod_settings;

static BERR_Code tuner_init(const BFramework_Info    *pFrameworkInfo, BTST_P_Context *pContext)
{
	BERR_Code err = 0;
	
    if ((err = BTNR_DTT76800_GetDefaultSettings(&tuner_settings, pFrameworkInfo->hChp)))
    {
		printf("%s: Get DTT76800 default settings error %d\n", __FUNCTION__, err);
		return err;
    }

	if ((err = BTNR_DTT76800_Open(&pContext->hTuner, pFrameworkInfo->hChp, 
		         pFrameworkInfo->hFrmWorkBoard->ahI2cRegHandles[0], &tuner_settings)))
	{
		printf("%s  Open DTT76800 error %d\n", __FUNCTION__, err);
		return err;
	}

	return err;
}


static BERR_Code demod_init(const BFramework_Info    *pFrameworkInfo, BTST_P_Context *pContext)
{
	BERR_Code err = 0;
	
    if ((err =  BVSB_3548_GetDefaultSettings(&demod_settings)))
    {
		printf("%s: Get VSB default settings error %d\n", __FUNCTION__, err);
		return err;
    }

    demod_settings.hTmr = pContext->hTmr;

	/* open device */
	if ((err = BVSB_Open(&pContext->hDemodulator, pFrameworkInfo->hChp, pFrameworkInfo->hReg, 
		      pFrameworkInfo->hInt, &demod_settings)))
	{
		printf("%s  Open VSB error %d\n", __FUNCTION__, err);
		return err;
	}

	/* Initialize Acquisition Processor */
    if ((err = BVSB_InitAp(pContext->hDemodulator, NULL)))
	{
		printf("%s	Init VSB AP error %d\n", __FUNCTION__, err);
		return err;
	}

	return err;
}


BERR_Code frontend_init(const BFramework_Info    *pFrameworkInfo, BTST_P_Context *pContext)
{
	BERR_Code err = 0;
	
	/* Initialize tuner */
	if ((err = tuner_init(pFrameworkInfo, pContext)))
	{
		printf("%s: tuner init err %d\n ", __FUNCTION__, err);
		return err;
	}


	/* Initialize demodulator */ 
	if ((err = demod_init(pFrameworkInfo, pContext)))
	{
		printf("%s: demodulator init err %d\n ", __FUNCTION__, err);
		return err;
	}


	return err; 
}

BERR_Code frontend_exit(
	void
	)
{
	return 0;	

}

/* end of file */
