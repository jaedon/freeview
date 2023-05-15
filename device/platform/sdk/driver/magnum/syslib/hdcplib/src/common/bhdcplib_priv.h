/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdcplib_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 4/30/12 2:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/hdcplib/7401/bhdcplib_priv.h $
 * 
 * Hydra_Software_Devel/3   4/30/12 2:43p vle
 * SW7425-2666, SW7405-5150: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-2666_FifoTest/1   4/27/12 3:02p vle
 * SW7425-2666, SW7405-5150: Add additional state waitForValidVideo to
 * HDCP state machine and add configurable delay for reading KVS List in
 * addition to the required 5seconds time out. - Take 2
 * 
 * Hydra_Software_Devel/2   3/18/08 7:18p vle
 * PR 39991: Rename HDMIlib to HDCPlib. Remove unused implementation.
 * 
 * Hydra_Software_Devel/1   2/26/08 5:43p jgarrett
 * PR 39991: Renaming new HDMlib to HDCPlib
 * 
 * Hydra_Software_Devel/Syslib_Initiative/4   2/21/08 5:00p vle
 * PR 31924: Change Settings back to Dependencies
 * Remove Hotplug callback
 * Move RevokedKSVList to Handle
 * 
 * Hydra_Software_Devel/Syslib_Initiative/3   2/15/08 10:01p vle
 * PR 31924: Improve HDMI Syslib API from feedback.
 * 
 ***************************************************************************/
#ifndef BHDCPLIB_PRIV_H__
#define BHDCPLIB_PRIV_H__

#include "bhdcplib.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct BHDCPlib_P_Handle
{
	BHDCPlib_Dependencies stDependencies;

	/* HDCP */
	BHDCPlib_Event stHdcpLIC_Ri;
	BHDCPlib_Event stHdcpLIC_Pj;
	BHDCPlib_Configuration stHdcpConfiguration ;
	BHDCPlib_RevokedKsvList RevokedKsvList;
	BHDCPlib_Status stHdcpStatus;
	uint8_t uiKsvFifoReadyCount;	/* Number of times KSV FIFO has been checked */

	BTMR_Handle hTmr ;
	BTMR_TimerHandle hTimer;
	BTMR_Settings stTmrSettings;
	bool bR0ReadyTimerExpired;
	bool bRepeaterReadyTimerExpired;
} BHDCPlib_P_Handle;


/******************
Summary: Initiate the authentication process with the attached receiver
******************/
BERR_Code BHDCPlib_InitializeReceiverAuthentication(BHDCPlib_Handle hHDCPlib);


/******************
Summary: Authenticate the receiver
Descripttion: Exchange KSVs, An and verify R0=R0'
******************/
BERR_Code BHDCPlib_AuthenticateReceiver(BHDCPlib_Handle hHDCPlib);


/******************
Summary: Initiate the authentication process with the attached repeater
******************/
BERR_Code BHDCPlib_InitializeRepeaterAuthentication(BHDCPlib_Handle hHDCPlib);


/******************
Summary: Authenticate attached repeater
Descripttion: Assemble a list of all downstream KSVs attached to the HDCP Repeater	
******************/
BERR_Code BHDCPlib_AuthenticateRepeater(BHDCPlib_Handle hHDCPlib);


#ifdef __cplusplus
}
#endif
 
#endif /* BHDCPLIB_PRIV_H__ */


