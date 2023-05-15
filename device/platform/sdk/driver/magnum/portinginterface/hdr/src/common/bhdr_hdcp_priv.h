/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_hdcp_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/17/11 4:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/3548/bhdr_hdcp_priv.h $
 * 
 * Hydra_Software_Devel/1   3/17/11 4:45p rgreen
 * SW7425-193: Add bhdr_hdcp_priv.h
 * 
 ***************************************************************************/

BERR_Code BHDR_HDCP_P_DisableKeyLoading(BHDR_Handle hHDR) ;

BERR_Code BHDR_HDCP_P_EnableKeyLoading(BHDR_Handle hHDR) ;


BERR_Code BHDR_HDCP_P_InstallHdcpStatusChangeCallback( 
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for packet error changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2) ;    /* [in] the second argument(int) passed to the callback function */ 


BERR_Code BHDR_HDCP_P_UnInstallHdcpStatusChangeCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr) ; /* [in] cb for Packet Error change Notification */

BERR_Code BHDR_HDCP_P_GetStatus(BHDR_Handle hHDR, BHDR_HDCP_Status *pStatus) ;

