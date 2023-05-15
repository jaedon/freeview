/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdcplib.c $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date: 7/5/12 1:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/hdcplib/7401/bhdcplib.c $
 * 
 * Hydra_Software_Devel/19   7/5/12 1:17p vle
 * SW7425-1988: Merge to mainline
 * 
 * Hydra_Software_Devel/18   6/2/12 3:28p rgreen
 * SW7425-1988: Use text based debug message instead of code to report
 * failure
 * 
 * Hydra_Software_Devel/17   4/30/12 2:42p vle
 * SW7425-2666, SW7405-5150: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-2666_FifoTest/2   4/27/12 3:02p vle
 * SW7425-2666, SW7405-5150: Add additional state waitForValidVideo to
 * HDCP state machine and add configurable delay for reading KVS List in
 * addition to the required 5seconds time out. - Take 2
 * 
 * Hydra_Software_Devel/SW7425-2666_FifoTest/1   4/25/12 5:17p vle
 * SW7425-2666, SW7405-5150: Add additional state waitForValidVideo to
 * HDCP state machine and add configurable delay for reading KVS List in
 * addition to the required 5seconds time out.
 * 
 * Hydra_Software_Devel/16   4/16/12 3:12p rgreen
 * SW7425-2666: Update debug message to indicate which timer has expired
 * 
 * Hydra_Software_Devel/15   11/14/11 3:23p rgreen
 * SW7325-872: Fix unused warning messages for non-debug buiilds
 * 
 * Hydra_Software_Devel/14   11/7/11 11:25a rgreen
 * SW7325-872,SW7405-3987:  Remove double negative in error message.
 * Update message to include the return code and current HDCP
 * Authentication state
 * 
 * Hydra_Software_Devel/13   10/31/11 3:34p rgreen
 * SW7325-872: update debug messages to use text instead of numbers when
 * showing the HDCP states
 * 
 * Hydra_Software_Devel/12   10/12/11 3:39p vle
 * SW7125-1091: Enable encryption right after part 1 of hdcp
 * authentication succeed
 * 
 * Hydra_Software_Devel/11   9/9/11 3:19p vle
 * SW7420-2041: Avoid unnecessary attempts to check hdcp status when HDMI
 * Rx is disconnected/unpowered
 * 
 * Hydra_Software_Devel/10   3/3/10 12:37p vle
 * SW7405-3987: Merge into main branch
 * 
 * Hydra_Software_Devel/SW7405-3987/2   3/2/10 7:12p vle
 * SW7405-3987: Report specific HDCP error status to high level
 * application - Take 2: warn users for invalid errors.
 * 
 * Hydra_Software_Devel/SW7405-3987/1   3/2/10 5:39p vle
 * SW7405-3987: Report specific HDCP error status to high level
 * application.
 * 
 * Hydra_Software_Devel/9   1/28/10 1:56p vle
 * SW7420-489: merge to main branch
 * 
 * Hydra_Software_Devel/SW7420-489/1   1/26/10 3:39p vle
 * SW7420-489: propagate max_device_count and max_depth to application
 * 
 * Hydra_Software_Devel/8   8/25/09 3:31p vle
 * SW7405-2670:
 * Do not poll for receiver sense while encryption is still enabled.
 * Add implementation of interrupt based receiver sense
 * 
 * Hydra_Software_Devel/PR56486/1   8/6/09 12:05p vle
 * PR56486: Do not poll for receiver sense while encryption is still
 * enabled.
 * 
 * Hydra_Software_Devel/7   2/27/09 8:56p vle
 * PR52505: Fix non-const global data in HDM and HDCPLIB
 * 
 * Hydra_Software_Devel/6   11/24/08 8:42p vle
 * PR49219, PR49593, PR48842, PR48937: Merge to main branch
 * 
 * Hydra_Software_Devel/HDMI_TX_Plugfest11_200811/2   11/24/08 8:26p vle
 * PR49219, PR49593, PR48842: Add additional state in the state machine to
 * clearly identify HDCP authentication errors. Update the current hdcp
 * state appropriately.
 * 
 * Hydra_Software_Devel/HDMI_TX_Plugfest11_200811/1   11/12/08 4:54p vle
 * PR 48937: Add support for HDCP repeater error during HDCP
 * authentication process.
 * 
 * Hydra_Software_Devel/5   11/10/08 6:56p vle
 * PR44550, PR48842: Be sure to retrieve receiver info for the
 * authentication process instead of relying on the application.
 * 
 * Hydra_Software_Devel/4   11/6/08 11:23a vle
 * PR48705: Fix crash in kernel mode when closing BHDCPlib.
 * 
 * Hydra_Software_Devel/3   10/29/08 7:40p vle
 * PR44550: Merge changes from bdvd_v2.0 branch.
 * Fix bug on size of revoked KSV list
 * Be sure to free the revoked KSV list appropriately.
 * 
 * Hydra_Software_Devel/2   3/18/08 7:18p vle
 * PR 39991: Rename HDMIlib to HDCPlib. Remove unused implementation.
 * 
 * Hydra_Software_Devel/1   2/26/08 5:43p jgarrett
 * PR 39991: Renaming new HDMlib to HDCPlib
 * 
 * Hydra_Software_Devel/Syslib_Initiative/11   2/22/08 6:16p vle
 * PR 31924: Set default HDCP configuration during open.
 * Use HDCP test keys as default HDCP keys.
 * 
 * Hydra_Software_Devel/Syslib_Initiative/10   2/21/08 7:01p vle
 * PR 31924: Fix RevokedKSV callback
 * 
 * Hydra_Software_Devel/Syslib_Initiative/9   2/21/08 5:00p vle
 * PR 31924: Change Settings back to Dependencies
 * Remove Hotplug callback
 * Move RevokedKSVList to Handle
 * 
 * Hydra_Software_Devel/Syslib_Initiative/8   2/15/08 10:01p vle
 * PR 31924: Improve HDMI Syslib API from feedback.
 * 
 ***************************************************************************/

#include "bavc_hdmi.h"
#include "bhdcplib.h"
#include "bhdcplib_priv.h"
#include "bhdcplib_keyloader.h"


BDBG_MODULE(BHDCPLIB) ;


#define	BHDCPLIB_CHECK_RC( rc, func )	              \
do                                                \
{										          \
	if( (rc = BERR_TRACE(func)) != BERR_SUCCESS ) \
	{										      \
		goto done;							      \
	}										      \
} while(0)


