/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkir_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 9/15/03 10:29a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/kir/7038/bkir_priv.h $
 * 
 * Hydra_Software_Devel/1   9/15/03 10:29a brianlee
 * Initial version.
 * 
 * 
 ***************************************************************************/
#ifndef BKIR_PRIV_H__
#define BKIR_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

void BKIR_P_EnableInt(
	BKIR_ChannelHandle 	hChn			/* Device channel handle */
	);

void BKIR_P_DisableInt(
	BKIR_ChannelHandle 	hChn			/* Device channel handle */
	);

static void BKIR_P_HandleInterrupt_Isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
);

void BKIR_P_WriteCirParam (
	BKIR_ChannelHandle	hChn, 		/* Device channel handle */
	uint32_t				addr,
	uint32_t				data
);

void BKIR_P_ConfigCir (
	BKIR_ChannelHandle	hChn, 		/* Device channel handle */
	BKIR_KirDevice 		device		/* device type to enable */
);


#ifdef __cplusplus
}
#endif
 
#endif



