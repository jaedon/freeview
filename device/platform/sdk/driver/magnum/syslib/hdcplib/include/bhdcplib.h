/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdcplib.h $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 4/30/12 2:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/hdcplib/7401/bhdcplib.h $
 * 
 * Hydra_Software_Devel/7   4/30/12 2:42p vle
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
 * Hydra_Software_Devel/6   3/3/10 12:37p vle
 * SW7405-3987: Merge into main branch
 * 
 * Hydra_Software_Devel/SW7405-3987/1   3/2/10 5:39p vle
 * SW7405-3987: Report specific HDCP error status to high level
 * application.
 * 
 * Hydra_Software_Devel/5   1/28/10 1:58p vle
 * SW7420-489: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7420-489/1   1/26/10 3:39p vle
 * SW7420-489: propagate max_device_count and max_depth to application
 * 
 * Hydra_Software_Devel/4   8/25/09 3:30p vle
 * SW7405-2670:
 * Do not poll for receiver sense while encryption is still enabled.
 * Add implementation of interrupt based receiver sense
 * 
 * Hydra_Software_Devel/PR56486/1   8/6/09 12:05p vle
 * PR56486: Do not poll for receiver sense while encryption is still
 * enabled.
 * 
 * Hydra_Software_Devel/3   11/24/08 8:37p vle
 * PR49219, PR49593, PR48842, PR48937: Merge to main branch
 * 
 * Hydra_Software_Devel/HDMI_TX_Plugfest11_200811/2   11/24/08 8:27p vle
 * PR49219, PR49593, PR48842: Add additional state in the state machine to
 * clearly identify HDCP authentication errors. Update the current hdcp
 * state appropriately.
 * 
 * Hydra_Software_Devel/HDMI_TX_Plugfest11_200811/1   11/12/08 4:55p vle
 * PR 48937: Add support for HDCP repeater error during HDCP
 * authentication process.
 * 
 * Hydra_Software_Devel/2   3/18/08 7:18p vle
 * PR 39991: Rename HDMIlib to HDCPlib. Remove unused implementation.
 * 
 * Hydra_Software_Devel/1   2/26/08 5:43p jgarrett
 * PR 39991: Renaming new HDMlib to HDCPlib
 * 
 * Hydra_Software_Devel/Syslib_Initiative/5   2/21/08 5:00p vle
 * PR 31924: Change Settings back to Dependencies
 * Remove Hotplug callback
 * Move RevokedKSVList to Handle
 * 
 * Hydra_Software_Devel/Syslib_Initiative/4   2/15/08 10:01p vle
 * PR 31924: Improve HDMI Syslib API from feedback.
 * 
 ***************************************************************************/

#ifndef BHDCPLIB_H__
#define BHDCPLIB_H__

#include "bfmt.h"
#include "bavc_hdmi.h"

#include "bhdm.h"
#include "bhdm_edid.h"
#include "bhdm_hdcp.h"
#include "bhsm.h"
#include "btmr.h"


#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
Summary:
HDCPlib Context Handle
*******************************************************************************/
typedef struct BHDCPlib_P_Handle *BHDCPlib_Handle ;


/******************************************************************************
Summary:
	Encrypted HDCP key settings. 

*******************************************************************************/
typedef struct BHDCPlib_EncryptedHdcpKeyStruct
{
	uint8_t  Alg;
	uint8_t  cusKeyVarL;
	uint8_t  cusKeyVarH;
	uint8_t  cusKeySel;
	uint32_t CaDataLo;
	uint32_t CaDataHi;
	uint32_t TCaDataLo;
	uint32_t TCaDataHi;
	uint32_t HdcpKeyLo;
	uint32_t HdcpKeyHi;
} BHDCPlib_EncryptedHdcpKeyStruct;


/******************************************************************************
Summary:
	HDCP information from attached receiver
	
Description:
	Receiver's HDCP related information is retrieved and save for additional usages
	o RxBksv  -  Receiver BKSV
	o uiIsAuthenticated  -  authentication result
	o uiRxBCaps  -  Rx Capabilities
	o bIsHdcpRepeater  -  Receiver Repeater bit
	
See Also:
	o BHDCPlib_GetReceiverInfo

*******************************************************************************/
typedef struct BHDCPlib_RxInfo
{
	BAVC_HDMI_HDCP_KSV RxBksv;			/* Receiver Bksv Value */
	uint8_t uiIsAuthenticated;			/* flag indicate the authentication result */
	uint8_t uiRxBCaps;					/* Rx Capabilities	*/
	bool bIsHdcpRepeater;				/* flag indicate whether Rx is an HDCP Repeater */
} BHDCPlib_RxInfo;