/* HDCP default settings */
static const BHDCPlib_Configuration stHdcpDefaultConfiguration = 
{
	BHDM_HDCP_AnSelect_eRandomAn, 
	{
		{0x14, 0xF7, 0x61, 0x03, 0xB7}, 
		{		/* HDCP Test Keys */		
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x691e138f, 0x58a44d00},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x0950e658, 0x35821f00},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x0d98b9ab, 0x476a8a00},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xcac5cb52, 0x1b18f300},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xb4d89668, 0x7f14fb00},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x818f4878, 0xc98be000},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x412c11c8, 0x64d0a000},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x44202428, 0x5a9db300},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x6b56adbd, 0xb228b900},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xf6e46c4a, 0x7ba49100},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x589d5e20, 0xf8005600},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xa03fee06, 0xb77f8c00},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x28bc7c9d, 0x8c2dc000},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x059f4be5, 0x61125600},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xcbc1ca8c, 0xdef07400},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x6adbfc0e, 0xf6b83b00},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xd72fb216, 0xbb2ba000},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x98547846, 0x8e2f4800},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x38472762, 0x25ae6600},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xf2dd23a3, 0x52493d00},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x543a7b76, 0x31d2e200},  
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x2561e6ed, 0x1a584d00},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xf7227bbf, 0x82603200},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x6bce3035, 0x461bf600},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x6b97d7f0, 0x09043600},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xf9498d61, 0x05e1a100}, 
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x063405d1, 0x9d8ec900}, 
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x90614294, 0x67c32000},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xc34facce, 0x51449600},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x8a8ce104, 0x45903e00},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xfc2d9c57, 0x10002900},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x80b1e569, 0x3b94d700},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x437bdd5b, 0xeac75400},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xba90c787, 0x58fb7400}, 
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xe01d4e36, 0xfa5c9300},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xae119a15, 0x5e070300},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x01fb788a, 0x40d30500},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0xb34da0d7, 0xa5590000},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x409e2c4a, 0x633b3700},
			{ 0, 0, 0, 0, 0, 0, 0, 0, 0x412056b4, 0xbb732500}
		}
	},
	{{0x11,0x11,0x11,0x11,0x11}, 0, 0, false}, 
	100					/* msWaitForValidVideo */,
	0 					/* msWaitForRxR0Margin */, 
	100				 	/* msIntervalKsvFifoReadyCheck */,
	0					/* msWaitForKsvFifoMargin */,
	BHDM_HDCP_REPEATER_MAX_DEVICE_COUNT		/* uiMaxDeviceCount */,
	BHDM_HDCP_REPEATER_MAX_DEPTH			/* uiMaxDepth */
};


typedef struct _BHDCPLIB_P_HDM_HdcpErrors
{
	BERR_Code hdmHdcpError;
	BHDCPlib_HdcpError hdcplibHdcpError;
	
} BHDCPLIB_P_HDM_HdcpErrors;

static const BHDCPLIB_P_HDM_HdcpErrors BHDCPlib_SupportedHdcpErrors[BHDCPlib_HdcpError_eCount]=
{
	{BERR_SUCCESS,							BHDCPlib_HdcpError_eSuccess},
	{BHDM_HDCP_RX_BKSV_ERROR,				BHDCPlib_HdcpError_eRxBksvError},
	{BHDM_HDCP_RX_BKSV_REVOKED,				BHDCPlib_HdcpError_eRxBksvRevoked},
	{BHDM_HDCP_RX_BKSV_I2C_READ_ERROR,		BHDCPlib_HdcpError_eRxBksvI2cReadError},
	{BHDM_HDCP_TX_AKSV_ERROR,				BHDCPlib_HdcpError_eTxAksvError},
	{BHDM_HDCP_TX_AKSV_I2C_WRITE_ERROR, 	BHDCPlib_HdcpError_eTxAksvI2cWriteError},
	{BHDM_HDCP_RECEIVER_AUTH_ERROR, 		BHDCPlib_HdcpError_eReceiverAuthenticationError},
	{BHDM_HDCP_REPEATER_AUTH_ERROR,			BHDCPlib_HdcpError_eRepeaterAuthenticationError},
	{BHDM_HDCP_RX_DEVICES_EXCEEDED, 		BHDCPlib_HdcpError_eRxDevicesExceeded},
	{BHDM_HDCP_REPEATER_DEPTH_EXCEEDED, 	BHDCPlib_HdcpError_eRepeaterDepthExceeded},
	{BHDM_HDCP_REPEATER_FIFO_NOT_READY, 	BHDCPlib_HdcpError_eRepeaterFifoNotReady},
	{BHDM_HDCP_REPEATER_DEVCOUNT_0, 		BHDCPlib_HdcpError_eRepeaterDeviceCount0},
	{BHDM_HDCP_LINK_FAILURE,				BHDCPlib_HdcpError_eRepeaterLinkFailure},
	{BHDM_HDCP_LINK_RI_FAILURE, 			BHDCPlib_HdcpError_eLinkRiFailure},
	{BHDM_HDCP_LINK_PJ_FAILURE, 			BHDCPlib_HdcpError_eLinkPjFailure},
	{BHDM_HDCP_FIFO_UNDERFLOW,				BHDCPlib_HdcpError_eFifoUnderflow},
	{BHDM_HDCP_FIFO_OVERFLOW,				BHDCPlib_HdcpError_eFifoOverflow},
	{BHDM_HDCP_MULTIPLE_AN_REQUEST, 		BHDCPlib_HdcpError_eMultipleAnRequest}
};


#if BDBG_DEBUG_BUILD			
static const char *BHDCPlib_StateText[] =
{
	"UnPowered",
	"Unauthenticated",
	"WaitForValidVideo",
	"InitializeAuthentication",
	"WaitForReceiverAuthentication",
	"ReceiverR0Ready",
	"R0LinkFailure",
	"ReceiverAuthenticated",	/* Part 1 Completed; R0 Match */
	"WaitForRepeaterReady",
	"CheckForRepeaterReady",
	"RepeaterReady",
	"LinkAuthenticated",		/* Part 2 Completed; Include down stream devices */
	"EncryptionEnabled",		/* Part 3 Ri Link Integrity Checks Match */
	"RepeaterAuthenticationFailure",
	"RiLinkIntegrityFailure",
	"PjLinkIntegrityFailure"
} ;
#endif

