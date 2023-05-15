/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bbcp_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/26/12 7:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/bcp/7346/bbcp_priv.h $
 * 
 * Hydra_Software_Devel/1   2/26/12 7:19p agin
 * SWNOOS-518:  Create BICAP PI.
 * 
 * Hydra_Software_Devel/3   2/3/05 11:35a agin
 * PR13212: RC6 support
 * 
 * Hydra_Software_Devel/2   9/16/03 7:03p brianlee
 * Added enable/disable interrupt and ISR functions.
 * 
 * Hydra_Software_Devel/1   7/31/03 10:39a brianlee
 * Private ICAP functions.
 * 
 ***************************************************************************/
#ifndef BBCP_PRIV_H__
#define BBCP_PRIV_H__

#include "bbcp.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
	This function is the FIFO overflow ISR

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
static void BBCP_P_HandleFifoOfInterrupt_Isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
);

/***************************************************************************
Summary:
	This function is the FIFO level ISR

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
static void BBCP_P_HandleFifoIrqInterrupt_Isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
);

/***************************************************************************
Summary:
	This function is the FIFO inactivity ISR

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
static void BBCP_P_HandleCpuTimeoutIrqInterrupt_Isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
);

/***************************************************************************
Summary:
	This function is the edge timeout ISR

Description:
		
Returns:
	TODO:

See Also:
	

****************************************************************************/
static void BBCP_P_HandleEdgeTimeoutIrqInterrupt_Isr
(
	void *pParam1,						/* Device channel handle */
	int parm2							/* not used */
);

#ifdef __cplusplus
}
#endif
 
#endif