/******************************************************************************
Summary:
	Transmitter's HDCP key set
	
Description:
	Transmitter's HDCP keys for use in HDCP authentication process
	o TxAksv  -  Transmitter AKSV
	o TxKeyStructure  - HDCP keys
	
See Also:
	o BHDCPlib_GetConfiguration
	o BHDCPlib_SetConfiguration

*******************************************************************************/
typedef struct BHDCPlib_EncryptedTxKeySet
{
	/* Transmitter Information */
	BAVC_HDMI_HDCP_KSV TxAksv;								/* Transmitter Aksv value */
	BHDCPlib_EncryptedHdcpKeyStruct 
			TxKeyStructure[BAVC_HDMI_HDCP_N_PRIVATE_KEYS]; 	/* HDCP private keys */

} BHDCPlib_EncryptedTxKeySet;


/******************************************************************************
Summary:
	List of HDCP Revoked KSVs	
	
Description:
	The latest list of HDCP Revoked KSV to check the attach receiver(s) KSV
	o Ksvs  -  List of KSV
	o uiNumRevokedKsvs  -  number of KSV in the revoked KSV list
	
See Also:
	o BHDCPlib_SetRevokedKSVs

*******************************************************************************/
typedef struct BHDCPlib_RevokedKsvList
{
	BAVC_HDMI_HDCP_KSV *Ksvs; 					/* pointer to Revoked KSV List */
	uint16_t uiNumRevokedKsvs; 					/* number of KSVs in Revoked Ksv List */	
} BHDCPlib_RevokedKsvList ;


/******************************************************************************
Summary:
	HDCP configuration for HDCPlib
	
Description:
	Contains all HDCP configurations/information of both transmitter and receivers
	o eAnSelection  -  An Selection
	o TxKeySet  - Transmitter HDCP key set
	o RxInfo  -  Receiver HDCP information
	o msWaitForR0  -  Wait time before reading R0' (>= 100ms)
	o msIntervalKsvFifoReadyCheck  -  Interval to check for KSV FIFO ready bit
	o uiKsvFifoReadyChecks  -  Number of intervals, start value: 1 (count to 50)
	
See Also:
	o BHDCPlib_GetDefaultConfiguration
	o BHDCPlib_GetConfiguration
	o BHDCPlib_SetConfiguration

*******************************************************************************/
typedef struct BHDCPlib_Configuration
{
	BHDM_HDCP_AnSelect eAnSelection;
	BHDCPlib_EncryptedTxKeySet TxKeySet;
	BHDCPlib_RxInfo RxInfo;
	uint16_t msWaitForValidVideo;				/* Wait time for valid video before starting authentication */
	uint8_t msWaitForRxR0Margin;				/* Wait time to read R0' in addition to 100ms required by the spec */
	uint8_t msIntervalKsvFifoReadyCheck;        /* Interval to check for KSV FIFO ready bit. Default at 100ms */
	uint16_t msWaitForKsvFifoMargin;		/* Additional wait time for KSV FIFO ready; 
														needed for some repeater test equipment */
	uint8_t uiMaxDeviceCount;					/* Max downstream devices support */
	uint8_t uiMaxDepth;							/* Max depth support */
	
} BHDCPlib_Configuration;


/******************************************************************************
Summary:
	Enumerated Type indicating the current HDCP authentication state


See Also:
	o BHDCPlib_GetAuthenticationState
	
*******************************************************************************/
typedef enum BHDCPlib_State		
{
	BHDCPlib_State_eUnPowered,
	BHDCPlib_State_eUnauthenticated,
	BHDCPlib_State_eWaitForValidVideo,
	BHDCPlib_State_eInitializeAuthentication,
	BHDCPlib_State_eWaitForReceiverAuthentication,
	BHDCPlib_State_eReceiverR0Ready,
	BHDCPlib_State_eR0LinkFailure,
	BHDCPlib_State_eReceiverAuthenticated,  /* Part 1 Completed; R0 Match */
	BHDCPlib_State_eWaitForRepeaterReady,
	BHDCPlib_State_eCheckForRepeaterReady,
	BHDCPlib_State_eRepeaterReady,
	BHDCPlib_State_eLinkAuthenticated,      /* Part 2 Completed; Include down stream devices */
	BHDCPlib_State_eEncryptionEnabled,      /* Part 3 Ri Link Integrity Checks Match */
	BHDCPlib_State_eRepeaterAuthenticationFailure,
	BHDCPlib_State_eRiLinkIntegrityFailure,
	BHDCPlib_State_ePjLinkIntegrityFailure
	
} BHDCPlib_State;


