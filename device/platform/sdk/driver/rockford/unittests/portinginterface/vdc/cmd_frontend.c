/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "test_util.h"
#include "commands.h"
#include "bvdc_dbg.h"
#include "bkni_multi.h"
#include "bdbg.h"
#include "bstd_defs.h"


void BTST_P_GetVsbStatus(BTST_P_Context  *pContext )
{
	BVSB_Handle hVsb = pContext->hDemodulator;
	BVSB_QamStatus qamStatus;

	/* Get QAM for now. Add others later. */ 
	BVSB_GetQamStatus(hVsb, &qamStatus);

	printf("QAM status:  FEC Locked[%d], PLL Locked[%d] \n", qamStatus.bFecLocked, qamStatus.bPllLocked);

}

void BTST_P_Tune(BTST_P_Context  *pContext )
{
	int mode, freq;
	BVSB_InbandParams vsb_ib_params;
    BVSB_Settings vsb_settings;
	BTNR_TunerMode tuner_mode;
	int err;

	printf("Enter mode: QAM_64(0), QAM_128(1), QAM_256(2)\n");
	BTST_P_Scanf(pContext, "%d", &mode);

	/* Support only QAM Annex B for now */ 
	switch(mode)
	{
		case 0:
			vsb_ib_params.mode = BVSB_InbandMode_e64QAM_B;
			tuner_mode = BTNR_TunerMode_eDigital;
			break;
			
		case 1:
			vsb_ib_params.mode = BVSB_InbandMode_e128QAM_B;
			tuner_mode = BTNR_TunerMode_eDigital;
			break;

		case 2:
			vsb_ib_params.mode = BVSB_InbandMode_e256QAM_B;
			tuner_mode = BTNR_TunerMode_eDigital;
			break;

		default:
			printf("Unsupported mode %d\n", mode);
			return;
			break;
	}

	printf("Enter freq in MHz\n");
	BTST_P_Scanf(pContext, "%d", &freq);

	freq *= 1000000;

	/* Set up demodulator */ 
    BVSB_GetSettings(pContext->hDemodulator, &vsb_settings);
    vsb_settings.qam.bFastAcq = 0;
    vsb_settings.qam.bTerr = 0; /* This needs to be set to 1 for VSB */ 
    if ((err = BVSB_SetSettings(pContext->hDemodulator, &vsb_settings)))
	{
		printf("%s: Failed to set VSB, error %d\n", 
			   __FUNCTION__, err);
		return;
	}

	/* Set up tuner */ 
	if ((err=BTNR_SetTunerRfFreq(pContext->hTuner, (uint32_t)freq, tuner_mode)))
	{
		printf("%s: Failed to set tuner frequency to %dHz, error %d\n", 
			   __FUNCTION__, freq, err);
		return;
	}

	/* Acquire in band */
	vsb_ib_params.ifFreqOffset = 0;
	vsb_ib_params.symbolRateOffset = 0;
	vsb_ib_params.symbolRate = 0;
    if ((err=BVSB_AcquireInband(pContext->hDemodulator, &vsb_ib_params)))
    {
		printf("%s: Failed to acquire inband, error %d\n", 
			   __FUNCTION__, err);
		return;
    }
}

void BTST_P_XptSrc(BTST_P_Context  *pContext )
{
	int xptSrc;
	
	printf("Choose the source for transport processor: Streamer(0), Tuner+Demodulator(1)\n");
	BTST_P_Scanf(pContext, "%d", &xptSrc);

	if (xptSrc > BTST_XPT_SRC_Demodulator)
	{
		printf("Invalid source[%d] for transport processor\n", xptSrc);
		return;
	}

	pContext->eXptSrc = (BTST_P_Xpt_Src) xptSrc;
}


/* end of file */
