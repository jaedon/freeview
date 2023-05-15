/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_hdcp.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 8/3/11 2:08p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/3548/bhdr_hdcp.c $
 * 
 * Hydra_Software_Devel/10   8/3/11 2:08p rgreen
 * SW7425-833: Update to use BDBG_OBJECT_ASSERT
 * 
 * Hydra_Software_Devel/9   4/18/11 11:42a rgreen
 * SW7422-186: Merge changes for HDCP Repeater Support
 * 
 * Hydra_Software_Devel/SW7422-186/5   4/13/11 1:54p rgreen
 * SW7422-186: Update HDCP Status change callback
 * 
 * Hydra_Software_Devel/SW7422-186/4   4/11/11 3:27p rgreen
 * SW7422-186: Add HDCP Repeater support
 * 
 * Hydra_Software_Devel/SW7422-186/2   3/31/11 2:37p rgreen
 * SW7422-186: Add HDCP Repeater support
 * 
 * Hydra_Software_Devel/SW7422-186/1   3/28/11 5:37p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * Hydra_Software_Devel/8   3/17/11 5:03p rgreen
 * SW7425-193: Create/use HDCP private functions
 * 
 * Hydra_Software_Devel/7   3/4/11 5:22p rgreen
 * SWDTV-4664,SW7422-129: Add return code for Enable/Disable HDCP Key
 * Loading
 * 
 * Hydra_Software_Devel/6   3/3/11 11:48a rgreen
 * SWDTV-4664: Update HDCP Key Set verification and HDCP processing for
 * OTP ROM based chips
 * 
 * Hydra_Software_Devel/5   2/3/11 8:46p rgreen
 * SW35230-2374: Replace HdcpKeySetStatus with generic HdcpStatus used
 * accross all platforms.  Replace BHDR_HDCP_GetKeySetStatus with generic
 * BHDR_HDCP_GetStatus. Include KeySet status in HdcpStatus structure
 * 
 * Hydra_Software_Devel/4   12/20/10 3:58p rgreen
 * SW35230-2374: Use state machine to describe status of HDCP KeySet
 * verification from OTP
 * 
 * Hydra_Software_Devel/3   12/8/10 12:39p rgreen
 * SW35230-2374,SW7422-129: Add support for HDCP Key loading and
 * verfication
 * 
 * Hydra_Software_Devel/2   12/7/10 4:59p rgreen
 * SW35230-2416: Add timer support to delay reset of front end;
 * 
 * Hydra_Software_Devel/1   12/6/10 10:29a rgreen
 * SW35230-2374: Add support for HDCP Key verfication
 * 
 ***************************************************************************/
#include "bhdr_hdcp.h"
#include "bhdr_priv.h"
#include "bhdr_hdcp_priv.h"


BDBG_MODULE(BHDR_HDCP) ;


/******************************************************************************
Summary: Get Default HDCP Settings 
*******************************************************************************/
void BHDR_HDCP_GetDefaultSettings(BHDR_HDCP_Settings *pHdcpSettings) 
{
	BDBG_ENTER(BHDR_HDCP_GetSettings) ;
	
	BKNI_Memset(pHdcpSettings, 0, sizeof(BHDR_HDCP_Settings)) ;
	
#if 	HDMI_RX_GEN == 7422
	pHdcpSettings->bRepeater = true ;
      pHdcpSettings->uiMaxDevices = 15 ;
      pHdcpSettings->uiMaxLevels = 7 ;
#else
       /* does not support repeaters */
	pHdcpSettings->bRepeater = false ;
      pHdcpSettings->uiMaxDevices = 1 ;
      pHdcpSettings->uiMaxLevels = 1 ;

#endif

	BDBG_LEAVE(BHDR_HDCP_GetSettings) ;
}



/******************************************************************************
Summary: Get Current HDCP Settings 
*******************************************************************************/
void BHDR_HDCP_GetSettings(BHDR_Handle hHDR, BHDR_HDCP_Settings *pHdcpSettings) 
{
	BDBG_ENTER(BHDR_HDCP_GetSettings) ;
	BKNI_Memset(pHdcpSettings, 0, sizeof(BHDR_HDCP_Settings)) ;
	
	BKNI_Memcpy(pHdcpSettings, &hHDR->stHdcpSettings, 
		sizeof(BHDR_HDCP_Settings)) ;

	BDBG_LEAVE(BHDR_HDCP_GetSettings) ;
}


/******************************************************************************
Summary: Set/Update HDCP Settings 
*******************************************************************************/
void BHDR_HDCP_SetSettings(BHDR_Handle hHDR, BHDR_HDCP_Settings *pHdcpSettings) 
{
	BREG_Handle hRegister ;
	uint32_t Register ;
	uint32_t ulOffset ;

	BDBG_ENTER(BHDR_HDCP_SetSettings) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	BDBG_ASSERT(pHdcpSettings) ;

	hRegister = hHDR->hRegister ;
	ulOffset = hHDR->ulOffset ;

	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_CONFIG + ulOffset) ;
	Register &= ~ (
		  BCHP_MASK(HDMI_RX_0_HDCP_CONFIG, RDB_REPEATER)
		| BCHP_MASK(HDMI_RX_0_HDCP_CONFIG, DISABLE_OTP_REPEATER)) ;
	Register |= 
		  BCHP_FIELD_DATA(HDMI_RX_0_HDCP_CONFIG, RDB_REPEATER, pHdcpSettings->bRepeater)
		| BCHP_FIELD_DATA(HDMI_RX_0_HDCP_CONFIG, DISABLE_OTP_REPEATER, pHdcpSettings->bRepeater) ;
	BREG_Write32(hRegister, BCHP_HDMI_RX_0_HDCP_CONFIG + ulOffset, Register) ;

	BKNI_Memcpy(&hHDR->stHdcpSettings, pHdcpSettings, sizeof(BHDR_HDCP_Settings)) ;