static void BHDCPlib_P_ShowStateChange(BHDCPlib_Handle hHDCPlib, BHDCPlib_State currentState) 
{
	if (currentState != hHDCPlib->stHdcpStatus.eAuthenticationState) 
	{
		BDBG_MSG(("HDCP State Change: %s (%d) --> %s (%d)", 
			BHDCPlib_StateText[currentState], currentState,
			BHDCPlib_StateText[hHDCPlib->stHdcpStatus.eAuthenticationState],		
			hHDCPlib->stHdcpStatus.eAuthenticationState)) ;
	}
	return ;
}
	

static BERR_Code BHDCPlib_P_CheckRepeaterReady(BHDCPlib_Handle hHDCPlib)
{
	BERR_Code rc = BERR_SUCCESS;
	uint16_t msElapsedTime;
	BHDCPlib_State currentState = hHDCPlib->stHdcpStatus.eAuthenticationState;
	
	BDBG_ENTER(BHDCPlib_P_CheckRepeaterReady);
	BDBG_ASSERT(hHDCPlib);


	rc = BHDCPlib_GetReceiverInfo(hHDCPlib, &hHDCPlib->stHdcpConfiguration.RxInfo);
	if (rc != BERR_SUCCESS)
		goto done;

	if (hHDCPlib->stHdcpConfiguration.RxInfo.uiRxBCaps & BHDM_HDCP_RxCaps_eKsvFifoReady) 
	{
		hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eRepeaterReady;
		goto done;		
	}	

	/* Calculate elapsed time */
	msElapsedTime = hHDCPlib->uiKsvFifoReadyCount
		* hHDCPlib->stHdcpConfiguration.msIntervalKsvFifoReadyCheck;

	/* the minimum wait time for KSV FIFO Ready is 5seconds */
	if (msElapsedTime >= 5000 + 
		hHDCPlib->stHdcpConfiguration.msWaitForKsvFifoMargin)
	{
		BDBG_ERR(("HDCP Auth Failure; Repeater KSV FIFO not ready")) ;
		hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eRepeaterAuthenticationFailure;
		rc = BHDM_HDCP_REPEATER_FIFO_NOT_READY ;
		goto done;
		
	}
	BDBG_WRN(("Wait for KSV FIFO Ready; timeout in %d ms", (5000+
				hHDCPlib->stHdcpConfiguration.msWaitForKsvFifoMargin - msElapsedTime)));
	hHDCPlib->uiKsvFifoReadyCount++;

	/* Update HDCP Authentication state */
	hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eWaitForRepeaterReady;

	/* Restart periodic timer for msIntervalKsvFifoReadyCheck */
	hHDCPlib->bRepeaterReadyTimerExpired = false;
	BTMR_StopTimer(hHDCPlib->hTimer);
	BTMR_StartTimer(hHDCPlib->hTimer, hHDCPlib->stHdcpConfiguration.msIntervalKsvFifoReadyCheck * 1000) ;

done:	
	
	/* Clear authentication if error */
	if (rc != BERR_SUCCESS)
		BHDM_HDCP_ClearAuthentication(hHDCPlib->stDependencies.hHdm);

	BHDCPlib_P_ShowStateChange(hHDCPlib, currentState) ;
	
	BDBG_LEAVE(BHDCPlib_P_CheckRepeaterReady);
	return rc;
	
}


static void BHDCPlib_P_TimerExpiration_isr (BHDCPlib_Handle hHDCPlib, int parm2) 
{
	BDBG_ENTER(BHDCPlib_P_TimerExpiration_isr);
	BDBG_ASSERT(hHDCPlib);
	BSTD_UNUSED(parm2);

	switch (hHDCPlib->stHdcpStatus.eAuthenticationState)
	{
	case BHDCPlib_State_eWaitForReceiverAuthentication :
		hHDCPlib->bR0ReadyTimerExpired = true;
		break ;

	case BHDCPlib_State_eWaitForRepeaterReady:
		hHDCPlib->bRepeaterReadyTimerExpired = true;
		break;
		
	default :
		BDBG_WRN(("Unknown Timer <%d> expiration...",
			hHDCPlib->stHdcpStatus.eAuthenticationState)) ;
	}
	
	BDBG_LEAVE(BHDCPlib_P_TimerExpiration_isr);
}


BERR_Code BHDCPlib_Open(BHDCPlib_Handle *hHDCPlib, const BHDCPlib_Dependencies *pstDependencies)
{
	BERR_Code rc = BERR_SUCCESS;
	BHDCPlib_Handle hHandle;


	BDBG_ENTER(BHDCPlib_Open);
	BDBG_ASSERT(pstDependencies->hHdm);
	BDBG_ASSERT(pstDependencies->hHsm);
	BDBG_ASSERT(pstDependencies->hTmr);

	/* Alloc memory from the system */
	hHandle = BKNI_Malloc(sizeof(BHDCPlib_P_Handle));
	if (hHandle == NULL)
	{
		rc = BERR_OUT_OF_SYSTEM_MEMORY;
		BDBG_ERR(("BHDCPlib_Open: BKNI_malloc() failed\n"));
		goto done;
	}		
	

#if 0
	if (!pstDependentData->pfHdcpGetRevokedKsvsCallback)
	{
		BDBG_ERR(("Missing required callback functions"));
		rc = BERR_INVALID_PARAMETER;
		goto done ;
	}
#endif

	BKNI_Memcpy(&hHandle->stDependencies, pstDependencies, sizeof(BHDCPlib_Dependencies)) ;
	hHandle->hTmr = pstDependencies->hTmr ;
		
    hHandle->stTmrSettings.type =  BTMR_Type_eCountDown ;
	hHandle->stTmrSettings.cb_isr = (BTMR_CallbackFunc) BHDCPlib_P_TimerExpiration_isr ;
	hHandle->stTmrSettings.pParm1 = hHandle;
	hHandle->stTmrSettings.parm2 = 0 ;
	hHandle->stTmrSettings.exclusive = false ;

	rc = BTMR_CreateTimer(hHandle->hTmr, &hHandle->hTimer, &hHandle->stTmrSettings) ;
	if(rc != BERR_SUCCESS)
	{
		rc = BERR_TRACE(BERR_LEAKED_RESOURCE);
		goto done ;
	}

	/* Initialize HDCP authentication state, error status */
	hHandle->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eUnauthenticated;
	hHandle->stHdcpStatus.eHdcpError = BHDCPlib_HdcpError_eSuccess;

	/* Set default HDCP configuration information*/
	BHDCPlib_GetDefaultConfiguration(&hHandle->stHdcpConfiguration);

	/* Initialize revoked KSV list */
	hHandle->RevokedKsvList.Ksvs = NULL;
	hHandle->RevokedKsvList.uiNumRevokedKsvs = 0;
	
done:
	if (rc != BERR_SUCCESS)
	{
		if (!hHandle)
		{
			BKNI_Free(hHandle);
			hHandle=NULL;
		}
	}
	
	*hHDCPlib = hHandle;
	
	BDBG_LEAVE(BHDCPlib_Open);
	return rc ;
	
}


