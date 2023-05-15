/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bhdr_hdcp.h $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 8/28/12 4:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hdr/3548/bhdr_hdcp.h $
 * 
 * Hydra_Software_Devel/12   8/28/12 4:10p vle
 * SW7425-1988: Make sure the HDR PI structure matches with NEXUS
 * structure.
 * 
 * Hydra_Software_Devel/11   7/5/12 1:18p vle
 * SW7425-1988: Merge to mainline
 * 
 * Hydra_Software_Devel/SW7425-1988/3   6/29/12 4:05p vle
 * SW7425-1988: Fix comments
 * 
 * Hydra_Software_Devel/SW7425-1988/2   6/29/12 3:43p vle
 * SW7425-1988: Add device count and KSV to KSV list if attached device is
 * a repeater. This resolve compliance test 3C-II-01 and 3C-II-06.
 * 
 * Hydra_Software_Devel/SW7425-1988/1   6/22/12 5:44p vle
 * SW7425-1988: update the MAX_DEV_EXCEED and MAX_CASCADE_EXCEED error
 * from the hdmi_output to the hdmi_input. This is to resolve the
 * incorrect status update in 7425 repeater test 3C-II-08 and 3C-II-09.
 * The tests still fail due to some other errors.
 * 
 * Hydra_Software_Devel/10   10/31/11 6:14p rgreen
 * SW7425-1351:  Update key loading references to refer to Key RAM vs OTP
 * ROM.
 * 
 * Hydra_Software_Devel/9   4/19/11 5:45p rgreen
 * SW7422-186: Remove unused enums for authenticaton states; report
 * received HDCP values during authentication only
 * 
 * Hydra_Software_Devel/8   4/18/11 1:32p rgreen
 * SW7422-186:  Merge HDMI HDCP Repeater Support
 * 
 * Hydra_Software_Devel/SW7422-186/6   4/18/11 1:28p rgreen
 * SW7422-186: Resolve 35230 compilation issue
 * 
 * Hydra_Software_Devel/7   4/18/11 12:10p rgreen
 * SW7422-186:  Merge HDMI HDCP Repeater Support
 *
 * Hydra_Software_Devel/SW7422-186/5   4/13/11 1:55p rgreen
 * SW7422-186: Update HDCP Status change callback
 * 
 * Hydra_Software_Devel/SW7422-186/4   4/11/11 3:22p rgreen
 * SW7422-186: Add HDCP Repeater support
 * 
 * Hydra_Software_Devel/SW7422-186/2   3/31/11 2:37p rgreen
 * SW7422-186: Add HDCP Repeater support
 * 
 * Hydra_Software_Devel/SW7422-186/1   3/28/11 5:37p rgreen
 * SW7422-186: Add HDCP Repeater Support
 * 
 * Hydra_Software_Devel/6   3/4/11 3:53p rgreen
 * SWDTV-4664,SW7422-129: return status for BHDR_HDCP_Enable/Disable Key
 * loading Move BHDR_HDCP_CheckKeyset functionality to
 * BHDR_HDCP_DisableKeyLoading which will check key load results
 * 
 * Hydra_Software_Devel/5   3/3/11 11:49a rgreen
 * SWDTV-4664: Update HDCP Key Set verification and HDCP processing for
 * OTP ROM based chips
 * 
 * Hydra_Software_Devel/4   2/3/11 8:46p rgreen
 * SW35230-2374,SW7422-129:  Combine OTP ROM and OTP RAM HDCP Key checking
 * APIs
 * 
 * Hydra_Software_Devel/3   12/20/10 4:19p rgreen
 * SW35230-2374: Remove unused state from HDCP KeySet verification state
 * machine
 * 
 * Hydra_Software_Devel/2   12/20/10 3:58p rgreen
 * SW35230-2374: Use state machine to describe status of HDCP KeySet
 * verification from OTP
 * 
 * Hydra_Software_Devel/1   12/6/10 10:29a rgreen
 * SW35230-2374: Add support for HDCP Key verfication
 * 
 ***************************************************************************/
#ifndef BHDR_HDCP_H__
#define BHDR_HDCP_H__

#include "bhdr.h"


typedef enum BHDR_HDCP_KeyStorage
{
	BHDR_HDCP_KeyStorage_eNone,
	BHDR_HDCP_KeyStorage_eOtpROM,
	BHDR_HDCP_KeyStorage_eKeyRAM,
	BHDR_HDCP_KeyStorage_eMax 
} BHDR_HDCP_KeyStorage ;


typedef enum BHDR_HDCP_OtpState
{
	BHDR_HDCP_OtpState_eNone,
	BHDR_HDCP_OtpState_eHwError,
	BHDR_HDCP_OtpState_eCalcInitialized,
	BHDR_HDCP_OtpState_eCalcRunning,
	BHDR_HDCP_OtpState_eCrcMatch,
	BHDR_HDCP_OtpState_eCrcMismatch,
	BHDR_HDCP_OtpState_eMax 
} BHDR_HDCP_OtpState ;


typedef enum BHDR_HDCP_AuthState
{
	BHDR_HDCP_AuthState_eKeysetInitialization,
	BHDR_HDCP_AuthState_eKeysetError,
	BHDR_HDCP_AuthState_eIdle,
	BHDR_HDCP_AuthState_eWaitForKeyloading,
	BHDR_HDCP_AuthState_eWaitForDownstreamKsvs,
	BHDR_HDCP_AuthState_eKsvFifoReady,
	BHDR_HDCP_AuthState_eMax 
} BHDR_HDCP_AuthState ;


