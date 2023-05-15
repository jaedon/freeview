/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: burt_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/21/09 5:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/urt/7405/burt_priv.h $
 * 
 * Hydra_Software_Devel/2   7/21/09 5:42p jkim
 * PR30806: Add the function to set stop bits
 * 
 * Hydra_Software_Devel/1   9/16/03 5:00p brianlee
 * Initial version.
 * 
 * 
 ***************************************************************************/
#ifndef BURT_PRIV_H__
#define BURT_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif

void BURT_P_EnableTxRx(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	bool				enableTx,		/* enable flag for transmitter */
	bool				enableRx		/* enable flag for receiver */
);

void BURT_P_SetBaudRate(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	uint32_t			baud
);

void BURT_P_SetDataBits(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	BURT_DataBits		bits
);

void BURT_P_SetParity(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	BURT_Parity			parity
);

void BURT_P_SetStopBits(
	BURT_ChannelHandle 	hChn,			/* Device channel handle */
	BURT_StopBits		stop_bits
);

static void BURT_P_HandleInterrupt_Isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
);

#ifdef __cplusplus
}
#endif
 
#endif