BERR_Code BHDCPlib_Close(BHDCPlib_Handle hHDCPlib)
{
	BERR_Code rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDCPlib_Close);
	BDBG_ASSERT(hHDCPlib);

	rc = BTMR_DestroyTimer(hHDCPlib->hTimer);
	if (rc != BERR_SUCCESS)
		BDBG_ERR(("Error destroying timer"));


	/* free memory associated with the Revoked KSV list, if any */
	if (hHDCPlib->RevokedKsvList.Ksvs)
	{
		BKNI_Free(hHDCPlib->RevokedKsvList.Ksvs);
		hHDCPlib->RevokedKsvList.Ksvs = NULL;
		hHDCPlib->RevokedKsvList.uiNumRevokedKsvs = 0;
	}
	
	/* free memory associated with the HDMIlib Handle */
	BKNI_Memset(hHDCPlib, 0, sizeof(BHDCPlib_P_Handle)) ;
	BKNI_Free( (void *) hHDCPlib) ;

	
	BDBG_LEAVE(BHDCPlib_Close);
	return rc;
}


BERR_Code BHDCPlib_GetDefaultDependencies(BHDCPlib_Dependencies *pDefaultDependencies)
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER(BHDCPlib_GetDefaultDependencies);

	/* Set all Dependencies to NULL */
	BKNI_Memset(pDefaultDependencies, 0, sizeof(*pDefaultDependencies)); 

	BDBG_LEAVE(BHDCPlib_GetDefaultDependencies);
	return rc;
}


BERR_Code BHDCPlib_InitializeReceiverAuthentication(BHDCPlib_Handle hHDCPlib)
{
	BERR_Code   rc = BERR_SUCCESS;
	uint8_t ucDeviceAttached=0;
	
	BDBG_ENTER(BHDCPlib_InitializeReceiverAuthentication) ;
	BDBG_ASSERT(hHDCPlib);
	BDBG_ASSERT(hHDCPlib->stDependencies.hHdm) ;	


	rc = BHDM_RxDeviceAttached(hHDCPlib->stDependencies.hHdm, &ucDeviceAttached) ;
	if (!ucDeviceAttached)
	{
		hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eUnPowered;
		goto done;
	}

	/* get revoked list */
	if (!hHDCPlib->stDependencies.pfHdcpGetRevokedKsvsCallback)
	{
		BDBG_WRN(("No Revoked KSV Callback Installed")) ;
	}
	else
	{
		BHDCPlib_RevokedKsvList tmpRevokedKSVs;
		
		hHDCPlib->stDependencies.pfHdcpGetRevokedKsvsCallback(
			hHDCPlib->stDependencies.pSysDevice, 
			hHDCPlib->stDependencies.iGetRevokedKsvParam2,
			&tmpRevokedKSVs) ;

		BHDCPlib_SetRevokedKSVs(hHDCPlib, &tmpRevokedKSVs);
	}

	/* Retrieve Receiver information before starting the authentication process */
	rc = BHDCPlib_GetReceiverInfo(hHDCPlib, &hHDCPlib->stHdcpConfiguration.RxInfo);
	if (rc != BERR_SUCCESS)
	{
		BDBG_ERR(("Error getting receiver info"));
		goto done;
	}

	/* Read the Bksv from the Rx,  compare revoked list against retrieved Rx information */
	rc = BHDM_HDCP_ReadRxBksv(hHDCPlib->stDependencies.hHdm, 
		(const uint8_t *) hHDCPlib->RevokedKsvList.Ksvs, 
		hHDCPlib->RevokedKsvList.uiNumRevokedKsvs);
	if (rc != BERR_SUCCESS) 
		goto done;


	/* Update the RxBksv in BHDCPlib_Configuration */
	rc = BHDM_HDCP_GetRxKsv(hHDCPlib->stDependencies.hHdm, (uint8_t *) hHDCPlib->stHdcpConfiguration.RxInfo.RxBksv);
	if (rc != BERR_SUCCESS) 
		goto done;
	

	/* Generate/Write the Authentication An value */
	rc = BHDM_HDCP_GenerateAn(hHDCPlib->stDependencies.hHdm, hHDCPlib->stHdcpConfiguration.eAnSelection) ;
	if (rc != BERR_SUCCESS)
		goto done;	


	/* Write the Tx Aksv to the Receiver */
	rc = BHDM_HDCP_WriteTxAksvToRx(hHDCPlib->stDependencies.hHdm, hHDCPlib->stHdcpConfiguration.TxKeySet.TxAksv);
	if (rc != BERR_SUCCESS)
		goto done;


	/* load the transmitter HDCP Keys */
	rc = BHDM_HDCP_EnableSerialKeyLoad(hHDCPlib->stDependencies.hHdm);
	if (rc != BERR_SUCCESS) 
		goto done;


	rc = BHDCPlib_FastLoadEncryptedHdcpKeys(hHDCPlib);
	if (rc != BERR_SUCCESS)
		goto done;


	/* Initiate timer countdown for 100ms + msWaitForR0Margin before setting Receiver Ready */
	hHDCPlib->bR0ReadyTimerExpired = false;
	BTMR_StopTimer(hHDCPlib->hTimer);
	BTMR_StartTimer(hHDCPlib->hTimer, (100+hHDCPlib->stHdcpConfiguration.msWaitForRxR0Margin) * 1000) ;

	/* Update HDCP authentication state */
	hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eWaitForReceiverAuthentication;
	
done:

	if (rc != BERR_SUCCESS)
		hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eUnauthenticated;

	BDBG_LEAVE(BHDCPlib_InitializeReceiverAuthentication) ;
	return rc ;
	
}



BERR_Code BHDCPlib_AuthenticateReceiver(BHDCPlib_Handle hHDCPlib)
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER(BHDCPlib_AuthenticateReceiver) ;
	BDBG_ASSERT(hHDCPlib);


	/* Authenticate the Link  */
	rc = BHDM_HDCP_AuthenticateLink(hHDCPlib->stDependencies.hHdm) ;
	if ( rc != BERR_SUCCESS)  
	{
		hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eR0LinkFailure;
		goto done;
	}

	hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eReceiverAuthenticated;

	rc = BHDM_HDCP_XmitEncrypted(hHDCPlib->stDependencies.hHdm);
	if (rc != BERR_SUCCESS)
		goto done;
	
