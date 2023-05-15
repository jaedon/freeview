/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: birb_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 2/20/08 1:56p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/irb/7405/birb_priv.h $
 * 
 * Hydra_Software_Devel/8   2/20/08 1:56p jkim
 * PR37963: Add XMP-2 support
 * 
 * Hydra_Software_Devel/7   10/7/04 6:35p brianlee
 * PR11333: Modified multiple sequence blast functions to accommodate
 * another case.
 * 
 * Hydra_Software_Devel/6   9/22/04 5:59p brianlee
 * PR11333: Fixed various multiple sequence sending functions.
 * 
 * Hydra_Software_Devel/5   7/23/04 4:02p brianlee
 * PR11333: Added another type of multiple blast sequences.
 * 
 * Hydra_Software_Devel/4   6/7/04 4:13p brianlee
 * PR11386: Make configregisters function public so it can be called by
 * test routines.
 * 
 * Hydra_Software_Devel/3   6/3/04 11:18a brianlee
 * PR11333: Added support for sending multiple blast sequences.
 * 
 * Hydra_Software_Devel/2   3/4/04 5:34p brianlee
 * PR9958: Added BIRB_Device_eCustom type and the function
 * BIRB_ConfigCustom() to allow user to configure custom IR blaster
 * setting.
 * 
 * Hydra_Software_Devel/1   9/18/03 4:13p brianlee
 * Initial version.
 * 
 * Hydra_Software_Devel/2   9/16/03 7:03p brianlee
 * Added enable/disable interrupt and ISR functions.
 * 
 * Hydra_Software_Devel/1   7/31/03 10:39a brianlee
 * Private ICAP functions.
 * 
 ***************************************************************************/
#ifndef BICP_PRIV_H__
#define BICP_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

void BIRB_P_EnableInt(
	BIRB_Handle 	hDev
);

void BIRB_P_DisableInt(
	BIRB_Handle 	hDev
);

void BIRB_P_ConfigDataSequence(
	BIRB_Handle 	hDev,
	uint32_t		*pData,			/* pointer to data to blast */
	uint8_t			bits,			/* number of bits to blast */
	bool			headerPulse		/* header flag */
);

void BIRB_P_ConfigDataSequenceAB(
	BIRB_Handle 	hDev, 
	uint32_t		*pDataA, 		/* pointer to data A to blast */
	uint8_t			bitsA, 			/* number of bits A to blast */
	uint32_t		*pDataB, 		/* pointer to data B to blast */
	uint8_t			bitsB, 			/* number of bits B to blast */
	bool			headerA, 		/* header A flag */
	bool			headerB, 		/* header B pulse */
	bool			fixedFlag 		/* fixed Frame flag */
);

void BIRB_P_ConfigDataSequenceAA(
	BIRB_Handle 	hDev,
	uint32_t		*pData,			/* pointer to data to blast */
	uint8_t			bits,			/* number of bits to blast */
	bool			headerPulse, 	/* header flag */
	bool			fixedFlag		/* fixed frame flag */
);

void BIRB_P_ConfigDataSequenceXmp2(
    BIRB_Handle     hDev,
	uint8_t 		*pData,             /* pointer to data to blast */
	uint8_t			numByte				/* number of bytes to send. 1 or 4 */
);

static void BIRB_P_HandleInterrupt_Isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
);

#ifdef __cplusplus
}
#endif
 
#endif