/******************************************************************************
Summary:
	Enumerated Type indicating the latest HDCP errors

See Also:
	o BHDCPlib_State	
	o BHDCPlib_GetHdcpStatus
	
*******************************************************************************/
typedef enum BHDCPlib_HdcpError
{
	BHDCPlib_HdcpError_eSuccess,
	BHDCPlib_HdcpError_eRxBksvError,
	BHDCPlib_HdcpError_eRxBksvRevoked,
	BHDCPlib_HdcpError_eRxBksvI2cReadError,
	BHDCPlib_HdcpError_eTxAksvError,
	BHDCPlib_HdcpError_eTxAksvI2cWriteError,
	
	BHDCPlib_HdcpError_eReceiverAuthenticationError,
	BHDCPlib_HdcpError_eRepeaterAuthenticationError,
	BHDCPlib_HdcpError_eRxDevicesExceeded,
	BHDCPlib_HdcpError_eRepeaterDepthExceeded,
	BHDCPlib_HdcpError_eRepeaterFifoNotReady,
	BHDCPlib_HdcpError_eRepeaterDeviceCount0,

	BHDCPlib_HdcpError_eRepeaterLinkFailure,		/* Repeater Error */	
	BHDCPlib_HdcpError_eLinkRiFailure,
	BHDCPlib_HdcpError_eLinkPjFailure,

	BHDCPlib_HdcpError_eFifoUnderflow,
	BHDCPlib_HdcpError_eFifoOverflow,
	BHDCPlib_HdcpError_eMultipleAnRequest,
	
	BHDCPlib_HdcpError_eCount
	
} BHDCPlib_HdcpError;


/******************************************************************************
Summary:
	Current HDCP status 
	
Description:
	The latest HDCP status to report back to the higher application.
	o eAuthenticationState  -  current authentication state
	o msRecommendedWaitTime  -  the recommended wait time for higher application
								before continue with the authentication processs
	
See Also:
	o BHDCPlib_ProcessAuthentication

*******************************************************************************/
typedef struct BHDCPlib_Status
{
	BHDCPlib_State eAuthenticationState;		/* current authentication state */
	BHDCPlib_HdcpError eHdcpError;		/* last Hdcp error */
	uint16_t msRecommendedWaitTime;
} BHDCPlib_Status;



/******************************************************************************
Summary:
	Callback handles retrieves latest revoked KSVs
		
See Also:
*******************************************************************************/
typedef void (* BHDCPlib_CallBack_GetRevokedKsvs)
	( void *pvParam1, int iParam2,  BHDCPlib_RevokedKsvList *pvRevokedKsvList );


/******************************************************************************
Summary:
	HDCPlib events
	
Description:
	HDCPlib process RI/PJ change event and hotplug event. 
	o event  -  event type  
	
See Also:
	o BHDCPlib_ProcessEvent

*******************************************************************************/
typedef struct BHDCPlib_Event
{
	BHDM_EventType event ;
} BHDCPlib_Event ;


/******************************************************************************
Summary:
	HDCPlib dependencies data 
	
Description:
	Dependencies data require for BHDCPlib_Open()
	o hHdm  -  HDMI Core handle
	o hHsm  -  HSM handle
	o hTme  -  TMR handle
	o Revoke KSV Callback.
	
See Also:
	o BHDCPlib_Open
	o BHDCPlib_GetDefaultDependencies

*******************************************************************************/
typedef struct BHDCPlib_Dependencies
{
	BHDM_Handle hHdm;	  			/* HDMI handle */
	BHSM_Handle hHsm;
	BTMR_Handle hTmr ;

	BHDCPlib_CallBack_GetRevokedKsvs pfHdcpGetRevokedKsvsCallback;
	void *pSysDevice;
	int iGetRevokedKsvParam2;
	
} BHDCPlib_Dependencies;


/****************************************** HDMI ************************************/
/*************
Summary: Open the HDMIlib handle
**************/
BERR_Code BHDCPlib_Open(BHDCPlib_Handle *hHDCPlib, const BHDCPlib_Dependencies *pstDependencies);


/****************
Summary: Close the HDMIlib handle
****************/
BERR_Code BHDCPlib_Close(BHDCPlib_Handle hHDCPlib);