done:
	
	BDBG_LEAVE(BHDCPlib_AuthenticateReceiver) ;
	return rc;
	
}



BERR_Code BHDCPlib_InitializeRepeaterAuthentication(BHDCPlib_Handle hHDCPlib)
{
	BERR_Code rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDCPlib_InitializeRepeaterAuthentication) ;
	BDBG_ASSERT(hHDCPlib);

	/* Set default configuration	*/
	hHDCPlib->uiKsvFifoReadyCount = 1;

	/* Enable/Initiate periodic timer once every msIntervalKsvFifoReadyCheck */
	hHDCPlib->bRepeaterReadyTimerExpired = false;
	BTMR_StopTimer(hHDCPlib->hTimer);
	BTMR_StartTimer(hHDCPlib->hTimer, hHDCPlib->stHdcpConfiguration.msIntervalKsvFifoReadyCheck * 1000) ;

	/* Update state machine	*/
	hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eWaitForRepeaterReady;

	BDBG_LEAVE(BHDCPlib_InitializeRepeaterAuthentication) ;
	return rc ;
}



BERR_Code BHDCPlib_AuthenticateRepeater(BHDCPlib_Handle hHDCPlib)
{
	BERR_Code rc = BERR_SUCCESS;
	uint16_t BStatus;
	uint8_t RepeaterLevels;
	uint8_t DeviceCount;
	uint16_t uiNumKsvBytes;
	uint8_t KsvListMemoryAllocated = 0 ;
	uint8_t *KsvFifoList = NULL ;

	BDBG_ENTER(BHDCPlib_AuthenticateRepeater) ;
	BDBG_ASSERT(hHDCPlib);
	

	/* check Repeater Values */
	BHDCPLIB_CHECK_RC(rc, 
		BHDM_HDCP_GetRxStatus(hHDCPlib->stDependencies.hHdm, &BStatus)) ;
	BHDCPLIB_CHECK_RC(rc, 
		BHDM_HDCP_GetRepeaterDepth(hHDCPlib->stDependencies.hHdm, &RepeaterLevels)) ;
	BHDCPLIB_CHECK_RC(rc, 
		BHDM_HDCP_GetRepeaterDeviceCount(hHDCPlib->stDependencies.hHdm, &DeviceCount)) ;

	BDBG_MSG(("RXStatus: %X, Depth: %d, Devices: %d",
		BStatus, RepeaterLevels, DeviceCount)) ;
	
	
	/* check if the number of repeater levels has been exceeded */
	if (BStatus & BHDM_HDCP_RxStatus_eMaxRepeatersExceeded)
	{
		BDBG_ERR(("%d Levels of Repeaters exceed the MAX allowed of %d", 
			RepeaterLevels, BHDM_HDCP_REPEATER_MAX_DEPTH )) ;
		rc = BHDM_HDCP_REPEATER_DEPTH_EXCEEDED ;
		goto done;
	}

	/* check if the number of receiver devices has been exceeded */
	if (BStatus & BHDM_HDCP_RxStatus_eMaxDevicesExceeded) 
	{
		BDBG_ERR(("Number of Devices: %d exceeds the MAX allowed of %d", 
			DeviceCount, BHDM_HDCP_REPEATER_MAX_DEVICE_COUNT )) ;
		rc = BHDM_HDCP_RX_DEVICES_EXCEEDED ;
		goto done ;
	}

	/* initialize the Repeater Authentication */
	BHDM_HDCP_InitializeRepeaterAuthentication(hHDCPlib->stDependencies.hHdm);

	if (DeviceCount)
	{
		/* allocate a buffer to hold the Ksv List */
		uiNumKsvBytes = (uint16_t) (DeviceCount * BHDM_HDCP_KSV_LENGTH) ;
		KsvFifoList = (uint8_t *) BKNI_Malloc(sizeof(uint8_t) * uiNumKsvBytes) ;
		KsvListMemoryAllocated = 1 ;

		/* read the Ksv List */
		/* pass the revoked list for checking against downstream Rx devices */
		BHDCPLIB_CHECK_RC(rc, 
			BHDM_HDCP_ReadRxRepeaterKsvFIFO(hHDCPlib->stDependencies.hHdm, 
				KsvFifoList, DeviceCount, 
				(uint8_t *) &hHDCPlib->RevokedKsvList.Ksvs, 
				(uint16_t) hHDCPlib->RevokedKsvList.uiNumRevokedKsvs)) ;

		/* write the Ksvs from the Rx (Repeater) to the Transmitter core for verification */
		BHDCPLIB_CHECK_RC(rc, 
			BHDM_HDCP_WriteTxKsvFIFO(hHDCPlib->stDependencies.hHdm, 
				KsvFifoList, DeviceCount)) ;
	
	}
	else	/* handle zero devices attached to repeater */
	{
#if BHDM_CONFIG_DISABLE_HDCP_AUTH_REPEATER_DEVCOUNT0
		/* do not allow authentication with repeaters that have device count of 0 */
		BDBG_WRN(("Auth Disabled for Repeaters with Device Count of 0")) ;
		rc = BHDM_HDCP_REPEATER_DEVCOUNT_0 ;
		goto done;
#else
		/* force V calculation for repeater with zero attached devices */
		BHDM_HDCP_ForceVCalculation(hHDCPlib->stDependencies.hHdm) ;
#endif		
	
	}

	/* check SHA-1 Hash Verification (V).... */
	BHDCPLIB_CHECK_RC(rc, 
		BHDM_HDCP_RepeaterAuthenticateLink(hHDCPlib->stDependencies.hHdm, 
			&hHDCPlib->stHdcpConfiguration.RxInfo.uiIsAuthenticated)) ;
	
	if (!hHDCPlib->stHdcpConfiguration.RxInfo.uiIsAuthenticated)
	{
		BDBG_ERR(("Repeater failed to authenticate")) ;	
		rc = BHDM_HDCP_REPEATER_AUTH_ERROR ;
		goto done ;
	}
	
	hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eLinkAuthenticated;		
	
done:
	/* release allocated memory for Ksv list */
	if (KsvListMemoryAllocated)
		BKNI_Free(KsvFifoList) ;

	if (rc != BERR_SUCCESS)
		hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eRepeaterAuthenticationFailure;
		
	BDBG_LEAVE(BHDCPlib_AuthenticateRepeater) ;
	return rc ;
}