/******************************************************************************
Summary:
	Encrypted HDCP Key structure 
*******************************************************************************/
typedef struct BHDR_HDCP_EncryptedKeyStruct
{
	uint32_t CaDataLo;
	uint32_t CaDataHi;
	uint32_t TCaDataLo;
	uint32_t TCaDataHi;
	uint32_t HdcpKeyLo;
	uint32_t HdcpKeyHi;
} BHDR_HDCP_EncryptedKeyStruct ;


typedef struct 
{
	BHDR_HDCP_KeyStorage eKeyStorage ;
	BHDR_HDCP_OtpState eOtpState ;
	BHDR_HDCP_OtpState eAuthState ;
	uint32_t programmedCrc ;
	uint32_t calculatedCrc ;
	uint8_t anValue[BAVC_HDMI_HDCP_AN_LENGTH] ;
	uint8_t aksvValue[BAVC_HDMI_HDCP_KSV_LENGTH] ;
	uint8_t bksvValue[BAVC_HDMI_HDCP_KSV_LENGTH] ;

} BHDR_HDCP_Status ;


typedef struct BHDR_HDCP_Settings 
{
	uint8_t rxBksv[BAVC_HDMI_HDCP_KSV_LENGTH] ;
	uint8_t uiMaxLevels ;
	uint8_t uiMaxDevices ;
	uint8_t bRepeater ;
} BHDR_HDCP_Settings ;




/***************************************************************************
Summary:
HDMI HDCP Key Set Information  (keys, Tx and Rx KSVs, etc.).
****************************************************************************/

void BHDR_HDCP_GetDefaultSettings(
	BHDR_HDCP_Settings *pHdcpSettings) ;

void BHDR_HDCP_GetSettings(
	BHDR_Handle hHDR, 
	BHDR_HDCP_Settings *pHdcpSettings) ;

void BHDR_HDCP_SetSettings(
	BHDR_Handle hHDR, 
	BHDR_HDCP_Settings *pHdcpSettings) ;

BERR_Code BHDR_HDCP_GetStatus(
	BHDR_Handle hHDR, 
	BHDR_HDCP_Status *pStatus) ;

BERR_Code BHDR_HDCP_UnInstallHdcpStatusChangeCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr)  ; /* [in] cb for Packet Error change Notification */ 
	
BERR_Code BHDR_HDCP_InstallHdcpStatusChangeCallback( 
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for packet error changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2) ;    /* [in] the second argument(int) passed to the callback function */ 

BERR_Code BHDR_HDCP_EnableKeyLoading(BHDR_Handle hHDR)  ;

BERR_Code BHDR_HDCP_DisableKeyLoading(BHDR_Handle hHDR)  ;


typedef struct BHDR_HDCP_RepeaterDownStreamInfo
{
    uint8_t depth;                  /* number of levels of devices */
    bool maxDepthExceeded;  
	
    uint8_t devices;            /* number of devices  */
    bool maxDevicesExceeded;

    bool isRepeater;
    uint8_t repeaterKsv[BAVC_HDMI_HDCP_KSV_LENGTH] ;    
} BHDR_HDCP_RepeaterDownStreamInfo ;


#if BHDR_CONFIG_HDCP_REPEATER
/**************************************************************************
Summary:
	Register a callback function to be called after a HDMI ksv FIFO request 

Description:
	This function is used to enable a callback function that will
	be called any time an attached receiver requests the downstream KSV FIFO

Input:
	hHDR - HDMI Rx Handle
	pfCallback_isr - pointer to callback function to be called at the time of the request
	pvParam1 - User defined data structure casted to void.
	iParam2 - Additional user defined value... 
	
Returns:
	BERR_SUCCESS - Callback Installation Successful

See Also:
		BHDR_HDCP_UnInstallRepeaterRequestKsvFifoCallback

**************************************************************************/
BERR_Code BHDR_HDCP_InstallRepeaterRequestKsvFifoCallback(
	BHDR_Handle hHDR,			/* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr, /* [in] cb for packet changes */
	void *pvParm1, /* [in] the first argument (void *) passed to the callback function */ 
	int iParm2) ;   /* [in] the second argument(int) passed to the callback function */ 


/**************************************************************************
Summary:
	Remove a previously registered callback function for Ksv Request

Description:
	This function is used to remove the callback function that is called when there is KSV FIFO request 

Input:
	hHDR - HDMI Rx Handle
	pfCallback - pointer to callback function to be removed
	
Returns:
	BERR_SUCCESS - Callback Removal Successful

See Also:
	BHDR_HDCP_InstallRepeaterRequestKsvFifoCallback
**************************************************************************/
BERR_Code BHDR_HDCP_UnInstallRepeaterRequestKsvFifoCallback( 
	BHDR_Handle hHDR,                       /* [in] HDMI Rx Handle */
	const BHDR_CallbackFunc pfCallback_isr) ; /* [in] cb for format changes */


/**************************************************************************
Summary:
	Load the KSV FIFO with downstream KSVs

Description:
	This function will load each KSV into the KSV FIFO for access by an attached transmtter.

Input:
	hHDR - HDMI Rx Handle
	pDownstreamInfo  - pointer to structure containing the downstream Info (devices, levels, number of KSVs)
	
Returns:
	none

See Also:
	BHDR_HDCP_InstallRepeaterRequestKsvFifoCallback
	BHDR_HDCP_UnInstallRepeaterRequestKsvFifoCallback
**************************************************************************/
void BHDR_HDCP_LoadRepeaterKsvFifo(
	BHDR_Handle hHDR, 
	BHDR_HDCP_RepeaterDownStreamInfo *pDownstreamInfo, 
	uint8_t *pKsvs) ;
#endif




#ifdef __cplusplus
}
#endif

#endif /* BHDR_HDCP_H__ */

