/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btnr_3420_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 3/24/11 6:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/tnr/3420/btnr_3420_priv.h $
 * 
 * Hydra_Software_Devel/5   3/24/11 6:14p VISHK
 * SW7405-5202: BTNR_3420 should not assert on I2C failure
 * 
 * Hydra_Software_Devel/4   9/19/07 11:23a vishk
 * PR 29024: To create PI for BCM3420 tuner.
 * 
 * Hydra_Software_Devel/3   9/12/07 12:52p vishk
 * PR 29024: To create PI for BCM3420 tuner.
 * 
 * Hydra_Software_Devel/2   9/6/07 2:11p vishk
 * PR 29024: To create PI for BCM3420 tuner.
 * 
 * Hydra_Software_Devel/1   5/12/07 3:59p dlwin
 * PR 29024: initial version of 3420 PI, never tested.
 * 
 ***************************************************************************/

#ifndef BTNR_3420_PRIV_H__
#define BTNR_3420_PRIV_H__


#ifdef __cplusplus
extern "C" {
#endif

typedef struct BTNR_P_3420_Settings
{
	unsigned long ifFreq;
	unsigned long xtalFreq;
	BREG_I2C_Handle hI2CReg;
	unsigned short i2cAddr;

	int iRevLetter, iRevNumber, iType;		/* Saved chip information */
	int enableAgcGain;
	unsigned long rfFreq;

	
	unsigned short AnnexMode;	/* only used for Docsis tuner */	
	unsigned long DSFreq;		/* only used for Docsis tuner */
	unsigned char TunerType;
	bool PowerTune;				/* only used for Docsis tuner */
	BTNR_TunerMode tunerMode;
	bool isInitialized;
} BTNR_P_3420_Settings;

BERR_Code BTNR_P_3420_tune( BTNR_P_3420_Settings *pTnrImplData, int lFreq );
BERR_Code BTNR_P_3420_initialize( BTNR_P_3420_Settings *pTnrImplData );


#ifdef __cplusplus
}
#endif
 
#endif