BERR_Code BHDCPlib_TransmitEncrypted(BHDCPlib_Handle hHDCPlib)
{
	BERR_Code rc = BERR_SUCCESS;
	BHDCPlib_State currentState = hHDCPlib->stHdcpStatus.eAuthenticationState;

	BDBG_ENTER(BHDCPlib_TransmitEncrypted);
	BDBG_ASSERT(hHDCPlib);
	
	rc = BHDM_HDCP_XmitEncrypted(hHDCPlib->stDependencies.hHdm);
	if (rc != BERR_SUCCESS)
		goto done;

	hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eEncryptionEnabled;
	BHDCPlib_P_ShowStateChange(hHDCPlib, currentState) ;
	
done:	

	BDBG_LEAVE(BHDCPlib_TransmitEncrypted);	
	return rc;
}

BERR_Code BHDCPlib_TransmitClear(BHDCPlib_Handle hHDCPlib)
{
	BERR_Code rc = BERR_SUCCESS;
	BHDCPlib_State currentState = hHDCPlib->stHdcpStatus.eAuthenticationState;

	BDBG_ENTER(BHDCPlib_TransmitClear);
	BDBG_ASSERT(hHDCPlib);
	
	rc = BHDM_HDCP_XmitClear(hHDCPlib->stDependencies.hHdm);
	if (rc != BERR_SUCCESS)
		goto done;

	hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eUnauthenticated;	

	BHDCPlib_P_ShowStateChange(hHDCPlib, currentState) ;
	
done:	

	BDBG_LEAVE(BHDCPlib_TransmitClear);	
	return rc;
}

void BHDCPlib_ProcessEvent(BHDCPlib_Handle hHDCPlib, BHDCPlib_Event *stHdmiEvent)
{

	BERR_Code rc ;
	uint8_t ucRxSense;
	uint8_t ucDeviceAttached;
	uint8_t ucErrorDetected = 0 ; 
	BHDCPlib_State currentState = hHDCPlib->stHdcpStatus.eAuthenticationState;

	BDBG_ENTER(BHDCPlib_ProcessEvent);
	
	switch (stHdmiEvent->event)
	{
	case BHDM_EventHDCPRiValue:
		if (BHDM_HDCP_RiLinkIntegrityCheck(hHDCPlib->stDependencies.hHdm) != BERR_SUCCESS)
		{
			hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eRiLinkIntegrityFailure;
			ucErrorDetected = 1 ;
		}

		break;
		
	case BHDM_EventHDCPPjValue:
		if (BHDM_HDCP_PjLinkIntegrityCheck(hHDCPlib->stDependencies.hHdm) != BERR_SUCCESS) 
		{
			hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_ePjLinkIntegrityFailure;
			ucErrorDetected = 1 ;
		}
	
		break;
		
	case BHDM_EventHDCPRepeater:
		hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eRepeaterAuthenticationFailure;
		ucErrorDetected = 1 ;
		break;
		
	case BHDM_EventHotPlug:
		rc = BHDM_RxDeviceAttached(hHDCPlib->stDependencies.hHdm, &ucDeviceAttached) ;

		if (!ucDeviceAttached)
			hHDCPlib->stHdcpStatus.eAuthenticationState =
				BHDCPlib_State_eUnPowered;
		else
			hHDCPlib->stHdcpStatus.eAuthenticationState = 
				BHDCPlib_State_eUnauthenticated;

		break;
		
	default:
		BDBG_WRN(("Invalid Event")) ;
		break;
	}

	if (ucErrorDetected)
	{
		rc = BHDM_GetReceiverSense(hHDCPlib->stDependencies.hHdm, &ucRxSense);
		if (!ucRxSense)
		{
			BDBG_WRN(("HdcpLibState: (%d) '%s' generated due to Rx powered off", 
				hHDCPlib->stHdcpStatus.eAuthenticationState,
				BHDCPlib_StateText[hHDCPlib->stHdcpStatus.eAuthenticationState]	)) ;
			hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eUnPowered;
		}
	}

	BHDCPlib_P_ShowStateChange(hHDCPlib, currentState) ;
	
	BDBG_LEAVE(BHDCPlib_ProcessEvent);
	return;
}


BERR_Code BHDCPlib_SetRevokedKSVs(BHDCPlib_Handle hHDCPlib, BHDCPlib_RevokedKsvList *stKsvList)
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER(BHDCPlib_SetRevokedKSVs);
	BDBG_ASSERT(hHDCPlib);


	if (hHDCPlib->RevokedKsvList.Ksvs)
	{
		BKNI_Free(hHDCPlib->RevokedKsvList.Ksvs);
		hHDCPlib->RevokedKsvList.Ksvs = NULL;
		hHDCPlib->RevokedKsvList.uiNumRevokedKsvs = 0;
	}
	
	if (!stKsvList->uiNumRevokedKsvs)
	{
		BDBG_WRN(("No Revoked KSVs specified."));
		goto done;
	}
	
	/* Store the Revoked KSVs in the handle */
	hHDCPlib->RevokedKsvList.Ksvs = 
		BKNI_Malloc(sizeof(uint8_t) * stKsvList->uiNumRevokedKsvs * BAVC_HDMI_HDCP_KSV_LENGTH);
	
	BKNI_Memcpy(hHDCPlib->RevokedKsvList.Ksvs, stKsvList->Ksvs, 
					(sizeof(uint8_t) * stKsvList->uiNumRevokedKsvs * BAVC_HDMI_HDCP_KSV_LENGTH));
	
	hHDCPlib->RevokedKsvList.uiNumRevokedKsvs = stKsvList->uiNumRevokedKsvs;

done:
	BDBG_LEAVE(BHDCPlib_SetRevokedKSVs);
	return rc;
}


BERR_Code BHDCPlib_SetConfiguration(BHDCPlib_Handle hHDCPlib, BHDCPlib_Configuration * stHdcpConfiguration)
{
	BDBG_ENTER(BHDCPlib_SetConfiguration);
	BDBG_ASSERT(hHDCPlib);

	
	/* Copy Hdcp Link data into HDMlib handle 	*/
	BKNI_Memcpy(&hHDCPlib->stHdcpConfiguration, stHdcpConfiguration, sizeof(BHDCPlib_Configuration));


	BDBG_LEAVE(BHDCPlib_SetConfiguration);
	return BERR_SUCCESS;
}

BERR_Code BHDCPlib_GetDefaultConfiguration(BHDCPlib_Configuration *stHdcpConfiguration)
{
	BDBG_ENTER(BHDCPlib_GetDefaultConfiguration);
	BDBG_ASSERT(stHdcpConfiguration);

	/* Copy default Hdcp configuration */
	BKNI_Memcpy(stHdcpConfiguration, &stHdcpDefaultConfiguration, sizeof(BHDCPlib_Configuration));

	BDBG_LEAVE(BHDCPlib_GetDefaultConfiguration);
	return BERR_SUCCESS;
}