#if HDMI_RX_GEN == 7422
	if (!pHdcpSettings->bRepeater)
	{
		BDBG_WRN((" ")) ;
		BDBG_WRN(("!!!!!  HDCP License requires repeater functionality be implemented")) ;
		BDBG_WRN(("!!!!!  if devices re-transmit encrypted content")) ;
		BDBG_WRN((" ")) ;
	}
#endif

#if 0
TODO CODE TO RETRIEVE BKSV ONCE STORED
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_MON_BKSV_0 + ulOffset) ;
		hHDR->stHdcpSettings.rxBksv[0] = (Register & 0x000000FF) ;
		hHDR->stHdcpSettings.rxBksv[1] = (Register & 0x0000FF00) >>  8 ;
		hHDR->stHdcpSettings.rxBksv[2] = (Register & 0x00FF0000) >> 16 ;
		hHDR->stHdcpSettings.rxBksv[3] = (Register & 0xFF000000) >> 24 ;
		
	Register = BREG_Read32(hRegister, BCHP_HDMI_RX_0_HDCP_MON_BKSV_1 + ulOffset) ;
		hHDR->stHdcpSettings.rxBksv[4] = (Register & 0x000000FF) ;
#endif
	hHDR->stHdcpStatus.eAuthState = BHDR_HDCP_AuthState_eIdle ;

	BDBG_LEAVE(BHDR_HDCP_SetSettings) ;
	
}


/******************************************************************************
Summary: Enable HDCP Key Loading 
*******************************************************************************/
BERR_Code BHDR_HDCP_EnableKeyLoading(BHDR_Handle hHDR) 
{
	BERR_Code rc = BERR_SUCCESS ;

	BDBG_ENTER(BHDR_HDCP_EnableKeyLoading) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	rc = BHDR_HDCP_P_EnableKeyLoading(hHDR) ;

	BDBG_LEAVE(BHDR_HDCP_EnableKeyLoading) ;
	return rc ;
}


/******************************************************************************
Summary: Disable HDCP Key Loading 
*******************************************************************************/
BERR_Code BHDR_HDCP_DisableKeyLoading(BHDR_Handle hHDR) 
{
	BERR_Code rc = BERR_SUCCESS ;	
	
	BDBG_ENTER(BHDR_HDCP_DisableKeyLoading) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	rc = BHDR_HDCP_P_DisableKeyLoading(hHDR);

	BDBG_LEAVE(BHDR_HDCP_DisableKeyLoading) ;
    return rc ;
}


/******************************************************************************
Summary: Install Callback used to Notify for changes in HDCP Status,  Key Set loading etc.
*******************************************************************************/
BERR_Code BHDR_HDCP_InstallHdcpStatusChangeCallback( 
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for packet error changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2)    /* [in] the second argument(int) passed to the callback function */ 
{
	BERR_Code			rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDR_HDCP_InstallHdcpStatusChangeCallback) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	/* Check if this is a valid function */
	if( pfCallback_isr == NULL )
	{
		rc = BERR_TRACE(BERR_INVALID_PARAMETER);
		return rc;
	}
		
	BKNI_EnterCriticalSection() ;
		hHDR->pfHdcpStatusChangeCallback = pfCallback_isr ;
		hHDR->pvHdcpStatusChangeParm1 = pvParm1  ;
		hHDR->iHdcpStatusChangeParm2 = iParm2 ;
	BKNI_LeaveCriticalSection() ;

	BDBG_LEAVE(BHDR_HDCP_InstallHdcpStatusChangeCallback);
	
	return rc ;
}


/******************************************************************************
Summary: Uninstall Callback used to Notify for for changes in HDCP Status,  Key Set loading etc.
*******************************************************************************/
BERR_Code BHDR_HDCP_UnInstallHdcpStatusChangeCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr) /* [in] cb for Packet Error change Notification */
{
	BERR_Code rc = BERR_SUCCESS ;

	BSTD_UNUSED(pfCallback_isr) ;

	BDBG_ENTER(BHDR_HDCP_UnInstallHdcpStatusChangeCallback) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;

	BKNI_EnterCriticalSection() ;
	
		hHDR->pfHdcpStatusChangeCallback = NULL ;
		hHDR->pvHdcpStatusChangeParm1 = NULL  ;
		hHDR->iHdcpStatusChangeParm2 = 0  ;

	BKNI_LeaveCriticalSection() ;
	
	BDBG_LEAVE(BHDR_UnInstallPacketErrorChangeCallback);
	
	return rc ;
}


/******************************************************************************
Summary: Get current status of HDCP Key Set, authentication etc.
*******************************************************************************/
BERR_Code BHDR_HDCP_GetStatus(BHDR_Handle hHDR, BHDR_HDCP_Status *pStatus) 
{
	BERR_Code rc = BERR_SUCCESS ;
	
	BDBG_ENTER(BHDR_HDCP_GetStatus) ;
	BDBG_OBJECT_ASSERT(hHDR, BHDR_P_Handle) ;
	
	rc = BHDR_HDCP_P_GetStatus(hHDR, pStatus) ;

	BDBG_LEAVE(BHDR_HDCP_GetStatus) ;
	
	return rc ;
}