/****************
Summary: Get the default dependencies
****************/
BERR_Code BHDCPlib_GetDefaultDependencies(BHDCPlib_Dependencies *pDefaultDependencies);


/******************
Summary: HDMI Syslib Event Handler
******************/
void BHDCPlib_ProcessEvent(BHDCPlib_Handle hHDCPlib, BHDCPlib_Event *stHdmiEvent);


/******************************************* HDCP ***********************************/
/******************
Summary: Initialize the  HDCP authentication state machine.
******************/
BERR_Code BHDCPlib_StartAuthentication(BHDCPlib_Handle hHDCPlib);


/******************************************************************************
Summary: 
Enable/Start HDCP authentication state machine.

Description:

Input:
	hHDMI - HDMI control handle that was previously opened by BHDM_Open.

Output:
	stHdcpStatus - HDCP status

Returns:
	BERR_SUCCESS - Successfully closed the HDMI connection.
	BHDM_NO_RX_DEVICE
	BHDM_HDCP_RX_BKSV_ERROR
	BHDM_HDCP_RX_BKSV_REVOKED
	BHDM_HDCP_TX_AKSV_ERROR
	BHDM_HDCP_RX_NO_HDCP_SUPPORT
	BHDM_HDCP_AUTH_ABORTED
	BHDM_HDCP_AUTHENTICATE_ERROR		
	BHDM_HDCP_LINK_RI_FAILURE
	BHDM_HDCP_LINK_PJ_FAILURE	
	BHDM_HDCP_REPEATER_FIFO_NOT_READY
	BHDM_HDCP_REPEATER_DEVCOUNT_0
	BHDM_HDCP_REPEATER_DEPTH_EXCEEDED
	BHDM_HDCP_RX_DEVICES_EXCEEDED

See Also:
	
*******************************************************************************/
BERR_Code BHDCPlib_ProcessAuthentication(BHDCPlib_Handle hHDCPlib, BHDCPlib_Status *stHdcpStatus);


/******************
Summary: Disable/Stop HDCP authentication state machine.
******************/
BERR_Code BHDCPlib_DisableAuthentication(BHDCPlib_Handle hHDCPlib);


/******************
Summary: Retrieve HDCP information of the attached receiver (Bcaps, etc.)
******************/
BERR_Code BHDCPlib_GetReceiverInfo(BHDCPlib_Handle hHDCPlib, 
	BHDCPlib_RxInfo *stRxHdcpInfo);


/******************
Summary: Get default hdcp configurations (keys, receiver info, etc.)
******************/
BERR_Code BHDCPlib_GetDefaultConfiguration(BHDCPlib_Configuration * stHdcpConfiguration);


/******************
Summary: Get hdcp configurations (keys, receiver info, etc.)
******************/
BERR_Code BHDCPlib_GetConfiguration(BHDCPlib_Handle hHDCPlib, BHDCPlib_Configuration *stHdcpConfiguration);


/******************
Summary: Set hdcp configurations (keys, receiver info, etc.)
******************/
BERR_Code BHDCPlib_SetConfiguration(BHDCPlib_Handle hHDCPlib, BHDCPlib_Configuration *stHdcpConfiguration);


/******************
Summary: Set hdcp revoked ksvs 
******************/
BERR_Code BHDCPlib_SetRevokedKSVs(BHDCPlib_Handle hHDCPlib, BHDCPlib_RevokedKsvList *stKsvList);


/******************
Summary: Transmit encrypted video 
******************/
BERR_Code BHDCPlib_TransmitEncrypted(BHDCPlib_Handle hHDCPlib);


/******************
Summary: Transmit clear video
******************/
BERR_Code BHDCPlib_TransmitClear(BHDCPlib_Handle hHDCPlib);


/******************
Summary: Retrieve current authentication state
******************/
BERR_Code BHDCPlib_GetAuthenticationState(BHDCPlib_Handle hHDCPlib, BHDCPlib_State *eAuthenticationState);


/******************
Summary: Retrieve current authentication state
******************/
BERR_Code BHDCPlib_GetHdcpStatus(BHDCPlib_Handle hHDCPlib, BHDCPlib_Status *stHdcpStatus);

/******************
Summary: 
	Check the current status of the HDCP link

Description:
	Returns true if the link is authenticated and ready for encryption.
******************/
bool BHDCPlib_LinkReadyForEncryption(BHDCPlib_Handle hHDCPlib);


#ifdef __cplusplus
}
#endif

#endif /* BHDCPLIB_H__ */