BERR_Code BHDCPlib_GetConfiguration(BHDCPlib_Handle hHDCPlib, BHDCPlib_Configuration *stHdcpConfiguration)
{
	BDBG_ENTER(BHDCPlib_GetConfiguration);
	BDBG_ASSERT(hHDCPlib);


	/* Copy Hdcp Link data from HDCPlib handle */
	BKNI_Memcpy(stHdcpConfiguration, &hHDCPlib->stHdcpConfiguration, sizeof(BHDCPlib_Configuration));


	BDBG_LEAVE(BHDCPlib_GetConfiguration);
	return BERR_SUCCESS;
}


BERR_Code BHDCPlib_GetReceiverInfo(BHDCPlib_Handle hHDCPlib, BHDCPlib_RxInfo *stRxHdcpInfo)
{
	BERR_Code rc = BERR_SUCCESS;
	
	BDBG_ENTER(BHDCPlib_GetReceiverInfo);
	BDBG_ASSERT(hHDCPlib);


	/* Retrieve RxBCaps */
	rc = BHDM_HDCP_GetRxCaps(hHDCPlib->stDependencies.hHdm, 
				&stRxHdcpInfo->uiRxBCaps) ;
	if (rc != BERR_SUCCESS)
	{
		BDBG_ERR(("Error getting Rx BCaps"));
		goto done;
	}

	/* Retrieve the RxBksv */
	rc = BHDM_HDCP_GetRxKsv(hHDCPlib->stDependencies.hHdm, 
				(uint8_t *) stRxHdcpInfo->RxBksv);
	if (rc != BERR_SUCCESS) 
	{
		BDBG_ERR(("Error getting Rx BCaps"));	
		goto done;
	}

	/* Check for Repeater */
	stRxHdcpInfo->bIsHdcpRepeater = 
		stRxHdcpInfo->uiRxBCaps & BHDM_HDCP_RxCaps_eHdcpRepeater ;
	
done:

	BDBG_LEAVE(BHDCPlib_GetReceiverInfo);
	return rc;
	
}


BERR_Code BHDCPlib_GetAuthenticationState(BHDCPlib_Handle hHDCPlib, BHDCPlib_State *eAuthenticationState)
{
	BDBG_ENTER(BHDCPlib_GetAuthenticationState);
	BDBG_ASSERT(hHDCPlib);

	/* Retrieve the current HDCP authentication state */
	*eAuthenticationState = hHDCPlib->stHdcpStatus.eAuthenticationState;

	BDBG_LEAVE(BHDCPlib_GetAuthenticationState);
	return BERR_SUCCESS;
}


BERR_Code BHDCPlib_GetHdcpStatus(BHDCPlib_Handle hHDCPlib, BHDCPlib_Status *stHdcpStatus)
{
	BDBG_ENTER(BHDCPlib_GetHdcpStatus);
	BDBG_ASSERT(hHDCPlib);

	/* Retrieve the current HDCP status  */
	BKNI_Memcpy(stHdcpStatus, &hHDCPlib->stHdcpStatus, sizeof(BHDCPlib_Status));


	BDBG_LEAVE(BHDCPlib_GetHdcpStatus);
	return BERR_SUCCESS;
}


bool BHDCPlib_LinkReadyForEncryption(BHDCPlib_Handle hHDCPlib)
{
	bool brc = false;

	BDBG_ENTER(BHDCPlib_LinkReadyForEncryption);
	BDBG_ASSERT(hHDCPlib);
	
	if (hHDCPlib->stHdcpStatus.eAuthenticationState == BHDCPlib_State_eLinkAuthenticated)
		brc = true;


	BDBG_LEAVE(BHDCPlib_LinkReadyForEncryption);
	return brc;
}


BERR_Code BHDCPlib_StartAuthentication(BHDCPlib_Handle hHDCPlib)
{
	BHDCPlib_State currentState = hHDCPlib->stHdcpStatus.eAuthenticationState;

	BDBG_ENTER(BHDCPlib_StartAuthentication);
	BDBG_ASSERT(hHDCPlib);

	/* Initialize HDCP Authentication state */
	hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eWaitForValidVideo;

	BHDCPlib_P_ShowStateChange(hHDCPlib, currentState) ;

	BDBG_LEAVE(BHDCPlib_StartAuthentication);
	return BERR_SUCCESS;
}


/******************************

Possible return errors:

BHDM_NO_RX_DEVICE
BHDM_HDCP_RX_BKSV_ERROR
BHDM_HDCP_RX_BKSV_REVOKED
BHDM_HDCP_TX_AKSV_ERROR

BHDM_HDCP_AUTH_ABORTED
BHDM_HDCP_AUTHENTICATE_ERROR		
BHDM_HDCP_LINK_RI_FAILURE
BHDM_HDCP_LINK_PJ_FAILURE	

BHDM_HDCP_REPEATER_FIFO_NOT_READY
BHDM_HDCP_REPEATER_DEVCOUNT_0
BHDM_HDCP_REPEATER_DEPTH_EXCEEDED
BHDM_HDCP_RX_DEVICES_EXCEEDED


TBD: 

BHDM_HDCP_RX_NO_HDCP_SUPPORT
BHDM_HDCP_RECEIVER_AUTH_ERROR 
BHDM_HDCP_REPEATER_AUTH_ERROR 
BHDM_HDCP_NO_AUTHENTICATED_LINK 

**********************************/ 
BERR_Code BHDCPlib_ProcessAuthentication(BHDCPlib_Handle hHDCPlib, BHDCPlib_Status *stHdcpStatus)
{
	BERR_Code rc = BERR_SUCCESS;
	uint8_t i;
	bool bErrFound = false;
	BHDCPlib_State currentState = hHDCPlib->stHdcpStatus.eAuthenticationState;

	BDBG_ENTER(BHDCPlib_ProcessAuthentication);
	BDBG_ASSERT(hHDCPlib);


	/* Always recommend a delay of 1 ms between states unless specified below */
	hHDCPlib->stHdcpStatus.msRecommendedWaitTime = 1;
	
	switch (hHDCPlib->stHdcpStatus.eAuthenticationState)
	{
	case BHDCPlib_State_eUnPowered:
		hHDCPlib->stHdcpStatus.msRecommendedWaitTime = 500 ;
		break;

	case BHDCPlib_State_eUnauthenticated:
		hHDCPlib->stHdcpStatus.msRecommendedWaitTime = 100;
		break;

	case BHDCPlib_State_eWaitForValidVideo:
		if (BHDM_CheckForValidVideo(hHDCPlib->stDependencies.hHdm)) 
		{
			hHDCPlib->stHdcpStatus.msRecommendedWaitTime = hHDCPlib->stHdcpConfiguration.msWaitForValidVideo;
		}
		else {
			hHDCPlib->stHdcpStatus.eAuthenticationState = 
					BHDCPlib_State_eInitializeAuthentication;
		}
		break;

	case BHDCPlib_State_eInitializeAuthentication:
		/* Attempting first part of authentication */
		rc = BHDCPlib_InitializeReceiverAuthentication(hHDCPlib);
		if (rc != BERR_SUCCESS)
		{
			BDBG_ERR(("first part of the authentication process failed")) ;
			goto done;
		}		
		break;

	case BHDCPlib_State_eWaitForReceiverAuthentication:
		if (hHDCPlib->bR0ReadyTimerExpired)
			hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eReceiverR0Ready;
		else
			hHDCPlib->stHdcpStatus.msRecommendedWaitTime = 100 + hHDCPlib->stHdcpConfiguration.msWaitForRxR0Margin;
		
		break;

	case BHDCPlib_State_eReceiverR0Ready:
		rc = BHDCPlib_AuthenticateReceiver(hHDCPlib);
		if (rc != BERR_SUCCESS)
		{
			BDBG_ERR(("Failed to authenticate receiver"));
			goto done;
		}
		break;

	case BHDCPlib_State_eR0LinkFailure:
		hHDCPlib->stHdcpStatus.msRecommendedWaitTime = 2000;
		break;
		
	case BHDCPlib_State_eReceiverAuthenticated:
		/* Proceed to part 2 of authentication if attached device is a repeater */
		if (hHDCPlib->stHdcpConfiguration.RxInfo.bIsHdcpRepeater)
		{
			rc = BHDCPlib_InitializeRepeaterAuthentication(hHDCPlib);
			if (rc != BERR_SUCCESS)
			{
				BDBG_ERR(("Second part of the authentication process failed"));
				goto done;
			}
		}
		else
			hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eLinkAuthenticated;

		break;
		
	case BHDCPlib_State_eWaitForRepeaterReady:		
		if (hHDCPlib->bRepeaterReadyTimerExpired)
			hHDCPlib->stHdcpStatus.eAuthenticationState = BHDCPlib_State_eCheckForRepeaterReady;
		else
			/* Recommend waiting 100 ms for Repeater Fifo ready */
			hHDCPlib->stHdcpStatus.msRecommendedWaitTime = 100;
		
		break;
	
	case BHDCPlib_State_eCheckForRepeaterReady:
		/* Check for repeater FIFO ready */
		rc = BHDCPlib_P_CheckRepeaterReady(hHDCPlib);
		if (rc != BERR_SUCCESS)
		{
			BDBG_ERR(("Repeater KSV FIFO not ready"));
			goto done;
		}
		break;

	case BHDCPlib_State_eRepeaterReady:
		/* Repeater Ready, start authenticate repeater */
		rc = BHDCPlib_AuthenticateRepeater(hHDCPlib);
		if (rc != BERR_SUCCESS)
		{
			BDBG_ERR(("Failed to authenticate repeater"));
			goto done;
		}
		break;
		
	case BHDCPlib_State_eLinkAuthenticated:
		/* HDCP AUTHENTICATION SUCCESS - link is now authenticated */

		/* Recommend 10ms for checking encryption enabled */
		hHDCPlib->stHdcpStatus.msRecommendedWaitTime = 10;		
		break;


	case BHDCPlib_State_eEncryptionEnabled:
		/* Transmitting Encrypted Video	*/
		hHDCPlib->stHdcpStatus.msRecommendedWaitTime = 2000;
		goto done;		
		break;

	case BHDCPlib_State_eRepeaterAuthenticationFailure:
		/* Repeater Error */
		rc = BHDM_HDCP_LINK_FAILURE;
		break;

	case BHDCPlib_State_eRiLinkIntegrityFailure:
		/* Ri Link Failure */
		rc = BHDM_HDCP_LINK_RI_FAILURE;
		break;

	case BHDCPlib_State_ePjLinkIntegrityFailure:
		/* Pj Link Failure */
		rc = BHDM_HDCP_LINK_PJ_FAILURE ;
		break;
	
	default:
		BDBG_WRN(("Invalid HDCP Authentication state: %d", hHDCPlib->stHdcpStatus.eAuthenticationState)) ;
		break;
			
	}

	
done:
	/* HDCP error */
	for (i=0; i < sizeof(BHDCPlib_SupportedHdcpErrors)/sizeof(BHDCPLIB_P_HDM_HdcpErrors); i++)
	{
		if (rc == BHDCPlib_SupportedHdcpErrors[i].hdmHdcpError)
		{
			hHDCPlib->stHdcpStatus.eHdcpError = BHDCPlib_SupportedHdcpErrors[i].hdcplibHdcpError;
			bErrFound = true;
			break;
		}
	}

	if (!bErrFound)
	{
		BDBG_ERR(("Unable to find matching BHDM_HDCP return code for HDCPlib return code %d", rc));
		BDBG_ERR(("Current Authentication State: %s (%d)", 
			BHDCPlib_StateText[currentState], currentState)) ;

		hHDCPlib->stHdcpStatus.eHdcpError = BHDCPlib_HdcpError_eReceiverAuthenticationError;
	}

	/* Save current hdcp status  */
	BKNI_Memcpy(stHdcpStatus, &hHDCPlib->stHdcpStatus, sizeof(BHDCPlib_Status));

	BHDCPlib_P_ShowStateChange(hHDCPlib, currentState) ;

	BDBG_LEAVE(BHDCPlib_ProcessAuthentication);
	return rc;

}


BERR_Code BHDCPlib_DisableAuthentication(BHDCPlib_Handle hHDCPlib)
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER(BHDCPlib_DisableAuthentication);
	BDBG_ASSERT(hHDCPlib);

	/* clear HDCP Authentication */
	BHDM_HDCP_ClearAuthentication(hHDCPlib->stDependencies.hHdm) ;

	/* Transmit clear data */
	rc = BHDCPlib_TransmitClear(hHDCPlib);
	
	BDBG_LEAVE(BHDCPlib_DisableAuthentication);
	return rc;
}

